#include "koffscreenrenderer.h"
#include "kopengldriver.h"
#include "kdriver.h"
#include "kmesh.h"
#include "kscene.h"
#include "kworld.h"
#include "klight.h"
#include "kmaterial.h"
#include "ktexture.h"
#include "kshader.h"
#include "kobject.h"

#include "stb_image_write.h"

#include <algorithm>
#include <cstring>
#include <functional>

namespace kemena
{
    // -----------------------------------------------------------------------
    // Built-in Lambert preview shader — used when mesh has no material
    // -----------------------------------------------------------------------

    static const char *kPreviewVS = R"(
        #version 330 core
        layout(location = 0) in vec3 aPosition;
        layout(location = 3) in vec3 aNormal;
        layout(location = 5) in vec3 aBitangent;
        uniform mat4 modelMatrix;
        uniform mat4 viewMatrix;
        uniform mat4 projectionMatrix;
        out vec3 vNormal;
        out vec3 vBitangent;
        out vec3 vFragPos;
        void main() {
            mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
            vFragPos    = vec3(modelMatrix * vec4(aPosition, 1.0));
            vNormal     = normalize(normalMatrix * aNormal);
            vBitangent  = normalize(normalMatrix * aBitangent);
            gl_Position = projectionMatrix * viewMatrix * vec4(vFragPos, 1.0);
        }
    )";

    static const char *kPreviewFS = R"(
        #version 330 core
        in vec3 vNormal;
        in vec3 vBitangent;
        in vec3 vFragPos;
        uniform vec3 viewPos;
        out vec4 fragColor;
        void main() {
            vec3  n      = normalize(vNormal);
            vec3  b      = normalize(vBitangent);
            vec3  V      = normalize(viewPos - vFragPos);
            float dN     = dot(n, V) * 0.5 + 0.5;
            float dB     = dot(b, V) * 0.5 + 0.5;
            float top    = n.y * 0.5 + 0.5;
            float shade  = dN * 0.50 + dB * 0.25 + top * 0.25;
            vec3  albedo = vec3(0.75, 0.75, 0.75);
            fragColor    = vec4(albedo * (0.10 + shade * 0.90), 1.0);
        }
    )";

    // -----------------------------------------------------------------------
    // Construction / destruction
    // -----------------------------------------------------------------------

    kOffscreenRenderer::kOffscreenRenderer(int width, int height)
        : width(width), height(height)
    {
        driver = kDriver::getCurrent();
        createFBO();
    }

    kOffscreenRenderer::~kOffscreenRenderer()
    {
        destroyFBO();
        if (builtinShader) { delete builtinShader; builtinShader = nullptr; }
    }

    void kOffscreenRenderer::ensureBuiltinShader()
    {
        if (builtinShader) return;
        builtinShader = new kShader();
        builtinShader->loadShadersCode(kPreviewVS, kPreviewFS);
    }

    // -----------------------------------------------------------------------
    // FBO lifecycle
    // -----------------------------------------------------------------------

    void kOffscreenRenderer::createFBO()
    {
        if (!driver) return;

        fbo      = driver->createFramebuffer();
        colorTex = driver->createFBOColorTexture(width, height);
        driver->attachFBOColorTexture(fbo, colorTex);

        depthRbo = driver->createRenderbuffer();
        driver->setupRenderbuffer(depthRbo, width, height);
        driver->attachRenderbufferDepthStencil(fbo, depthRbo);

        driver->bindFramebuffer(fbo);
        driver->setFramebufferDrawBuffer();
        driver->unbindFramebuffer();
    }

    void kOffscreenRenderer::destroyFBO()
    {
        if (!driver) return;
        if (colorTex) { driver->deleteFBOTexture(colorTex); colorTex = 0; }
        if (depthRbo) { driver->deleteRenderbuffer(depthRbo); depthRbo = 0; }
        if (fbo)      { driver->deleteFramebuffer(fbo);       fbo      = 0; }
    }

    void kOffscreenRenderer::resize(int newWidth, int newHeight)
    {
        width  = newWidth;
        height = newHeight;
        destroyFBO();
        createFBO();
    }

    // -----------------------------------------------------------------------
    // render — full scene
    // -----------------------------------------------------------------------

    void kOffscreenRenderer::render(kWorld *world, kScene *scene, kCamera *camera)
    {
        if (!driver || !scene || !camera || !fbo) return;

        driver->bindFramebuffer(fbo);
        driver->setViewport(0, 0, width, height);
        driver->setClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        driver->clear(true, true, false);
        driver->setDepthTest(true);
        driver->setDepthWrite(true);
        driver->setCullFace(true);
        driver->setBlend(false);

        renderNodeFull(scene->getRootNode(), scene, camera);

        driver->unbindFramebuffer();
    }

    void kOffscreenRenderer::renderNodeFull(kObject *node, kScene *scene, kCamera *camera)
    {
        if (!node || !node->getActive()) return;

        node->calculateModelMatrix();

        if (node->getType() == kNodeType::NODE_TYPE_MESH)
        {
            kMesh *mesh = static_cast<kMesh *>(node);

            if (mesh->getLoaded() && mesh->getVisible() &&
                mesh->getMaterial() != nullptr &&
                mesh->getMaterial()->getShader() != nullptr)
            {
                if (mesh->getMaterial()->getTransparent() == kTransparentType::TRANSP_TYPE_BLEND)
                {
                    driver->setBlend(true);
                    driver->setBlendFunc(kBlendFactor::SRC_ALPHA, kBlendFactor::ONE_MINUS_SRC_ALPHA);
                }
                else
                {
                    driver->setBlend(false);
                }

                if (mesh->getMaterial()->getSingleSided())
                {
                    driver->setCullFace(true);
                    driver->setFrontFace(kFrontFace::CCW);
                    driver->setCullMode(mesh->getMaterial()->getCullBack()
                                        ? kCullMode::BACK : kCullMode::FRONT);
                }
                else
                {
                    driver->setCullFace(false);
                }

                int sun = 0, point = 0, spot = 0;
                kShader *shader = mesh->getMaterial()->getShader();
                shader->use();

                setupLightsFromScene(shader, scene, sun, point, spot);
                drawMeshWithMaterial(mesh, scene, camera, sun, point, spot);

                shader->unuse();
            }
        }

        for (kObject *child : node->getChildren())
            renderNodeFull(child, scene, camera);
    }

    // -----------------------------------------------------------------------
    // renderMesh — single mesh, auto-framed
    // -----------------------------------------------------------------------

    void kOffscreenRenderer::renderMesh(kMesh *mesh, kCamera *camera)
    {
        if (!driver || !mesh || !fbo) return;

        // Auto-frame camera using the bounding box of the entire hierarchy
        kCamera autoCamera;
        kCamera *cam = camera;
        if (!cam)
        {
            // Compute combined AABB of root + all children recursively
            kAABB combined;
            std::function<void(kMesh*)> expandAABB = [&](kMesh *m) {
                m->calculateModelMatrix();
                kAABB b = m->getWorldAABB();
                if (b.isValid())
                {
                    combined.expandBy(b.min);
                    combined.expandBy(b.max);
                }
                for (kObject *child : m->getChildren())
                    if (child->getType() == kNodeType::NODE_TYPE_MESH)
                        expandAABB(static_cast<kMesh*>(child));
            };
            expandAABB(mesh);

            kVec3 center = combined.isValid() ? combined.center() : kVec3(0.0f);
            kVec3 he     = combined.isValid() ? combined.halfExtents() : kVec3(1.0f);
            float radius = glm::length(he);
            if (radius < 0.001f) radius = 1.0f;

            float fov  = 45.0f;
            float dist = (radius / glm::tan(glm::radians(fov * 0.5f))) * 1.0f;
            kVec3 dir  = glm::normalize(kVec3(0.5f, 0.5f, 1.0f));
            kVec3 eye  = center + dir * dist;

            autoCamera.setPosition(eye);
            autoCamera.setLookAt(center);
            autoCamera.setFOV(fov);
            autoCamera.setAspectRatio((float)width / (float)height);
            autoCamera.setNearClip(dist * 0.01f);
            autoCamera.setFarClip(dist * 10.0f);
            cam = &autoCamera;
        }

        driver->bindFramebuffer(fbo);
        driver->setViewport(0, 0, width, height);
        driver->setClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        driver->clear(true, true, false);
        driver->setDepthTest(true);
        driver->setDepthWrite(true);
        driver->setCullFace(false);
        driver->setBlend(false);

        drawMeshHierarchy(mesh, cam);

        driver->unbindFramebuffer();
    }

    void kOffscreenRenderer::drawMeshHierarchy(kMesh *mesh, kCamera *camera)
    {
        if (!mesh) return;
        mesh->calculateModelMatrix();

        if (mesh->getLoaded())
            drawMeshBuiltin(mesh, camera);

        for (kObject *child : mesh->getChildren())
            if (child->getType() == kNodeType::NODE_TYPE_MESH)
                drawMeshHierarchy(static_cast<kMesh*>(child), camera);
    }

    void kOffscreenRenderer::drawMeshBuiltin(kMesh *mesh, kCamera *camera)
    {
        ensureBuiltinShader();
        builtinShader->use();
        mesh->calculateModelMatrix();
        mesh->calculateNormalMatrix();
        builtinShader->setValue("modelMatrix",      mesh->getModelMatrixWorld());
        builtinShader->setValue("viewMatrix",       camera->getViewMatrix());
        builtinShader->setValue("projectionMatrix", camera->getProjectionMatrix());
        builtinShader->setValue("viewPos",          camera->getPosition());
        mesh->draw();
        builtinShader->unuse();
    }

    // -----------------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------------

    void kOffscreenRenderer::drawMeshWithMaterial(kMesh *mesh, kScene *scene,
                                                   kCamera *camera,
                                                   int /*sunCount*/, int /*pointCount*/, int /*spotCount*/)
    {
        kShader *shader = mesh->getMaterial()->getShader();

        mesh->calculateNormalMatrix();

        shader->setValue("modelMatrix",      mesh->getModelMatrixWorld());
        shader->setValue("viewMatrix",       camera->getViewMatrix());
        shader->setValue("projectionMatrix", camera->getProjectionMatrix());
        shader->setValue("normalMatrix",     mesh->getNormalMatrix());
        shader->setValue("viewPos",          camera->getPosition());

        shader->setValue("material.tiling",    mesh->getMaterial()->getUvTiling());
        shader->setValue("material.ambient",   mesh->getMaterial()->getAmbientColor());
        shader->setValue("material.diffuse",   mesh->getMaterial()->getDiffuseColor());
        shader->setValue("material.specular",  mesh->getMaterial()->getSpecularColor());
        shader->setValue("material.shininess", mesh->getMaterial()->getShininess());
        shader->setValue("material.metallic",  mesh->getMaterial()->getMetallic());
        shader->setValue("material.roughness", mesh->getMaterial()->getRoughness());

        // Identity bone transforms (no skinning for thumbnails)
        std::vector<kMat4> bones(128, kMat4(1.0f));
        shader->setValue("finalBonesMatrices", bones);

        // Safe shadow defaults (no shadow pass)
        shader->setValue("lightSpaceMatrix", kMat4(1.0f));
        shader->setValue("shadowMap",        0);

        bindMaterialTextures(mesh, shader);

        mesh->draw();

        unbindMaterialTextures(mesh);
    }

    void kOffscreenRenderer::setupLightsFromScene(kShader *shader, kScene *scene,
                                                   int &outSun, int &outPoint, int &outSpot)
    {
        outSun = outPoint = outSpot = 0;

        shader->setValue("sceneAmbient",          scene->getAmbientLightColor());
        shader->setValue("skyboxAmbientEnabled",   scene->getSkyboxAmbientEnabled());
        shader->setValue("skyboxAmbientStrength",  scene->getSkyboxAmbientStrength());

        for (kLight *light : scene->getLights())
        {
            if (!light || !light->getActive()) continue;

            if (light->getLightType() == LIGHT_TYPE_SUN)
            {
                kString idx = std::to_string(outSun);
                shader->setValue("sunLights[" + idx + "].power",
                                 light->getPower());
                shader->setValue("sunLights[" + idx + "].direction",
                                 glm::normalize(light->getRotation() * kVec3(0.0f, -1.0f, 0.0f)));
                shader->setValue("sunLights[" + idx + "].diffuse",
                                 light->getDiffuseColor());
                shader->setValue("sunLights[" + idx + "].specular",
                                 light->getSpecularColor());
                outSun++;
            }
            else if (light->getLightType() == LIGHT_TYPE_POINT)
            {
                kString idx = std::to_string(outPoint);
                shader->setValue("pointLights[" + idx + "].power",     light->getPower());
                shader->setValue("pointLights[" + idx + "].position",  light->getPosition());
                shader->setValue("pointLights[" + idx + "].constant",  light->getConstant());
                shader->setValue("pointLights[" + idx + "].linear",    light->getLinear());
                shader->setValue("pointLights[" + idx + "].quadratic", light->getQuadratic());
                shader->setValue("pointLights[" + idx + "].diffuse",   light->getDiffuseColor());
                shader->setValue("pointLights[" + idx + "].specular",  light->getSpecularColor());
                outPoint++;
            }
            else if (light->getLightType() == LIGHT_TYPE_SPOT)
            {
                kString idx = std::to_string(outSpot);
                shader->setValue("spotLights[" + idx + "].power",       light->getPower());
                shader->setValue("spotLights[" + idx + "].position",    light->getPosition());
                shader->setValue("spotLights[" + idx + "].direction",
                                 glm::normalize(light->getRotation() * kVec3(0.0f, -1.0f, 0.0f)));
                shader->setValue("spotLights[" + idx + "].cutOff",      light->getCutOff());
                shader->setValue("spotLights[" + idx + "].outerCutOff", light->getOuterCutOff());
                shader->setValue("spotLights[" + idx + "].constant",    light->getConstant());
                shader->setValue("spotLights[" + idx + "].linear",      light->getLinear());
                shader->setValue("spotLights[" + idx + "].quadratic",   light->getQuadratic());
                shader->setValue("spotLights[" + idx + "].diffuse",     light->getDiffuseColor());
                shader->setValue("spotLights[" + idx + "].specular",    light->getSpecularColor());
                outSpot++;
            }
        }

        shader->setValue("sunLightNum",   outSun);
        shader->setValue("pointLightNum", outPoint);
        shader->setValue("spotLightNum",  outSpot);
    }

    void kOffscreenRenderer::setupSingleSunLight(kShader *shader,
                                                  kVec3 direction, kVec3 diffuse, float power)
    {
        shader->setValue("sunLights[0].power",     power);
        shader->setValue("sunLights[0].direction", direction);
        shader->setValue("sunLights[0].diffuse",   diffuse);
        shader->setValue("sunLights[0].specular",  diffuse);
    }

    void kOffscreenRenderer::bindMaterialTextures(kMesh *mesh, kShader *shader)
    {
        const auto &textures = mesh->getMaterial()->getTextures();
        for (size_t i = 0; i < textures.size(); ++i)
        {
            kTexture *tex = textures[i];
            if (!tex) continue;

            if (tex->getType() == kTextureType::TEX_TYPE_2D)
                driver->bindTexture2D((int)i, tex->getTextureID());
            else if (tex->getType() == kTextureType::TEX_TYPE_CUBE)
                driver->bindTextureCube((int)i, tex->getTextureID());

            shader->setValue(tex->getTextureName().c_str(), (unsigned int)i);
            shader->setValue("has_" + tex->getTextureName(), true);
        }
    }

    void kOffscreenRenderer::unbindMaterialTextures(kMesh *mesh)
    {
        int count = (int)mesh->getMaterial()->getTextures().size();
        for (int i = count - 1; i >= 0; --i)
        {
            driver->unbindTexture2D(i);
            driver->unbindTextureCube(i);
        }
    }

    // -----------------------------------------------------------------------
    // saveToFile
    // -----------------------------------------------------------------------

    bool kOffscreenRenderer::saveToFile(const kString &filePath) const
    {
        if (!fbo || !colorTex || filePath.empty()) return false;

        std::vector<uint8_t> pixels(width * height * 4);

        driver->bindFramebuffer(fbo);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
        driver->unbindFramebuffer();

        // OpenGL origin is bottom-left; image files expect top-left — flip vertically
        std::vector<uint8_t> flipped(pixels.size());
        int rowBytes = width * 4;
        for (int y = 0; y < height; ++y)
            std::memcpy(flipped.data() + y * rowBytes,
                        pixels.data() + (height - 1 - y) * rowBytes,
                        rowBytes);

        // Determine format from extension
        kString ext = filePath;
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext.size() >= 4 && ext.substr(ext.size() - 4) == ".png")
            return stbi_write_png(filePath.c_str(), width, height, 4,
                                  flipped.data(), rowBytes) != 0;
        if (ext.size() >= 4 && ext.substr(ext.size() - 4) == ".jpg")
            return stbi_write_jpg(filePath.c_str(), width, height, 4,
                                  flipped.data(), 90) != 0;
        if (ext.size() >= 4 && ext.substr(ext.size() - 4) == ".bmp")
            return stbi_write_bmp(filePath.c_str(), width, height, 4,
                                  flipped.data()) != 0;
        if (ext.size() >= 4 && ext.substr(ext.size() - 4) == ".tga")
            return stbi_write_tga(filePath.c_str(), width, height, 4,
                                  flipped.data()) != 0;

        return false;
    }
}
