#ifndef KRENDERER_H
#define KRENDERER_H

#include "kexport.h"

#include "kwindow.h"
#include "kdatatype.h"

#include <string>
#include <vector>
#include <set>
#include <assert.h>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define NO_SDL_GLEXT
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include "kmesh.h"
#include "kcamera.h"
#include "kshader.h"
#include "klight.h"
#include "kscene.h"
#include "kobject.h"

#include <glm/gtx/string_cast.hpp>

namespace kemena
{
    class KEMENA3D_API kRenderer
    {
        public:
            kRenderer();

            bool init(kWindow* window = nullptr, kRendererType type = kRendererType::RENDERER_GL);
            void destroy();
            void setEngineInfo(const std::string name, uint32_t version);

            void render(kScene* scene, int x, int y, int width, int height, float deltaTime = 0.0f);

            glm::vec4 getClearColor();
            void setClearColor(glm::vec4 newColor);

            void setScreenShader(kShader* newShader);
            kShader* getScreenShader();

            void setShadowShader(kShader* newShader);
            kShader* getShadowShader();
			
			SDL_GLContext getOpenGlContext();

            void resizeFbo(int newWidth, int newHeight);

            float srgbToLinear(float c);
            vec3 idToRgb(unsigned int i);
            unsigned int rgbToId(unsigned int r, unsigned int g, unsigned int b);



        protected:

        private:
            std::string engineName;
            uint32_t engineVersion;
            kWindow* appWindow;

            kRendererType renderType;

            vec4 clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
            kShader* screenShader = nullptr;
            kShader* shadowShader = nullptr;
            // This is used to determine if we're still on the same frame
            int frameId = 0;

            void renderSceneGraph(kScene* scene, kObject* rootNode, bool transparent = false, float deltaTime = 0.0f);
            void renderSceneGraphShadow(kScene* scene, kObject* rootNode, mat4 lightSpaceMatrix, mat4 lightView, mat4 lightProjection, bool transparent = false, float deltaTime = 0.0f);

            // OpenGL
            SDL_GLContext openglContext;

            // Screen FBO
            GLuint quadVao, quadVbo, quadEbo;
            GLuint fbo, fboTexColor, rboDepth;
            GLuint fboMsaa, fboTexColorMsaa, rboMsaa;

            // Shadow FBO
            GLuint shadowFbo;
            const unsigned int shadowWidth = 1024, shadowHeight = 1024;
            GLuint shadowFboTex;
            mat4 lightSpaceMatrix;

            // For auto exposure
            bool enableAutoExposure = true;
            float averageLuminance;
            float averageLuminanceColor[4];
            // 0.09	Dark and moody
            // 0.18	Neutral (default for many tone mappers)
            // 0.25+	Bright scene
            float exposureKey = 0.18f;
            float exposureAdaptationRate = 2.0f;
            float exposure = 1.0f;
    };
}

#endif // KRENDERER_H
