#include "krenderer.h"

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
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

            // OpenGL - WIP wrap OGL code into its own class
            // Don't do it here? Incase not using SDL
            if (window != nullptr)
            {
                // SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
                // SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

                // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);   // Enable multisample buffer
                // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);   // Number of samples (e.g., 4x MSAA)

                openglContext = SDL_GL_CreateContext(window->getSdlWindow());
                if (!openglContext)
                {
                    std::cout << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
                    // Optional: fallback to no multisampling
                }

                glewExperimental = GL_TRUE;
                GLenum status = glewInit();
                if (status != GLEW_OK)
                {
                    std::cout << "GLEW Error: " << glewGetErrorString(status) << std::endl;
                    return false;
                }
				
				fboWidth = window->getWindowWidth();
				fboHeight = window->getWindowHeight();
            }

            std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
            std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

            int major, minor;
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
            SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
            std::cout << "Requested OpenGL Version: " << major << "." << minor << std::endl;

            glEnable(GL_FRAMEBUFFER_SRGB);
            glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);

            // Must call this in the beginning to make it render
            glBindVertexArray(0);
        }

        return true;
    }

    void kRenderer::destroy()
    {
    }

    void kRenderer::setEngineInfo(const std::string name, uint32_t version)
    {
        engineName = name;
        engineVersion = version;
    }

    kWindow *kRenderer::getWindow()
    {
        return appWindow;
    }
	
	void kRenderer::clear()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (enableScreenBuffer)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fboMsaa);
        }
		
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    void kRenderer::render(kScene *scene, int x, int y, int width, int height, float deltaTime, bool autoClearSwapWindow)
    {
        if (frameId > 999999999999)
            frameId = 0;
        else
            frameId++;

        // std::cout << "render: " << deltaTime << std::endl;

        // Render shadow scene
        if (shadowShader != nullptr)
        {
            if (scene->getLights().size() > 0)
            {
                for (size_t i = 0; i < scene->getLights().size(); ++i)
                {
                    kLight *currentLight = scene->getLights().at(i);

                    if (currentLight->getLightType() == kLightType::LIGHT_TYPE_SUN)
                    {
                        // Render to shadow FBO
                        glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
                        glEnable(GL_DEPTH_TEST);
                        glViewport(0, 0, shadowWidth, shadowHeight);
                        glClear(GL_DEPTH_BUFFER_BIT);

                        shadowShader->use();

                        // Configure shader and matrices
                        glm::vec3 lightPos = currentLight->getPosition();
                        glm::vec3 lightDir = glm::normalize(currentLight->getDirection());

                        float near_plane = 1.0f, far_plane = 20.0f;
                        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
                        glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDir, glm::vec3(0.0f, 1.0f, 0.0f));

                        // Later send this to meshes for shadow calculation
                        lightSpaceMatrix = lightProjection * lightView;

                        renderSceneGraphShadow(scene, scene->getRootNode(), lightSpaceMatrix, lightView, lightProjection, false, deltaTime);

                        shadowShader->unuse();
                    }
                }
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (enableScreenBuffer)
        {
            // Render to MSAA FBO
            resizeFbo(width, height);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fboMsaa);
        }

        glEnable(GL_DEPTH_TEST);
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Ask for nicest perspective correction

		if (autoClearSwapWindow)
		{
			glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		}

        if (scene->getMainCamera() != nullptr)
        {
            // Use viewport instead of window?
            // Can set which camera to use for render?
            glViewport(x, y, width, height);
            scene->getMainCamera()->setAspectRatio((float)width / (float)height);

            // Render skybox if available
            kMaterial *skyboxMaterial = scene->getSkyboxMaterial();
            kMesh *skyboxMesh = scene->getSkyboxMesh();
            if (skyboxMaterial != nullptr && skyboxMesh != nullptr)
            {
                if (skyboxMesh->getLoaded())
                {
                    if (skyboxMaterial->getShader() != nullptr)
                    {
                        kShader *skyboxShader = skyboxMaterial->getShader();
                        skyboxShader->use();

                        glDepthMask(GL_FALSE);
                        glDisable(GL_CULL_FACE);

                        // Set VP matrices
                        skyboxShader->setValue("viewMatrix", mat4(mat3(scene->getMainCamera()->getViewMatrix())));
                        skyboxShader->setValue("projectionMatrix", scene->getMainCamera()->getProjectionMatrix());

                        // Set material attributes
                        // skyboxShader->setValue("material.ambient", skyboxMaterial->getAmbientColor());

                        if (skyboxMaterial->getTextures().size() > 0)
                        {
                            if (skyboxMaterial->getTexture(0)->getType() == kTextureType::TEX_TYPE_CUBE)
                            {
                                GLuint tex = skyboxMaterial->getTexture(0)->getTextureID();
                                // std::cout << tex << std::endl;
                                glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
                                glUniform1i(glGetUniformLocation(skyboxShader->getShaderProgram(), skyboxMaterial->getTexture(0)->getTextureName().c_str()), 0);
                            }
                        }

                        skyboxMesh->calculateModelMatrix();
                        skyboxMesh->draw();

                        // std::cout << skyboxMesh->getChildren().size() << std::endl;

                        if (skyboxMesh->getChildren().size() > 0)
                        {
                            for (size_t i = 0; i < skyboxMesh->getChildren().size(); ++i)
                            {
                                kMesh *childMesh = (kMesh *)skyboxMesh->getChildren().at(i);

                                if (childMesh != nullptr)
                                {
                                    childMesh->calculateModelMatrix();
                                    childMesh->draw();
                                }
                            }
                        }

                        skyboxShader->unuse();
                        // glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                    }
                }
            }

            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glEnable(GL_CULL_FACE);

            // Starting from root node, render everything recursively
            // Render non-transparent objects first
            renderSceneGraph(scene, scene->getRootNode(), false, deltaTime);
            // Sort transparent objects
            // Render transparent objects
            // renderSceneGraph(scene, scene->getRootNode(), true, deltaTime);
        }
        else
        {
            std::cout << "No main camera found" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (enableScreenBuffer)
        {
            // Blit MSAA FBO to regular FBO
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fboMsaa);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
            glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

            // Render FBO texture to screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDisable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            getScreenShader()->use();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fboTexColor);
            // glBindTexture(GL_TEXTURE_2D, shadowFboTex); // Debug shadow depth map

            // Calculate luminance for auto exposure
            if (enableAutoExposure)
            {
                glGenerateMipmap(GL_TEXTURE_2D); // auto-downsamples the texture
                int mipLevel = (int)std::floor(std::log2(std::max(width, height)));
                glGetTexImage(GL_TEXTURE_2D, mipLevel, GL_RGBA, GL_FLOAT, &averageLuminanceColor);
                averageLuminance = 0.2126f * averageLuminanceColor[0] + 0.7152f * averageLuminanceColor[1] + 0.0722f * averageLuminanceColor[2];
                float targetExposure = exposureKey / (averageLuminance + 0.001);
                exposure = glm::mix(exposure, targetExposure, deltaTime * exposureAdaptationRate);
                // std::cout << averageLuminance << std::endl;
                if (screenShader != nullptr)
                {
                    screenShader->setValue("enable_autoExposure", enableAutoExposure);
                    screenShader->setValue("exposure", exposure * 3.0f);
                    screenShader->setValue("contrast", 1.01f);
                    screenShader->setValue("gamma", 2.2f);
                }
            }

            glBindVertexArray(quadVao);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            getScreenShader()->unuse();
        }

        // Must call this in the end to make it render
        glBindVertexArray(0);

        if (autoClearSwapWindow && appWindow != nullptr)
        {
            appWindow->swap();
        }
    }

    void kRenderer::renderSceneGraph(kScene *scene, kObject *currentNode, bool transparent, float deltaTime)
    {
        // Ignore if the object is not active
        if (currentNode != nullptr)
        {
            if (currentNode->getActive())
            {
                // Calculate model matrix
                kObject *childNode = currentNode;
                childNode->calculateModelMatrix();

                if (currentNode->getType() == kNodeType::NODE_TYPE_MESH)
                {
                    // Mesh
                    kMesh *currentMesh = (kMesh *)currentNode;

                    // Make sure mesh is fully loaded before render
                    // This will prevent crash
                    if (currentMesh->getLoaded())
                    {
                        if (currentMesh->getMaterial() != nullptr)
                        {
                            // Blend mode
                            if (currentMesh->getMaterial()->getTransparent() == kTransparentType::TRANSP_TYPE_BLEND)
                            {
                                glEnable(GL_BLEND);
                                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                            }
                            else
                            {
                                glDisable(GL_BLEND);
                            }

                            // Face culling
                            if (currentMesh->getMaterial()->getSingleSided())
                            {
                                // std::cout << "yes" << std::endl;
                                glEnable(GL_CULL_FACE);
                                glFrontFace(GL_CCW);

                                if (currentMesh->getMaterial()->getCullBack())
                                {
                                    // std::cout << "back" << std::endl;
                                    glCullFace(GL_BACK);
                                }
                                else
                                {
                                    // std::cout << "front" << std::endl;
                                    glCullFace(GL_FRONT);
                                }
                            }
                            else
                            {
                                // std::cout << "no" << std::endl;
                                glDisable(GL_CULL_FACE);
                            }

                            if (currentMesh->getMaterial()->getShader() != nullptr)
                            {
                                kShader *shader = currentMesh->getMaterial()->getShader();
                                shader->use();

                                // Set MVP matrices
                                shader->setValue("normalMatrix", currentMesh->getNormalMatrix());
                                shader->setValue("modelMatrix", currentMesh->getModelMatrixWorld());
                                shader->setValue("viewMatrix", scene->getMainCamera()->getViewMatrix());
                                shader->setValue("projectionMatrix", scene->getMainCamera()->getProjectionMatrix());

                                // Set camera position
                                if (scene->getMainCamera() != nullptr)
                                {
                                    shader->setValue("viewPos", scene->getMainCamera()->getPosition());
                                }

                                // Set material attributes
                                shader->setValue("material.tiling", currentMesh->getMaterial()->getUvTiling());

                                shader->setValue("material.ambient", currentMesh->getMaterial()->getAmbientColor());
                                shader->setValue("material.diffuse", currentMesh->getMaterial()->getDiffuseColor());
                                shader->setValue("material.specular", currentMesh->getMaterial()->getSpecularColor());
                                shader->setValue("material.shininess", currentMesh->getMaterial()->getShininess());
                                shader->setValue("material.metallic", currentMesh->getMaterial()->getMetallic());
                                shader->setValue("material.roughness", currentMesh->getMaterial()->getRoughness());

                                std::vector<mat4> boneTransforms(128, mat4(1.0f));

                                // Set animator
                                if (currentMesh->getSkinned())
                                {
                                    if (currentMesh->getAnimator() != nullptr)
                                    {
                                        // Update animation
                                        currentMesh->getAnimator()->updateAnimation(deltaTime * currentMesh->getAnimator()->getCurrentAnimation()->getSpeed(), frameId);

                                        boneTransforms = currentMesh->getAnimator()->getFinalBoneMatrices();
                                        shader->setValue("finalBonesMatrices", boneTransforms);
                                    }
                                }
                                else
                                {
                                    shader->setValue("finalBonesMatrices", boneTransforms);
                                }

                                // Set lights
                                int countSunLight = 0;
                                int countPointLight = 0;
                                int countSpotLight = 0;

                                if (scene->getLights().size() > 0)
                                {
                                    for (size_t j = 0; j < scene->getLights().size(); ++j)
                                    {
                                        if (scene->getLights().at(j) != nullptr)
                                        {
                                            if (scene->getLights().at(j)->getActive())
                                            {
                                                // Sun light
                                                if (scene->getLights().at(j)->getLightType() == LIGHT_TYPE_SUN)
                                                {
                                                    shader->setValue("sunLights[" + std::to_string(countSunLight) + "].power", scene->getLights().at(j)->getPower());
                                                    shader->setValue("sunLights[" + std::to_string(countSunLight) + "].direction", scene->getLights().at(j)->getDirection());

                                                    shader->setValue("sunLights[" + std::to_string(countSunLight) + "].ambient", scene->getLights().at(j)->getAmbientColor());
                                                    shader->setValue("sunLights[" + std::to_string(countSunLight) + "].diffuse", scene->getLights().at(j)->getDiffuseColor());
                                                    shader->setValue("sunLights[" + std::to_string(countSunLight) + "].specular", scene->getLights().at(j)->getSpecularColor());

                                                    countSunLight++;
                                                }
                                                // Point light
                                                else if (scene->getLights().at(j)->getLightType() == LIGHT_TYPE_POINT)
                                                {
                                                    shader->setValue("pointLights[" + std::to_string(countPointLight) + "].power", scene->getLights().at(j)->getPower());
                                                    shader->setValue("pointLights[" + std::to_string(countPointLight) + "].position", scene->getLights().at(j)->getPosition());

                                                    shader->setValue("pointLights[" + std::to_string(countPointLight) + "].constant", scene->getLights().at(j)->getConstant());
                                                    shader->setValue("pointLights[" + std::to_string(countPointLight) + "].linear", scene->getLights().at(j)->getLinear());
                                                    shader->setValue("pointLights[" + std::to_string(countPointLight) + "].quadratic", scene->getLights().at(j)->getQuadratic());

                                                    shader->setValue("pointLights[" + std::to_string(countPointLight) + "].ambient", scene->getLights().at(j)->getAmbientColor());
                                                    shader->setValue("pointLights[" + std::to_string(countPointLight) + "].diffuse", scene->getLights().at(j)->getDiffuseColor());
                                                    shader->setValue("pointLights[" + std::to_string(countPointLight) + "].specular", scene->getLights().at(j)->getSpecularColor());

                                                    countPointLight++;
                                                }
                                                // Spot light
                                                else if (scene->getLights().at(j)->getLightType() == LIGHT_TYPE_SPOT)
                                                {
                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].power", scene->getLights().at(j)->getPower());
                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].position", scene->getLights().at(j)->getPosition());
                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].direction", scene->getLights().at(j)->getDirection());

                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].cutOff", scene->getLights().at(j)->getCutOff());
                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].outerCutOff", scene->getLights().at(j)->getOuterCutOff());

                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].constant", scene->getLights().at(j)->getConstant());
                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].linear", scene->getLights().at(j)->getLinear());
                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].quadratic", scene->getLights().at(j)->getQuadratic());

                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].ambient", scene->getLights().at(j)->getAmbientColor());
                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].diffuse", scene->getLights().at(j)->getDiffuseColor());
                                                    shader->setValue("spotLights[" + std::to_string(countSpotLight) + "].specular", scene->getLights().at(j)->getSpecularColor());

                                                    countSpotLight++;
                                                }
                                            }
                                        }
                                    }

                                    // Set light size
                                    shader->setValue("sunLightNum", countSunLight);
                                    shader->setValue("pointLightNum", countPointLight);
                                    shader->setValue("spotLightNum", countSpotLight);

                                    // Send the shadow info
                                    shader->setValue("lightSpaceMatrix", lightSpaceMatrix);
                                    glActiveTexture(GL_TEXTURE0 + (unsigned int)currentMesh->getMaterial()->getTextures().size());
                                    glBindTexture(GL_TEXTURE_2D, shadowFboTex);
                                    shader->setValue("shadowMap", (unsigned int)currentMesh->getMaterial()->getTextures().size());

                                    // std::cout << countSunLight << countPointLight << countSpotLight << std::endl;
                                }

                                if (currentMesh->getMaterial()->getTextures().size() > 0)
                                {
                                    for (size_t k = 0; k < currentMesh->getMaterial()->getTextures().size(); k++)
                                    {
                                        if (currentMesh->getMaterial()->getTexture(k) != nullptr)
                                        {
                                            GLuint tex = currentMesh->getMaterial()->getTexture(k)->getTextureID();
                                            glActiveTexture(GL_TEXTURE0 + k);

                                            if (currentMesh->getMaterial()->getTexture(k)->getType() == kTextureType::TEX_TYPE_2D)
                                                glBindTexture(GL_TEXTURE_2D, tex);
                                            else if (currentMesh->getMaterial()->getTexture(k)->getType() == kTextureType::TEX_TYPE_CUBE)
                                                glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

                                            currentMesh->getMaterial()->getShader()->setValue(currentMesh->getMaterial()->getTexture(k)->getTextureName().c_str(), (unsigned int)k);
                                            currentMesh->getMaterial()->getShader()->setValue("has_" + currentMesh->getMaterial()->getTexture(k)->getTextureName(), true);

                                            // std::cout << k << currentMesh->getMaterial()->getTexture2D(k)->getTextureName().c_str() << std::endl;
                                        }
                                    }
                                }

                                currentMesh->draw();

                                shader->unuse();
                                glActiveTexture(GL_TEXTURE0);
                                glBindTexture(GL_TEXTURE_2D, 0);
                            }
                        }
                        else
                        {
                            // std::cout << "No material found" << std::endl;
                        }
                    }
                }
                else if (currentNode->getType() == kNodeType::NODE_TYPE_LIGHT)
                {
                    // Light
                    kLight *currentLight = (kLight *)currentNode;

                    // Render icon
                    if (scene->getMainCamera() != nullptr)
                    {
                        mat4 view = lookAt(scene->getMainCamera()->getPosition(), scene->getMainCamera()->getLookAt(), scene->getMainCamera()->calculateUp());
                        mat4 projection = glm::perspective(glm::radians(scene->getMainCamera()->getFOV()), scene->getMainCamera()->getAspectRatio(), scene->getMainCamera()->getNearClip(), scene->getMainCamera()->getFarClip());

                        if (currentLight->getMaterial() != nullptr)
                        {
                            if (currentLight->getMaterial()->getTransparent() == kTransparentType::TRANSP_TYPE_BLEND)
                            {
                                glEnable(GL_BLEND);
                                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                            }
                            else
                            {
                                glDisable(GL_BLEND);
                            }

                            if (currentLight->getMaterial()->getShader() != nullptr)
                            {
                                kShader *shader = currentLight->getMaterial()->getShader();
                                shader->use();

                                shader->setValue("viewProjection", projection * view);

                                shader->setValue("cameraRightWorldSpace", vec3(view[0][0], view[1][0], view[2][0]));
                                shader->setValue("cameraUpWorldSpace", vec3(view[0][1], view[1][1], view[2][1]));

                                shader->setValue("billboardPosition", currentLight->getPosition());
                                shader->setValue("billboardSize", vec2(0.8f, 0.8f));

                                shader->setValue("color", currentLight->getDiffuseColor());

                                if (currentLight->getMaterial()->getTextures().size() > 0)
                                {
                                    for (size_t l = 0; l < currentLight->getMaterial()->getTextures().size(); l++)
                                    {
                                        if (currentLight->getMaterial()->getTexture(l) != nullptr)
                                        {
                                            if (currentLight->getMaterial()->getTexture(l)->getType() == kTextureType::TEX_TYPE_2D)
                                            {
                                                GLuint tex = currentLight->getMaterial()->getTexture(l)->getTextureID();
                                                glActiveTexture(GL_TEXTURE0 + l);
                                                glBindTexture(GL_TEXTURE_2D, tex);
                                                glUniform1i(glGetUniformLocation(currentLight->getMaterial()->getShader()->getShaderProgram(), "albedoMap"), l);
                                            }
                                        }
                                    }
                                }

                                currentLight->draw();

                                shader->unuse();
                                glBindTexture(GL_TEXTURE_2D, 0);
                            }
                        }
                        else
                        {
                            // std::cout << "No material found" << std::endl;
                        }
                    }
                }
                else if (currentNode->getType() == kNodeType::NODE_TYPE_OBJECT)
                {
                    // Empty object
                    kObject *currentObject = currentNode;

                    // Set MVP if needed
                    if (currentObject->getMaterial() != nullptr)
                    {
                        if (currentObject->getMaterial()->getTransparent() == kTransparentType::TRANSP_TYPE_BLEND)
                        {
                            glEnable(GL_BLEND);
                            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                        }
                        else
                        {
                            glDisable(GL_BLEND);
                        }

                        if (currentObject->getMaterial()->getShader() != nullptr)
                        {
                            kShader *shader = currentObject->getMaterial()->getShader();

                            shader->use();

                            // Set MVP matrices
                            shader->setValue("modelMatrix", currentObject->getModelMatrixWorld());
                            shader->setValue("viewMatrix", scene->getMainCamera()->getViewMatrix());
                            shader->setValue("projectionMatrix", scene->getMainCamera()->getProjectionMatrix());

                            currentObject->draw();

                            shader->unuse();
                            glBindTexture(GL_TEXTURE_2D, 0);
                        }
                    }
                }

                // Continue with the next child
                if (currentNode->getChildren().size() > 0)
                {
                    for (size_t i = 0; i < currentNode->getChildren().size(); ++i)
                    {
                        if (currentNode->getChildren().at(i) != nullptr)
                        {
                            renderSceneGraph(scene, currentNode->getChildren().at(i), transparent, deltaTime);
                        }
                    }
                }
            }
        }
    }

    void kRenderer::renderSceneGraphShadow(kScene *scene, kObject *currentNode, mat4 lightSpaceMatrix, mat4 lightView, mat4 lightProjection, bool transparent, float deltaTime)
    {
        // Ignore if the object is not active
        if (currentNode != nullptr)
        {
            if (currentNode->getActive())
            {
                // Calculate model matrix
                kObject *childNode = currentNode;
                childNode->calculateModelMatrix();

                if (currentNode->getType() == kNodeType::NODE_TYPE_MESH)
                {
                    // Mesh
                    kMesh *currentMesh = (kMesh *)currentNode;

                    if (currentMesh->getCastShadow())
                    {
                        shadowShader->setValue("lightSpaceMatrix", lightSpaceMatrix);

                        // Set MVP matrices
                        shadowShader->setValue("modelMatrix", currentMesh->getModelMatrixWorld());
                        shadowShader->setValue("viewMatrix", lightView);
                        shadowShader->setValue("projectionMatrix", lightProjection);

                        std::vector<mat4> boneTransforms(128, mat4(1.0f));

                        // Set animator
                        if (currentMesh->getSkinned())
                        {
                            if (currentMesh->getAnimator() != nullptr)
                            {
                                // Update animation
                                currentMesh->getAnimator()->updateAnimation(deltaTime * currentMesh->getAnimator()->getCurrentAnimation()->getSpeed(), frameId);

                                boneTransforms = currentMesh->getAnimator()->getFinalBoneMatrices();
                                shadowShader->setValue("finalBonesMatrices", boneTransforms);
                            }
                        }
                        else
                        {
                            shadowShader->setValue("finalBonesMatrices", boneTransforms);
                        }

                        currentMesh->draw();
                    }
                }

                // Continue with the next child
                if (currentNode->getChildren().size() > 0)
                {
                    for (size_t i = 0; i < currentNode->getChildren().size(); ++i)
                    {
                        if (currentNode->getChildren().at(i) != nullptr)
                        {
                            renderSceneGraphShadow(scene, currentNode->getChildren().at(i), lightSpaceMatrix, lightView, lightProjection, transparent, deltaTime);
                        }
                    }
                }
            }
        }
    }

    vec4 kRenderer::getClearColor()
    {
        return clearColor;
    }

    void kRenderer::setClearColor(vec4 newColor)
    {
        // Convert to sRGB
        newColor.r = srgbToLinear(newColor.r);
        newColor.g = srgbToLinear(newColor.g);
        newColor.b = srgbToLinear(newColor.b);

        clearColor = newColor;
        glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
    }

    void kRenderer::setEnableScreenBuffer(bool newEnable, bool useDefaultShader)
    {
        enableScreenBuffer = newEnable;

        if (newEnable)
        {
            glDisable(GL_FRAMEBUFFER_SRGB);

            // Screen quad
            float quadVerts[] = {
                -1,
                -1,
                0,
                0,
                1,
                -1,
                1,
                0,
                1,
                1,
                1,
                1,
                -1,
                1,
                0,
                1,
            };
            GLuint quadIndices[] = {0, 1, 2, 2, 3, 0};

            glGenVertexArrays(1, &quadVao);
            glGenBuffers(1, &quadVbo);
            glGenBuffers(1, &quadEbo);
            glBindVertexArray(quadVao);
            glBindBuffer(GL_ARRAY_BUFFER, quadVbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEbo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // Create MSAA FBO
            glGenFramebuffers(1, &fboMsaa);
            glBindFramebuffer(GL_FRAMEBUFFER, fboMsaa);
            glGenTextures(1, &fboTexColorMsaa);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fboTexColorMsaa);
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGB, fboWidth, fboHeight, GL_TRUE);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fboTexColorMsaa, 0);

            glGenRenderbuffers(1, &rboMsaa);
            glBindRenderbuffer(GL_RENDERBUFFER, rboMsaa);
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboMsaa);

			glGenRenderbuffers(1, &rboDepthMsaa);
			glBindRenderbuffer(GL_RENDERBUFFER, rboDepthMsaa);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, GL_DEPTH24_STENCIL8, fboWidth, fboHeight);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthMsaa);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cerr << "FBO not complete!" << std::endl;
                return;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            // Create FBO
            glGenFramebuffers(1, &fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, fbo);

            glGenTextures(1, &fboTexColor);
            glBindTexture(GL_TEXTURE_2D, fboTexColor);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fboWidth, fboHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexColor, 0);

            glGenRenderbuffers(1, &rboDepth);
            glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fboWidth, fboHeight); // depth + stencil
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cerr << "FBO not complete!" << std::endl;
                return;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            if (useDefaultShader)
            {
                string vertexShader = "#version 330 core \
				layout(location = 0) in vec2 aPos; \
				layout(location = 1) in vec2 aTexCoord; \
				out vec2 TexCoord; \
				void main() \
				{ \
					TexCoord = aTexCoord; \
					gl_Position = vec4(aPos, 0.0, 1.0); \
				}";

                string fragmentShader = "#version 330 core \
				in vec2 TexCoord; \
				out vec4 FragColor; \
				uniform sampler2D screenTexture; \
				uniform int enable_autoExposure; \
				uniform float exposure; \
				uniform float contrast; \
				uniform float gamma; \
				void main() \
				{ \
					vec3 color = texture(screenTexture, TexCoord).rgb; \
					vec3 mapped = color * exposure; \
					mapped = (mapped - 0.5) * contrast + 0.5; \
					mapped = pow(mapped, vec3(1.0 / gamma)); \
					vec4 result = vec4(mapped, 1.0); \
					FragColor = result; \
				}";

                kShader *screenShader = new kShader();
                screenShader->loadShadersCode(vertexShader.c_str(), fragmentShader.c_str());
                setScreenShader(screenShader);
            }
        }
        else
        {
            glEnable(GL_FRAMEBUFFER_SRGB);
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
            // Create shadow FBO
            glGenFramebuffers(1, &shadowFbo);
            glGenTextures(1, &shadowFboTex);
            glBindTexture(GL_TEXTURE_2D, shadowFboTex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = {1.0, 1.0, 1.0, 1.0};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glBindFramebuffer(GL_FRAMEBUFFER, shadowFbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowFboTex, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (status != GL_FRAMEBUFFER_COMPLETE)
            {
                std::cerr << "Shadow framebuffer is incomplete: " << status << std::endl;
            }

            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            if (useDefaultShader)
            {
                string vertexShader = "#version 330 core \
				layout (location = 0) in vec3 vertexPosition; \
				layout (location = 6) in ivec4 boneIDs;  \
				layout (location = 7) in vec4 weights; \
				uniform mat4 lightSpaceMatrix; \
				uniform mat4 modelMatrix; \
				uniform mat4 viewMatrix; \
				uniform mat4 projectionMatrix; \
				const int MAX_BONES = 128; \
				const int MAX_BONE_INFLUENCE = 4; \
				uniform mat4 finalBonesMatrices[MAX_BONES]; \
				out vec3 vertexPositionFrag; \
				void main() \
				{ \
					vec4 totalPosition = vec4(vertexPosition, 1.0f); \
					float totalWeight = 0.0; \
					for(int i = 0 ; i < MAX_BONE_INFLUENCE; i++) \
					{ \
						int boneID = boneIDs[i]; \
						float weight = weights[i]; \
						if(boneID == -1 || weight <= 0.0) \
							continue; \
						if(boneID >= MAX_BONES) \
						{ \
							totalPosition = vec4(vertexPosition, 1.0f); \
							break; \
						} \
						totalPosition += (finalBonesMatrices[boneID] * vec4(vertexPosition, 1.0f)) * weight; \
						mat3 normalMatrixBone = transpose(inverse(mat3(finalBonesMatrices[boneID]))); \
						totalWeight += weight; \
					} \
					if (totalWeight == 0.0) \
					{ \
						totalPosition = vec4(vertexPosition, 1.0f); \
					} \
					mat4 mvp = projectionMatrix * viewMatrix * modelMatrix; \
					vec4 worldPosition = modelMatrix * totalPosition; \
					vertexPositionFrag = (lightSpaceMatrix * worldPosition).xyz; \
					gl_Position = lightSpaceMatrix * worldPosition; \
				}";

                string fragmentShader = "#version 330 core \
				in vec3 vertexPositionFrag; \
				out vec4 fragColor; \
				void main() \
				{ \
				}";

                kShader *shadowShader = new kShader();
                shadowShader->loadShadersCode(vertexShader.c_str(), fragmentShader.c_str());
                setShadowShader(shadowShader);
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

    SDL_GLContext kRenderer::getOpenGlContext()
    {
        return openglContext;
    }

    void kRenderer::resizeFbo(int newWidth, int newHeight)
	{
		if (newWidth == fboWidth && newHeight == fboHeight) return;
		if (newWidth <= 0 || newHeight <= 0) return;

		// --- Resize MSAA color attachment ---
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fboTexColorMsaa);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB,
								newWidth, newHeight, GL_TRUE);

		// --- Resize MSAA depth/stencil attachment ---
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepthMsaa);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4,
										 GL_DEPTH24_STENCIL8,
										 newWidth, newHeight);

		// --- Re-attach MSAA buffers ---
		glBindFramebuffer(GL_FRAMEBUFFER, fboMsaa);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							   GL_TEXTURE_2D_MULTISAMPLE, fboTexColorMsaa, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
								  GL_RENDERBUFFER, rboDepthMsaa);
		
		glBindFramebuffer(GL_FRAMEBUFFER, fboMsaa);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fboTexColorMsaa, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthMsaa);

		// --- Resize resolve FBO (single-sample) ---
		glBindTexture(GL_TEXTURE_2D, fboTexColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newWidth, newHeight, 0,
					 GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
							  newWidth, newHeight);

		// --- Re-attach resolve FBO ---
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
							   GL_TEXTURE_2D, fboTexColor, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
								  GL_RENDERBUFFER, rboDepth);

		// Reset
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		fboWidth  = newWidth;
		fboHeight = newHeight;

		// (Optional) Debug check
		glBindFramebuffer(GL_FRAMEBUFFER, fboMsaa);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "MSAA FBO incomplete after resize!" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Resolve FBO incomplete after resize!" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

    GLuint kRenderer::getFboTexture()
    {
        return fboTexColor;
    }

    float kRenderer::srgbToLinear(float c)
    {
        return (c <= 0.04045) ? (c / 12.92) : pow((c + 0.055) / 1.055, 2.4);
    }

    vec3 kRenderer::idToRgb(unsigned int i)
    {
        int r = (i & 0x000000FF) >> 0;
        int g = (i & 0x0000FF00) >> 8;
        int b = (i & 0x00FF0000) >> 16;

        return vec3(r, g, b);
    }

    unsigned int kRenderer::rgbToId(unsigned int r, unsigned int g, unsigned int b)
    {
        unsigned int pickedId =
            r +
            g * 256 +
            b * 256 * 256;

        return pickedId;
    }
}
