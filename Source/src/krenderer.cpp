#include "krenderer.h"
#include "kopengldriver.h"

namespace kemena
{
    kRenderer::kRenderer()
    {
    }

    bool kRenderer::init(kWindow *window, kRendererType type)
    {
        if (window != nullptr)
            appWindow = window;

        renderType = type;

        if (renderType == kRendererType::RENDERER_GL)
        {
            driver = new kOpenGLDriver();
            if (!driver->init(window))
            {
                delete driver;
                driver = nullptr;
                return false;
            }
            kDriver::setCurrent(driver);

            if (window != nullptr)
            {
                fboWidth  = window->getWindowWidth();
                fboHeight = window->getWindowHeight();
            }
        }

        return true;
    }

    void kRenderer::destroy()
    {
        if (driver == nullptr) return;

        if (enableScreenBuffer)
        {
            if (fboMsaa)         { driver->deleteFramebuffer(fboMsaa);        fboMsaa = 0; }
            if (fboTexColorMsaa) { driver->deleteFBOTexture(fboTexColorMsaa); fboTexColorMsaa = 0; }
            if (rboDepthMsaa)    { driver->deleteRenderbuffer(rboDepthMsaa);  rboDepthMsaa = 0; }

            if (fbo)             { driver->deleteFramebuffer(fbo);            fbo = 0; }
            if (fboTexColor)     { driver->deleteFBOTexture(fboTexColor);     fboTexColor = 0; }
            if (rboDepth)        { driver->deleteRenderbuffer(rboDepth);      rboDepth = 0; }

            if (quadVao)         { driver->deleteVertexArray(quadVao);        quadVao = 0; }
            if (quadVbo)         { driver->deleteBuffer(quadVbo);             quadVbo = 0; }
            if (quadEbo)         { driver->deleteBuffer(quadEbo);             quadEbo = 0; }
        }

        if (enableShadow)
        {
            if (shadowFbo)    { driver->deleteFramebuffer(shadowFbo);  shadowFbo = 0; }
            if (shadowFboTex) { driver->deleteFBOTexture(shadowFboTex); shadowFboTex = 0; }
        }

        if (enablePicking)
        {
            if (pickFbo)      { driver->deleteFramebuffer(pickFbo);    pickFbo = 0; }
            if (pickFboTex)   { driver->deleteFBOTexture(pickFboTex);  pickFboTex = 0; }
            if (pickRboDepth) { driver->deleteRenderbuffer(pickRboDepth); pickRboDepth = 0; }
        }

        if (outlineShader)
        {
            delete outlineShader;
            outlineShader = nullptr;
        }

        if (debugAlbedoShader)  { delete debugAlbedoShader;  debugAlbedoShader  = nullptr; }
        if (debugNormalsShader) { delete debugNormalsShader; debugNormalsShader = nullptr; }
        if (debugWireShader)    { delete debugWireShader;    debugWireShader    = nullptr; }
        if (debugDepthShader)   { delete debugDepthShader;   debugDepthShader   = nullptr; }

        driver->destroy();
        delete driver;
        driver = nullptr;
    }

    void kRenderer::setEngineInfo(const kString name, uint32_t version)
    {
        engineName = name;
        engineVersion = version;
    }

    kWindow *kRenderer::getWindow()
    {
        return appWindow;
    }

    kDriver *kRenderer::getDriver()
    {
        return driver;
    }

    void kRenderer::clear()
    {
        if (enableScreenBuffer)
            driver->bindFramebuffer(fboMsaa);
        else
            driver->unbindFramebuffer();

        driver->setClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
        driver->clear(true, true, true);
        driver->unbindFramebuffer();
    }

    void kRenderer::render(kWorld *world, kScene *scene, int x, int y, int width, int height, float deltaTime, bool autoClearSwapWindow)
    {
        if (frameId > 999999999999)
            frameId = 0;
        else
            frameId++;

        // Render shadow scene
        if (shadowShader != nullptr)
        {
            for (size_t i = 0; i < scene->getLights().size(); ++i)
            {
                kLight *currentLight = scene->getLights().at(i);

                if (currentLight->getLightType() == kLightType::LIGHT_TYPE_SUN)
                {
                    driver->bindFramebuffer(shadowFbo);
                    driver->setDepthTest(true);
                    driver->setViewport(0, 0, shadowWidth, shadowHeight);
                    driver->clear(false, true, false);

                    shadowShader->use();

                    kVec3 lightPos = currentLight->getPosition();
                    kVec3 lightDir = glm::normalize(currentLight->getDirection());

                    float near_plane = 1.0f, far_plane = 20.0f;
                    kMat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
                    kMat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, kVec3(0.0f, 1.0f, 0.0f));

                    lightSpaceMatrix = lightProjection * lightView;

                    renderSceneGraphShadow(world, scene, scene->getRootNode(), lightSpaceMatrix, lightView, lightProjection, false, deltaTime);

                    shadowShader->unuse();
                }
            }
        }

        driver->unbindFramebuffer();

        if (enableScreenBuffer)
        {
            resizeFbo(width, height);
            driver->bindFramebuffer(fboMsaa);
        }

        driver->setDepthTest(true);

        if (autoClearSwapWindow)
        {
            driver->setClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
            driver->clear(true, true, true);
        }

        if (world->getMainCamera() != nullptr)
        {
            driver->setViewport(x, y, width, height);
            world->getMainCamera()->setAspectRatio((float)width / (float)height);

            // Render skybox
            kMaterial *skyboxMaterial = scene->getSkyboxMaterial();
            kMesh *skyboxMesh = scene->getSkyboxMesh();
            if (skyboxMaterial != nullptr && skyboxMesh != nullptr)
            {
                if (skyboxMesh->getLoaded() && skyboxMaterial->getShader() != nullptr)
                {
                    kShader *skyboxShader = skyboxMaterial->getShader();
                    skyboxShader->use();

                    driver->setDepthWrite(false);
                    driver->setCullFace(false);

                    skyboxShader->setValue("viewMatrix", kMat4(kMat3(world->getMainCamera()->getViewMatrix())));
                    skyboxShader->setValue("projectionMatrix", world->getMainCamera()->getProjectionMatrix());

                    if (skyboxMaterial->getTextures().size() > 0 &&
                        skyboxMaterial->getTexture(0)->getType() == kTextureType::TEX_TYPE_CUBE)
                    {
                        uint32_t tex = skyboxMaterial->getTexture(0)->getTextureID();
                        driver->bindTextureCube(0, tex);
                        driver->setUniformInt(skyboxShader->getShaderProgram(), skyboxMaterial->getTexture(0)->getTextureName(), 0);
                    }

                    skyboxMesh->calculateModelMatrix();
                    skyboxMesh->draw();

                    for (size_t i = 0; i < skyboxMesh->getChildren().size(); ++i)
                    {
                        kMesh *childMesh = (kMesh *)skyboxMesh->getChildren().at(i);
                        if (childMesh != nullptr)
                        {
                            childMesh->calculateModelMatrix();
                            childMesh->draw();
                        }
                    }

                    skyboxShader->unuse();
                }
            }

            driver->setDepthTest(true);
            driver->setDepthWrite(true);
            driver->setCullFace(true);

            if (renderMode == kRenderMode::RENDER_MODE_FULL ||
                renderMode == kRenderMode::RENDER_MODE_FULL_WIREFRAME)
            {
                renderSceneGraph(world, scene, scene->getRootNode(), false, deltaTime);
            }

            if (renderMode != kRenderMode::RENDER_MODE_FULL)
            {
                kShader *dbgShader = nullptr;
                bool wireframe = false;

                switch (renderMode)
                {
                    case kRenderMode::RENDER_MODE_ALBEDO:
                        if (!debugAlbedoShader) {
                            debugAlbedoShader = new kShader();
                            debugAlbedoShader->loadShadersCode(kDebugVS, kDebugAlbedoFS);
                        }
                        dbgShader = debugAlbedoShader;
                        break;
                    case kRenderMode::RENDER_MODE_NORMALS:
                        if (!debugNormalsShader) {
                            debugNormalsShader = new kShader();
                            debugNormalsShader->loadShadersCode(kDebugVS, kDebugNormalsFS);
                        }
                        dbgShader = debugNormalsShader;
                        break;
                    case kRenderMode::RENDER_MODE_WIREFRAME:
                        if (!debugWireShader) {
                            debugWireShader = new kShader();
                            debugWireShader->loadShadersCode(kDebugVS, kDebugWireFS);
                        }
                        dbgShader = debugWireShader;
                        wireframe = true;
                        break;
                    case kRenderMode::RENDER_MODE_DEPTH:
                        if (!debugDepthShader) {
                            debugDepthShader = new kShader();
                            debugDepthShader->loadShadersCode(kDebugVS, kDebugDepthFS);
                        }
                        dbgShader = debugDepthShader;
                        // Set camera near/far once before the traversal
                        dbgShader->use();
                        dbgShader->setValue("near", world->getMainCamera()->getNearClip());
                        dbgShader->setValue("far",  world->getMainCamera()->getFarClip());
                        dbgShader->unuse();
                        break;
                    case kRenderMode::RENDER_MODE_FULL_WIREFRAME:
                        if (!debugWireShader) {
                            debugWireShader = new kShader();
                            debugWireShader->loadShadersCode(kDebugVS, kDebugWireFS);
                        }
                        dbgShader = debugWireShader;
                        wireframe = true;
                        break;
                    default:
                        break;
                }

                if (dbgShader)
                    renderSceneGraphDebug(world, scene, scene->getRootNode(), dbgShader, wireframe);
            }
        }
        else
        {
            std::cout << "No main camera found" << std::endl;
        }

        driver->unbindFramebuffer();

        if (enableScreenBuffer)
        {
            // Blit MSAA to resolve FBO
            driver->bindReadFramebuffer(fboMsaa);
            driver->bindDrawFramebuffer(fbo);
            driver->blitFramebufferColor(0, 0, fboWidth, fboHeight, 0, 0, fboWidth, fboHeight);

            // Render resolve texture to screen quad
            driver->unbindFramebuffer();
            driver->setDepthTest(false);
            driver->setClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            driver->clear(true, false, false);

            getScreenShader()->use();
            driver->bindTexture2D(0, fboTexColor);

            if (enableAutoExposure)
            {
                driver->generateMipmaps2D(fboTexColor);
                int mipLevel = (int)std::floor(std::log2(std::max(width, height)));
                driver->readTexture2DRGB(fboTexColor, mipLevel, averageLuminanceColor);
                averageLuminance = 0.2126f * averageLuminanceColor[0]
                                 + 0.7152f * averageLuminanceColor[1]
                                 + 0.0722f * averageLuminanceColor[2];
                float targetExposure = exposureKey / (averageLuminance + 0.001f);
                exposure = glm::mix(exposure, targetExposure, deltaTime * exposureAdaptationRate);
                if (screenShader != nullptr)
                {
                    screenShader->setValue("enable_autoExposure", enableAutoExposure);
                    screenShader->setValue("exposure", exposure * 3.0f);
                    screenShader->setValue("contrast", 1.01f);
                    screenShader->setValue("gamma", 2.2f);
                }
            }

            driver->drawIndexed(quadVao, 6);
            getScreenShader()->unuse();
        }

        driver->unbindVertexArray();

        if (autoClearSwapWindow && appWindow != nullptr)
            appWindow->swap();
    }

    void kRenderer::renderSceneGraph(kWorld *world, kScene *scene, kObject *currentNode, bool transparent, float deltaTime)
    {
        if (currentNode == nullptr || !currentNode->getActive()) return;

        currentNode->calculateModelMatrix();

        if (currentNode->getType() == kNodeType::NODE_TYPE_MESH)
        {
            kMesh *currentMesh = (kMesh *)currentNode;

            if (currentMesh->getLoaded() && currentMesh->getMaterial() != nullptr)
            {
                // Blend
                if (currentMesh->getMaterial()->getTransparent() == kTransparentType::TRANSP_TYPE_BLEND)
                {
                    driver->setBlend(true);
                    driver->setBlendFunc(kBlendFactor::SRC_ALPHA, kBlendFactor::ONE_MINUS_SRC_ALPHA);
                }
                else
                {
                    driver->setBlend(false);
                }

                // Face culling
                if (currentMesh->getMaterial()->getSingleSided())
                {
                    driver->setCullFace(true);
                    driver->setFrontFace(kFrontFace::CCW);
                    driver->setCullMode(currentMesh->getMaterial()->getCullBack() ? kCullMode::BACK : kCullMode::FRONT);
                }
                else
                {
                    driver->setCullFace(false);
                }

                if (currentMesh->getMaterial()->getShader() != nullptr)
                {
                    kShader *shader = currentMesh->getMaterial()->getShader();
                    shader->use();

                    shader->setValue("normalMatrix", currentMesh->getNormalMatrix());
                    shader->setValue("modelMatrix", currentMesh->getModelMatrixWorld());
                    shader->setValue("viewMatrix", world->getMainCamera()->getViewMatrix());
                    shader->setValue("projectionMatrix", world->getMainCamera()->getProjectionMatrix());
                    shader->setValue("viewPos", world->getMainCamera()->getPosition());

                    shader->setValue("material.tiling",    currentMesh->getMaterial()->getUvTiling());
                    shader->setValue("material.ambient",   currentMesh->getMaterial()->getAmbientColor());
                    shader->setValue("material.diffuse",   currentMesh->getMaterial()->getDiffuseColor());
                    shader->setValue("material.specular",  currentMesh->getMaterial()->getSpecularColor());
                    shader->setValue("material.shininess", currentMesh->getMaterial()->getShininess());
                    shader->setValue("material.metallic",  currentMesh->getMaterial()->getMetallic());
                    shader->setValue("material.roughness", currentMesh->getMaterial()->getRoughness());

                    std::vector<kMat4> boneTransforms(128, kMat4(1.0f));
                    if (currentMesh->getSkinned() && currentMesh->getAnimator() != nullptr)
                    {
                        currentMesh->getAnimator()->updateAnimation(
                            deltaTime * currentMesh->getAnimator()->getCurrentAnimation()->getSpeed(), frameId);
                        boneTransforms = currentMesh->getAnimator()->getFinalBoneMatrices();
                    }
                    shader->setValue("finalBonesMatrices", boneTransforms);

                    // Lights
                    int countSun = 0, countPoint = 0, countSpot = 0;
                    for (size_t j = 0; j < scene->getLights().size(); ++j)
                    {
                        kLight *light = scene->getLights().at(j);
                        if (light == nullptr || !light->getActive()) continue;

                        if (light->getLightType() == LIGHT_TYPE_SUN)
                        {
                            kString idx = std::to_string(countSun);
                            shader->setValue("sunLights[" + idx + "].power",     light->getPower());
                            shader->setValue("sunLights[" + idx + "].direction", light->getDirection());
                            shader->setValue("sunLights[" + idx + "].ambient",   light->getAmbientColor());
                            shader->setValue("sunLights[" + idx + "].diffuse",   light->getDiffuseColor());
                            shader->setValue("sunLights[" + idx + "].specular",  light->getSpecularColor());
                            countSun++;
                        }
                        else if (light->getLightType() == LIGHT_TYPE_POINT)
                        {
                            kString idx = std::to_string(countPoint);
                            shader->setValue("pointLights[" + idx + "].power",     light->getPower());
                            shader->setValue("pointLights[" + idx + "].position",  light->getPosition());
                            shader->setValue("pointLights[" + idx + "].constant",  light->getConstant());
                            shader->setValue("pointLights[" + idx + "].linear",    light->getLinear());
                            shader->setValue("pointLights[" + idx + "].quadratic", light->getQuadratic());
                            shader->setValue("pointLights[" + idx + "].ambient",   light->getAmbientColor());
                            shader->setValue("pointLights[" + idx + "].diffuse",   light->getDiffuseColor());
                            shader->setValue("pointLights[" + idx + "].specular",  light->getSpecularColor());
                            countPoint++;
                        }
                        else if (light->getLightType() == LIGHT_TYPE_SPOT)
                        {
                            kString idx = std::to_string(countSpot);
                            shader->setValue("spotLights[" + idx + "].power",       light->getPower());
                            shader->setValue("spotLights[" + idx + "].position",    light->getPosition());
                            shader->setValue("spotLights[" + idx + "].direction",   light->getDirection());
                            shader->setValue("spotLights[" + idx + "].cutOff",      light->getCutOff());
                            shader->setValue("spotLights[" + idx + "].outerCutOff", light->getOuterCutOff());
                            shader->setValue("spotLights[" + idx + "].constant",    light->getConstant());
                            shader->setValue("spotLights[" + idx + "].linear",      light->getLinear());
                            shader->setValue("spotLights[" + idx + "].quadratic",   light->getQuadratic());
                            shader->setValue("spotLights[" + idx + "].ambient",     light->getAmbientColor());
                            shader->setValue("spotLights[" + idx + "].diffuse",     light->getDiffuseColor());
                            shader->setValue("spotLights[" + idx + "].specular",    light->getSpecularColor());
                            countSpot++;
                        }
                    }
                    shader->setValue("sunLightNum",   countSun);
                    shader->setValue("pointLightNum", countPoint);
                    shader->setValue("spotLightNum",  countSpot);

                    // Shadow map
                    shader->setValue("lightSpaceMatrix", lightSpaceMatrix);
                    unsigned int shadowUnit = (unsigned int)currentMesh->getMaterial()->getTextures().size();
                    driver->bindTexture2D((int)shadowUnit, shadowFboTex);
                    shader->setValue("shadowMap", shadowUnit);

                    // Material textures
                    for (size_t k = 0; k < currentMesh->getMaterial()->getTextures().size(); k++)
                    {
                        kTexture *tex = currentMesh->getMaterial()->getTexture(k);
                        if (tex == nullptr) continue;

                        if (tex->getType() == kTextureType::TEX_TYPE_2D)
                            driver->bindTexture2D((int)k, tex->getTextureID());
                        else if (tex->getType() == kTextureType::TEX_TYPE_CUBE)
                            driver->bindTextureCube((int)k, tex->getTextureID());

                        shader->setValue(tex->getTextureName().c_str(), (unsigned int)k);
                        shader->setValue("has_" + tex->getTextureName(), true);
                    }

                    currentMesh->draw();

                    // Unbind all texture units
                    int totalUnits = (int)currentMesh->getMaterial()->getTextures().size() + 1;
                    for (int k = totalUnits - 1; k >= 0; k--)
                    {
                        driver->unbindTexture2D(k);
                        driver->unbindTextureCube(k);
                    }

                    shader->unuse();
                }
            }
        }
        else if (currentNode->getType() == kNodeType::NODE_TYPE_LIGHT)
        {
            kLight *currentLight = (kLight *)currentNode;

            if (world->getMainCamera() != nullptr && currentLight->getMaterial() != nullptr)
            {
                kMat4 view = lookAt(world->getMainCamera()->getPosition(),
                                   world->getMainCamera()->getLookAt(),
                                   world->getMainCamera()->calculateUp());
                kMat4 projection = glm::perspective(glm::radians(world->getMainCamera()->getFOV()),
                                                   world->getMainCamera()->getAspectRatio(),
                                                   world->getMainCamera()->getNearClip(),
                                                   world->getMainCamera()->getFarClip());

                if (currentLight->getMaterial()->getTransparent() == kTransparentType::TRANSP_TYPE_BLEND)
                {
                    driver->setBlend(true);
                    driver->setBlendFunc(kBlendFactor::SRC_ALPHA, kBlendFactor::ONE_MINUS_SRC_ALPHA);
                }
                else
                {
                    driver->setBlend(false);
                }

                if (currentLight->getMaterial()->getShader() != nullptr)
                {
                    kShader *shader = currentLight->getMaterial()->getShader();
                    shader->use();

                    shader->setValue("viewProjection",           projection * view);
                    shader->setValue("cameraRightWorldSpace",    kVec3(view[0][0], view[1][0], view[2][0]));
                    shader->setValue("cameraUpWorldSpace",       kVec3(view[0][1], view[1][1], view[2][1]));
                    shader->setValue("billboardPosition",        currentLight->getPosition());
                    shader->setValue("billboardSize",            kVec2(0.8f, 0.8f));
                    shader->setValue("color",                    currentLight->getDiffuseColor());

                    for (size_t l = 0; l < currentLight->getMaterial()->getTextures().size(); l++)
                    {
                        kTexture *tex = currentLight->getMaterial()->getTexture(l);
                        if (tex != nullptr && tex->getType() == kTextureType::TEX_TYPE_2D)
                        {
                            driver->bindTexture2D((int)l, tex->getTextureID());
                            driver->setUniformInt(shader->getShaderProgram(), "albedoMap", (int)l);
                        }
                    }

                    currentLight->draw();

                    driver->unbindTexture2D(0);
                    shader->unuse();
                }
            }
        }
        else if (currentNode->getType() == kNodeType::NODE_TYPE_OBJECT)
        {
            kObject *currentObject = currentNode;

            if (currentObject->getMaterial() != nullptr)
            {
                if (currentObject->getMaterial()->getTransparent() == kTransparentType::TRANSP_TYPE_BLEND)
                {
                    driver->setBlend(true);
                    driver->setBlendFunc(kBlendFactor::SRC_ALPHA, kBlendFactor::ONE_MINUS_SRC_ALPHA);
                }
                else
                {
                    driver->setBlend(false);
                }

                if (currentObject->getMaterial()->getShader() != nullptr)
                {
                    kShader *shader = currentObject->getMaterial()->getShader();
                    shader->use();

                    shader->setValue("modelMatrix",      currentObject->getModelMatrixWorld());
                    shader->setValue("viewMatrix",       world->getMainCamera()->getViewMatrix());
                    shader->setValue("projectionMatrix", world->getMainCamera()->getProjectionMatrix());

                    currentObject->draw();

                    driver->unbindTexture2D(0);
                    shader->unuse();
                }
            }
        }

        // Recurse children
        for (size_t i = 0; i < currentNode->getChildren().size(); ++i)
        {
            if (currentNode->getChildren().at(i) != nullptr)
                renderSceneGraph(world, scene, currentNode->getChildren().at(i), transparent, deltaTime);
        }
    }

    void kRenderer::renderSceneGraphShadow(kWorld *world, kScene *scene, kObject *currentNode, kMat4 lightSpaceMatrix, kMat4 lightView, kMat4 lightProjection, bool transparent, float deltaTime)
    {
        if (currentNode == nullptr || !currentNode->getActive()) return;

        currentNode->calculateModelMatrix();

        if (currentNode->getType() == kNodeType::NODE_TYPE_MESH)
        {
            kMesh *currentMesh = (kMesh *)currentNode;

            if (currentMesh->getCastShadow())
            {
                shadowShader->setValue("lightSpaceMatrix", lightSpaceMatrix);
                shadowShader->setValue("modelMatrix",      currentMesh->getModelMatrixWorld());
                shadowShader->setValue("viewMatrix",       lightView);
                shadowShader->setValue("projectionMatrix", lightProjection);

                std::vector<kMat4> boneTransforms(128, kMat4(1.0f));
                if (currentMesh->getSkinned() && currentMesh->getAnimator() != nullptr)
                {
                    currentMesh->getAnimator()->updateAnimation(
                        deltaTime * currentMesh->getAnimator()->getCurrentAnimation()->getSpeed(), frameId);
                    boneTransforms = currentMesh->getAnimator()->getFinalBoneMatrices();
                }
                shadowShader->setValue("finalBonesMatrices", boneTransforms);

                currentMesh->draw();
            }
        }

        for (size_t i = 0; i < currentNode->getChildren().size(); ++i)
        {
            if (currentNode->getChildren().at(i) != nullptr)
                renderSceneGraphShadow(world, scene, currentNode->getChildren().at(i),
                                       lightSpaceMatrix, lightView, lightProjection, transparent, deltaTime);
        }
    }

    kVec4 kRenderer::getClearColor()
    {
        return clearColor;
    }

    void kRenderer::setClearColor(kVec4 newColor)
    {
        newColor.r = srgbToLinear(newColor.r);
        newColor.g = srgbToLinear(newColor.g);
        newColor.b = srgbToLinear(newColor.b);
        clearColor = newColor;
        if (driver)
            driver->setClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    }

    void kRenderer::setEnableScreenBuffer(bool newEnable, bool useDefaultShader)
    {
        enableScreenBuffer = newEnable;

        if (newEnable)
        {
            driver->setMultisample(true);
            driver->setSRGBEncoding(false);

            // Screen quad
            float quadVerts[] = {
                -1.0f, -1.0f,  0.0f, 0.0f,
                 1.0f, -1.0f,  1.0f, 0.0f,
                 1.0f,  1.0f,  1.0f, 1.0f,
                -1.0f,  1.0f,  0.0f, 1.0f,
            };
            uint32_t quadIndices[] = {0, 1, 2, 2, 3, 0};

            quadVao = driver->createVertexArray();
            quadVbo = driver->createBuffer();
            quadEbo = driver->createBuffer();
            driver->bindVertexArray(quadVao);
            driver->uploadVertexBuffer(quadVbo, quadVerts, sizeof(quadVerts));
            driver->setVertexAttribFloat(0, 2, 4 * sizeof(float), 0);
            driver->setVertexAttribFloat(1, 2, 4 * sizeof(float), 2 * sizeof(float));
            driver->uploadIndexBuffer(quadEbo, quadIndices, sizeof(quadIndices));
            driver->unbindVertexArray();

            // MSAA FBO
            fboMsaa         = driver->createFramebuffer();
            fboTexColorMsaa = driver->createFBOColorTextureMSAA(8, fboWidth, fboHeight);
            driver->attachFBOColorTextureMSAA(fboMsaa, fboTexColorMsaa);
            rboDepthMsaa = driver->createRenderbuffer();
            driver->setupRenderbufferMSAA(rboDepthMsaa, 8, fboWidth, fboHeight);
            driver->attachRenderbufferDepthStencil(fboMsaa, rboDepthMsaa);
            driver->bindFramebuffer(fboMsaa);
            driver->setFramebufferDrawBuffer();
            if (!driver->isFramebufferComplete())
            {
                std::cerr << "MSAA FBO not complete!" << std::endl;
                return;
            }
            driver->unbindFramebuffer();

            // Resolve FBO
            fbo         = driver->createFramebuffer();
            fboTexColor = driver->createFBOColorTexture(fboWidth, fboHeight);
            driver->attachFBOColorTexture(fbo, fboTexColor);
            rboDepth = driver->createRenderbuffer();
            driver->setupRenderbuffer(rboDepth, fboWidth, fboHeight);
            driver->attachRenderbufferDepthStencil(fbo, rboDepth);
            driver->bindFramebuffer(fbo);
            driver->setFramebufferDrawBuffer();
            if (!driver->isFramebufferComplete())
            {
                std::cerr << "Resolve FBO not complete!" << std::endl;
                return;
            }
            driver->unbindFramebuffer();

            if (useDefaultShader)
            {
                kString vertexShader = R"(#version 330 core
layout(location = 0) in kVec2 aPos;
layout(location = 1) in kVec2 aTexCoord;
out kVec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = kVec4(aPos, 0.0, 1.0);
})";

                kString fragmentShader = R"(#version 330 core
in kVec2 TexCoord;
out kVec4 FragColor;

uniform sampler2D screenTexture;
uniform int enable_autoExposure;
uniform float exposure;
uniform float contrast;
uniform float gamma;

void main()
{
    kVec3 color = texture(screenTexture, TexCoord).rgb;
    kVec3 mapped = color * exposure;
    mapped = (mapped - 0.5) * contrast + 0.5;
    mapped = pow(mapped, kVec3(1.0 / gamma));
    FragColor = kVec4(mapped, 1.0);
})";

                kShader *newScreenShader = new kShader();
                newScreenShader->loadShadersCode(vertexShader.c_str(), fragmentShader.c_str());
                setScreenShader(newScreenShader);
            }
        }
        else
        {
            driver->setSRGBEncoding(true);
        }
    }

    bool kRenderer::getEnableScreenBuffer()
    {
        return enableScreenBuffer;
    }

    void kRenderer::setScreenShader(kShader *newShader)
    {
        screenShader = newShader;
    }

    kShader *kRenderer::getScreenShader()
    {
        return screenShader;
    }

    void kRenderer::setEnableShadow(bool newEnable, bool useDefaultShader)
    {
        enableShadow = newEnable;

        if (newEnable)
        {
            shadowFboTex = driver->createFBODepthTexture(shadowWidth, shadowHeight);
            shadowFbo    = driver->createFramebuffer();
            driver->attachFBODepthTexture(shadowFbo, shadowFboTex);

            if (!driver->isFramebufferComplete())
                std::cerr << "Shadow framebuffer is incomplete" << std::endl;

            driver->unbindFramebuffer();

            if (useDefaultShader)
            {
                kString vertexShader = R"(#version 330 core
layout (location = 0) in kVec3 vertexPosition;
layout (location = 6) in kIvec4 boneIDs;
layout (location = 7) in kVec4 weights;

uniform kMat4 lightSpaceMatrix;
uniform kMat4 modelMatrix;
uniform kMat4 viewMatrix;
uniform kMat4 projectionMatrix;

const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;

uniform kMat4 finalBonesMatrices[MAX_BONES];

out kVec3 vertexPositionFrag;

void main()
{
    kVec4 totalPosition = kVec4(vertexPosition, 1.0f);
    float totalWeight = 0.0;

    for(int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        int boneID = boneIDs[i];
        float weight = weights[i];

        if(boneID == -1 || weight <= 0.0)
            continue;

        if(boneID >= MAX_BONES)
        {
            totalPosition = kVec4(vertexPosition, 1.0f);
            break;
        }

        totalPosition += (finalBonesMatrices[boneID] * kVec4(vertexPosition, 1.0f)) * weight;
        kMat3 normalMatrixBone = transpose(inverse(kMat3(finalBonesMatrices[boneID])));
        totalWeight += weight;
    }

    if (totalWeight == 0.0)
        totalPosition = kVec4(vertexPosition, 1.0f);

    kVec4 worldPosition = modelMatrix * totalPosition;
    vertexPositionFrag = (lightSpaceMatrix * worldPosition).xyz;
    gl_Position = lightSpaceMatrix * worldPosition;
})";

                kString fragmentShader = R"(#version 330 core
in kVec3 vertexPositionFrag;
out kVec4 fragColor;

void main()
{
})";

                kShader *newShadowShader = new kShader();
                newShadowShader->loadShadersCode(vertexShader.c_str(), fragmentShader.c_str());
                setShadowShader(newShadowShader);
            }
        }
    }

    bool kRenderer::getEnableShadow()
    {
        return enableShadow;
    }

    void kRenderer::setShadowShader(kShader *newShader)
    {
        shadowShader = newShader;
    }

    kShader *kRenderer::getShadowShader()
    {
        return shadowShader;
    }

    void kRenderer::setEnableAutoExposure(bool newEnable)
    {
        enableAutoExposure = newEnable;
    }

    bool kRenderer::getEnableAutoExposure()
    {
        return enableAutoExposure;
    }

    void kRenderer::resizeFbo(int newWidth, int newHeight)
    {
        if (newWidth == fboWidth && newHeight == fboHeight) return;
        if (newWidth <= 0 || newHeight <= 0) return;

        driver->setMultisample(true);

        // Resize MSAA color texture and depth RBO
        driver->resizeFBOColorTextureMSAA(fboTexColorMsaa, 8, newWidth, newHeight);
        driver->setupRenderbufferMSAA(rboDepthMsaa, 8, newWidth, newHeight);

        // Re-attach to MSAA FBO
        driver->attachFBOColorTextureMSAA(fboMsaa, fboTexColorMsaa);
        driver->attachRenderbufferDepthStencil(fboMsaa, rboDepthMsaa);
        driver->bindFramebuffer(fboMsaa);
        driver->setFramebufferDrawBuffer();
        driver->unbindFramebuffer();

        // Resize resolve color texture and depth RBO
        driver->resizeFBOColorTexture(fboTexColor, newWidth, newHeight);
        driver->setupRenderbuffer(rboDepth, newWidth, newHeight);

        // Re-attach to resolve FBO
        driver->attachFBOColorTexture(fbo, fboTexColor);
        driver->attachRenderbufferDepthStencil(fbo, rboDepth);
        driver->bindFramebuffer(fbo);
        driver->setFramebufferDrawBuffer();
        driver->unbindFramebuffer();

        fboWidth  = newWidth;
        fboHeight = newHeight;
    }

    uint32_t kRenderer::getFboTexture()
    {
        return fboTexColor;
    }

    int kRenderer::getFboWidth()
    {
        return fboWidth;
    }

    int kRenderer::getFboHeight()
    {
        return fboHeight;
    }

    float kRenderer::srgbToLinear(float c)
    {
        return (c <= 0.04045f) ? (c / 12.92f) : std::pow((c + 0.055f) / 1.055f, 2.4f);
    }

    kVec3 kRenderer::idToRgb(unsigned int i)
    {
        int r = (i & 0x000000FF) >> 0;
        int g = (i & 0x0000FF00) >> 8;
        int b = (i & 0x00FF0000) >> 16;
        return kVec3(r, g, b);
    }

    unsigned int kRenderer::rgbToId(unsigned int r, unsigned int g, unsigned int b)
    {
        return r + g * 256 + b * 256 * 256;
    }

    // -------------------------------------------------------------------------
    // Color ID object picking
    // -------------------------------------------------------------------------

    void kRenderer::setEnableObjectPicking(bool enable, bool useDefaultShader)
    {
        enablePicking = enable;

        if (enable)
        {
            // Allocate picking FBO at 1×1; it will be resized on first use.
            pickFbo     = driver->createFramebuffer();
            pickFboTex  = driver->createFBOColorTexture(1, 1);
            driver->attachFBOColorTexture(pickFbo, pickFboTex);
            pickRboDepth = driver->createRenderbuffer();
            driver->setupRenderbuffer(pickRboDepth, 1, 1);
            driver->attachRenderbufferDepthStencil(pickFbo, pickRboDepth);
            driver->bindFramebuffer(pickFbo);
            driver->setFramebufferDrawBuffer();
            if (!driver->isFramebufferComplete())
                std::cerr << "Picking FBO not complete!" << std::endl;
            driver->unbindFramebuffer();
            pickFboWidth  = 1;
            pickFboHeight = 1;

            if (useDefaultShader)
            {
                const char *vertSrc = R"(#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 6) in ivec4 boneIDs;
layout(location = 7) in vec4 weights;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

const int MAX_BONES = 128;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    vec4 totalPosition = vec4(vertexPosition, 1.0);
    float totalWeight = 0.0;

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        int boneID = boneIDs[i];
        float weight = weights[i];
        if (boneID == -1 || weight <= 0.0) continue;
        if (boneID >= MAX_BONES) { totalPosition = vec4(vertexPosition, 1.0); break; }
        totalPosition += (finalBonesMatrices[boneID] * vec4(vertexPosition, 1.0)) * weight;
        totalWeight += weight;
    }

    if (totalWeight == 0.0)
        totalPosition = vec4(vertexPosition, 1.0);

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * totalPosition;
})";

                const char *fragSrc = R"(#version 330 core
out vec4 fragColor;
uniform vec3 pickColor;

void main()
{
    fragColor = vec4(pickColor, 1.0);
})";

                kShader *newPickShader = new kShader();
                newPickShader->loadShadersCode(vertSrc, fragSrc);
                pickingShader = newPickShader;
            }
        }
        else
        {
            if (pickFbo)      { driver->deleteFramebuffer(pickFbo);      pickFbo = 0; }
            if (pickFboTex)   { driver->deleteFBOTexture(pickFboTex);    pickFboTex = 0; }
            if (pickRboDepth) { driver->deleteRenderbuffer(pickRboDepth); pickRboDepth = 0; }
            delete pickingShader;
            pickingShader = nullptr;
            pickFboWidth  = 0;
            pickFboHeight = 0;
        }
    }

    bool kRenderer::getEnableObjectPicking()
    {
        return enablePicking;
    }

    void kRenderer::renderSceneGraphPicking(kWorld *world, kScene *scene, kObject *currentNode)
    {
        if (currentNode == nullptr || !currentNode->getActive()) return;

        currentNode->calculateModelMatrix();

        if (currentNode->getType() == kNodeType::NODE_TYPE_MESH)
        {
            kMesh *currentMesh = (kMesh *)currentNode;
            if (currentMesh->getLoaded())
            {
                kVec3 idColor = idToRgb(currentMesh->getId());
                pickingShader->setValue("modelMatrix", currentMesh->getModelMatrixWorld());
                pickingShader->setValue("pickColor",   kVec3(idColor.r / 255.0f,
                                                             idColor.g / 255.0f,
                                                             idColor.b / 255.0f));

                std::vector<kMat4> boneTransforms(128, kMat4(1.0f));
                if (currentMesh->getSkinned() && currentMesh->getAnimator() != nullptr)
                    boneTransforms = currentMesh->getAnimator()->getFinalBoneMatrices();
                pickingShader->setValue("finalBonesMatrices", boneTransforms);

                currentMesh->draw();
            }
        }

        for (size_t i = 0; i < currentNode->getChildren().size(); ++i)
        {
            if (currentNode->getChildren().at(i) != nullptr)
                renderSceneGraphPicking(world, scene, currentNode->getChildren().at(i));
        }
    }

    static kObject *findObjectById(kObject *node, unsigned int id)
    {
        if (node == nullptr) return nullptr;
        if (node->getId() == id) return node;
        for (kObject *child : node->getChildren())
        {
            kObject *found = findObjectById(child, id);
            if (found) return found;
        }
        return nullptr;
    }

    kObject *kRenderer::pickObject(kWorld *world, kScene *scene,
                                   int mouseX, int mouseY,
                                   int viewWidth, int viewHeight)
    {
        if (!enablePicking || pickingShader == nullptr) return nullptr;
        if (world->getMainCamera() == nullptr) return nullptr;
        if (viewWidth <= 0 || viewHeight <= 0) return nullptr;

        // Resize picking FBO if the viewport dimensions changed.
        if (viewWidth != pickFboWidth || viewHeight != pickFboHeight)
        {
            driver->resizeFBOColorTexture(pickFboTex, viewWidth, viewHeight);
            driver->setupRenderbuffer(pickRboDepth, viewWidth, viewHeight);
            driver->attachFBOColorTexture(pickFbo, pickFboTex);
            driver->attachRenderbufferDepthStencil(pickFbo, pickRboDepth);
            pickFboWidth  = viewWidth;
            pickFboHeight = viewHeight;
        }

        // Disable sRGB encoding so ID colors are stored and read back exactly.
        driver->setSRGBEncoding(false);

        driver->bindFramebuffer(pickFbo);
        driver->setViewport(0, 0, viewWidth, viewHeight);
        driver->setClearColor(0.0f, 0.0f, 0.0f, 1.0f); // ID 0 = no object
        driver->clear(true, true, false);
        driver->setDepthTest(true);
        driver->setDepthWrite(true);
        driver->setBlend(false);
        driver->setCullFace(false);

        world->getMainCamera()->setAspectRatio((float)viewWidth / (float)viewHeight);

        pickingShader->use();
        pickingShader->setValue("viewMatrix",       world->getMainCamera()->getViewMatrix());
        pickingShader->setValue("projectionMatrix", world->getMainCamera()->getProjectionMatrix());

        renderSceneGraphPicking(world, scene, scene->getRootNode());

        pickingShader->unuse();

        // Read the pixel — OpenGL origin is bottom-left, screen Y is top-down, so flip Y.
        int glX = mouseX;
        int glY = viewHeight - 1 - mouseY;
        uint8_t r = 0, g = 0, b = 0, a = 0;
        driver->readPixelsRGBA(glX, glY, r, g, b, a);

        driver->unbindFramebuffer();

        // Restore sRGB state for the main render pipeline.
        if (!enableScreenBuffer)
            driver->setSRGBEncoding(true);

        unsigned int pickedId = rgbToId(r, g, b);
        if (pickedId == 0) return nullptr;

        return findObjectById(scene->getRootNode(), pickedId);
    }

    void kRenderer::setRenderMode(kRenderMode mode)
    {
        renderMode = mode;
    }

    kRenderMode kRenderer::getRenderMode()
    {
        return renderMode;
    }

    // ---------------------------------------------------------------------------
    // Debug / render-mode visualization
    // ---------------------------------------------------------------------------

    // Shared vertex shader for all debug modes.
    // Outputs: vTexCoord, vNormal (world-space), vFragPos.
    static const char *kDebugVS = R"(
#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 2) in vec2 vertexTexCoord;
layout(location = 3) in vec3 vertexNormal;
layout(location = 6) in ivec4 boneIDs;
layout(location = 7) in vec4  weights;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

const int MAX_BONES         = 128;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 vTexCoord;
out vec3 vNormal;

void main()
{
    vec4  pos = vec4(vertexPosition, 1.0);
    vec3  n   = vertexNormal;
    float tw  = 0.0;

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        int  id = boneIDs[i];
        float w = weights[i];
        if (id < 0 || w <= 0.0) continue;
        if (id >= MAX_BONES) { pos = vec4(vertexPosition, 1.0); n = vertexNormal; break; }
        pos += finalBonesMatrices[id] * vec4(vertexPosition, 1.0) * w;
        n   += mat3(transpose(inverse(finalBonesMatrices[id]))) * vertexNormal * w;
        tw  += w;
    }
    if (tw == 0.0) { pos = vec4(vertexPosition, 1.0); n = vertexNormal; }

    vTexCoord = vertexTexCoord;
    vNormal   = normalize(mat3(transpose(inverse(modelMatrix))) * n);
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * pos;
}
)";

    // Albedo mode — sample first texture or fall back to diffuse color.
    static const char *kDebugAlbedoFS = R"(
#version 330 core
in vec2 vTexCoord;
out vec4 fragColor;

uniform sampler2D debugTex;
uniform bool      hasDebugTex;
uniform vec3      diffuseColor;

void main()
{
    if (hasDebugTex)
        fragColor = texture(debugTex, vTexCoord);
    else
        fragColor = vec4(diffuseColor, 1.0);
}
)";

    // Normals mode — world-space normal as RGB.
    static const char *kDebugNormalsFS = R"(
#version 330 core
in vec3 vNormal;
out vec4 fragColor;

void main()
{
    fragColor = vec4(vNormal * 0.5 + 0.5, 1.0);
}
)";

    // Wireframe mode — flat light-grey fill (used with GL_LINE polygon mode).
    static const char *kDebugWireFS = R"(
#version 330 core
out vec4 fragColor;
void main() { fragColor = vec4(0.85, 0.85, 0.85, 1.0); }
)";

    // Depth mode — linearized depth as greyscale.
    static const char *kDebugDepthFS = R"(
#version 330 core
out vec4 fragColor;
uniform float near;
uniform float far;

void main()
{
    float z = gl_FragCoord.z * 2.0 - 1.0;
    float d = (2.0 * near * far) / (far + near - z * (far - near));
    float lin = clamp(d / far, 0.0, 1.0);
    fragColor = vec4(vec3(lin), 1.0);
}
)";

    void kRenderer::renderSceneGraphDebug(kWorld *world, kScene *scene, kObject *currentNode,
                                          kShader *shader, bool wireframe)
    {
        if (!currentNode || !currentNode->getActive()) return;
        currentNode->calculateModelMatrix();

        if (currentNode->getType() == kNodeType::NODE_TYPE_MESH)
        {
            kMesh *mesh = static_cast<kMesh *>(currentNode);
            if (mesh->getLoaded())
            {
                shader->use();
                shader->setValue("modelMatrix",      mesh->getModelMatrixWorld());
                shader->setValue("viewMatrix",       world->getMainCamera()->getViewMatrix());
                shader->setValue("projectionMatrix", world->getMainCamera()->getProjectionMatrix());

                std::vector<kMat4> bones(128, kMat4(1.0f));
                if (mesh->getSkinned() && mesh->getAnimator())
                    bones = mesh->getAnimator()->getFinalBoneMatrices();
                shader->setValue("finalBonesMatrices", bones);

                // Bind first texture as albedo hint (used by albedo mode).
                bool hasTex = false;
                if (mesh->getMaterial() && !mesh->getMaterial()->getTextures().empty())
                {
                    kTexture *tex = mesh->getMaterial()->getTexture(0);
                    if (tex && tex->getType() == kTextureType::TEX_TYPE_2D)
                    {
                        driver->bindTexture2D(0, tex->getTextureID());
                        shader->setValue("debugTex",    0);
                        shader->setValue("hasDebugTex", true);
                        hasTex = true;
                    }
                }
                if (!hasTex)
                {
                    shader->setValue("hasDebugTex", false);
                    kVec3 diff = mesh->getMaterial()
                                    ? mesh->getMaterial()->getDiffuseColor()
                                    : kVec3(0.7f, 0.7f, 0.7f);
                    shader->setValue("diffuseColor", diff);
                }

                driver->setBlend(false);
                driver->setCullFace(false);

                if (wireframe)
                {
                    glEnable(GL_POLYGON_OFFSET_LINE);
                    glPolygonOffset(-1.0f, -1.0f);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }

                mesh->draw();

                if (wireframe)
                {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    glDisable(GL_POLYGON_OFFSET_LINE);
                }

                if (hasTex)
                    driver->unbindTexture2D(0);

                shader->unuse();
            }
        }

        for (size_t i = 0; i < currentNode->getChildren().size(); ++i)
            if (currentNode->getChildren().at(i))
                renderSceneGraphDebug(world, scene, currentNode->getChildren().at(i), shader, wireframe);
    }

    // ---------------------------------------------------------------------------
    // Outline rendering (stencil-based, two-pass)
    // ---------------------------------------------------------------------------

    // Inline GLSL — vertex shader shared by both passes.
    // outlineThickness == 0  →  stencil mark pass (no expansion)
    // outlineThickness  > 0  →  outline shell pass (screen-space normal expansion)
    static const char *kOutlineVS = R"(
#version 330 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 3) in vec3 vertexNormal;
layout(location = 6) in ivec4 boneIDs;
layout(location = 7) in vec4  weights;

uniform mat4  modelMatrix;
uniform mat4  viewMatrix;
uniform mat4  projectionMatrix;
uniform float outlineThickness;

const int MAX_BONES          = 128;
const int MAX_BONE_INFLUENCE  = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

void main()
{
    vec4  totalPosition = vec4(vertexPosition, 1.0);
    vec3  totalNormal   = vertexNormal;
    float totalWeight   = 0.0;

    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        int   id = boneIDs[i];
        float w  = weights[i];
        if (id < 0 || w <= 0.0) continue;
        if (id >= MAX_BONES)
        {
            totalPosition = vec4(vertexPosition, 1.0);
            totalNormal   = vertexNormal;
            break;
        }
        totalPosition += finalBonesMatrices[id] * vec4(vertexPosition, 1.0) * w;
        totalNormal   += mat3(transpose(inverse(finalBonesMatrices[id]))) * vertexNormal * w;
        totalWeight   += w;
    }
    if (totalWeight == 0.0)
    {
        totalPosition = vec4(vertexPosition, 1.0);
        totalNormal   = vertexNormal;
    }

    vec4 clipPos = projectionMatrix * viewMatrix * modelMatrix * totalPosition;

    if (outlineThickness > 0.0)
    {
        // Screen-space normal expansion: shift by the projected normal direction
        // so the outline has uniform pixel thickness regardless of distance.
        vec3 worldNormal = normalize(mat3(modelMatrix) * normalize(totalNormal));
        vec4 clipNorm    = projectionMatrix * viewMatrix * vec4(worldNormal, 0.0);
        float len        = length(clipNorm.xy);
        vec2  dir        = (len > 1e-4) ? (clipNorm.xy / len) : vec2(0.0, 1.0);
        clipPos.xy      += dir * outlineThickness;
    }

    gl_Position = clipPos;
}
)";

    static const char *kOutlineFS = R"(
#version 330 core
out vec4 fragColor;
uniform vec4 outlineColor;
void main() { fragColor = outlineColor; }
)";

    static kObject *findNodeByUuid(kObject *node, const kString &uuid)
    {
        if (!node) return nullptr;
        if (node->getUuid() == uuid) return node;
        for (kObject *child : node->getChildren())
        {
            kObject *found = findNodeByUuid(child, uuid);
            if (found) return found;
        }
        return nullptr;
    }

    void kRenderer::renderOutline(kWorld *world, kScene *scene,
                                   const std::vector<kString> &selectedUuids,
                                   kVec4 color, float thickness)
    {
        if (!enableScreenBuffer || selectedUuids.empty() || !world || !scene) return;
        if (!world->getMainCamera()) return;

        // Lazy-compile outline shader
        if (!outlineShader)
        {
            kShader *newShader = new kShader();
            newShader->compileShaderProgram(kOutlineVS, kOutlineFS);
            outlineShader = newShader;
        }

        // Gather valid selected meshes
        std::vector<kMesh *> meshes;
        for (const auto &uuid : selectedUuids)
        {
            kObject *obj = findNodeByUuid(scene->getRootNode(), uuid);
            if (!obj || !obj->getActive()) continue;
            if (obj->getType() != kNodeType::NODE_TYPE_MESH) continue;
            kMesh *mesh = static_cast<kMesh *>(obj);
            if (mesh->getLoaded()) meshes.push_back(mesh);
        }
        if (meshes.empty()) return;

        // Bind the MSAA FBO (scene has already been rendered into it)
        driver->bindFramebuffer(fboMsaa);
        driver->setViewport(0, 0, fboWidth, fboHeight);

        // Clear only the stencil channel (preserve existing color and depth)
        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);

        outlineShader->use();
        outlineShader->setValue("viewMatrix",       world->getMainCamera()->getViewMatrix());
        outlineShader->setValue("projectionMatrix", world->getMainCamera()->getProjectionMatrix());
        outlineShader->setValue("outlineColor",     color);

        // ------------------------------------------------------------------
        // Pass 1 — stamp stencil=1 where selected mesh pixels land
        // ------------------------------------------------------------------
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(0xFF);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDepthMask(GL_FALSE);
        glEnable(GL_DEPTH_TEST);
        outlineShader->setValue("outlineThickness", 0.0f);

        for (kMesh *mesh : meshes)
        {
            mesh->calculateModelMatrix();
            outlineShader->setValue("modelMatrix", mesh->getModelMatrixWorld());
            std::vector<kMat4> bones(128, kMat4(1.0f));
            if (mesh->getSkinned() && mesh->getAnimator())
                bones = mesh->getAnimator()->getFinalBoneMatrices();
            outlineShader->setValue("finalBonesMatrices", bones);
            mesh->draw();
        }

        // ------------------------------------------------------------------
        // Pass 2 — draw outline ring only where stencil is NOT 1 (the border)
        // ------------------------------------------------------------------
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);          // X-ray: always visible
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
        outlineShader->setValue("outlineThickness", thickness);

        for (kMesh *mesh : meshes)
        {
            outlineShader->setValue("modelMatrix", mesh->getModelMatrixWorld());
            std::vector<kMat4> bones(128, kMat4(1.0f));
            if (mesh->getSkinned() && mesh->getAnimator())
                bones = mesh->getAnimator()->getFinalBoneMatrices();
            outlineShader->setValue("finalBonesMatrices", bones);
            mesh->draw();
        }

        // ------------------------------------------------------------------
        // Restore GL state
        // ------------------------------------------------------------------
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilMask(0xFF);
        glClear(GL_STENCIL_BUFFER_BIT);

        outlineShader->unuse();

        // Blit updated MSAA buffer to the resolve FBO (display texture)
        driver->bindReadFramebuffer(fboMsaa);
        driver->bindDrawFramebuffer(fbo);
        driver->blitFramebufferColor(0, 0, fboWidth, fboHeight, 0, 0, fboWidth, fboHeight);
        driver->unbindFramebuffer();
    }
}
