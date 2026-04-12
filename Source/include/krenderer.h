#ifndef KRENDERER_H
#define KRENDERER_H

#include "kexport.h"
#include "kdriver.h"

#include "kwindow.h"
#include "kdatatype.h"

#include <string>
#include <vector>
#include <set>
#include <assert.h>
#include <algorithm>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "kmesh.h"
#include "kcamera.h"
#include "kshader.h"
#include "klight.h"
#include "kworld.h"
#include "kscene.h"
#include "kobject.h"

#include <glm/gtx/string_cast.hpp>

namespace kemena
{
    class KEMENA3D_API kRenderer
    {
    public:
        kRenderer();

        bool init(kWindow *window = nullptr, kRendererType type = kRendererType::RENDERER_GL);
        void destroy();
        void setEngineInfo(const string name, uint32_t version);

        kWindow *getWindow();
        kDriver *getDriver();

        void clear();
        void render(kWorld *world, kScene *scene, int x, int y, int width, int height, float deltaTime = 0.0f, bool autoClearSwapWindow = true);

        vec4 getClearColor();
        void setClearColor(vec4 newColor);

        void setEnableScreenBuffer(bool newEnable, bool useDefaultShader = true);
        bool getEnableScreenBuffer();
        void setScreenShader(kShader *newShader);
        kShader *getScreenShader();

        void setEnableShadow(bool newEnable, bool useDefaultShader = true);
        bool getEnableShadow();
        void setShadowShader(kShader *newShader);
        kShader *getShadowShader();

        void setEnableAutoExposure(bool newEnable);
        bool getEnableAutoExposure();

        void resizeFbo(int newWidth, int newHeight);
        uint32_t getFboTexture();
        int getFboWidth();
        int getFboHeight();

        float srgbToLinear(float c);
        vec3 idToRgb(unsigned int i);
        unsigned int rgbToId(unsigned int r, unsigned int g, unsigned int b);

    protected:
    private:
        string engineName;
        uint32_t engineVersion = 0;
        kWindow *appWindow = nullptr;

        kRendererType renderType;
        kDriver *driver = nullptr;

        vec4 clearColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);

        // Used to detect same-frame animation updates
        int frameId = 0;

        void renderSceneGraph(kWorld *world, kScene *scene, kObject *rootNode, bool transparent = false, float deltaTime = 0.0f);
        void renderSceneGraphShadow(kWorld *world, kScene *scene, kObject *rootNode, mat4 lightSpaceMatrix, mat4 lightView, mat4 lightProjection, bool transparent = false, float deltaTime = 0.0f);

        // Screen FBO
        bool enableScreenBuffer = false;
        kShader *screenShader = nullptr;
        uint32_t quadVao = 0, quadVbo = 0, quadEbo = 0;
        uint32_t fbo = 0, fboTexColor = 0, rboDepth = 0;
        uint32_t fboMsaa = 0, fboTexColorMsaa = 0, rboDepthMsaa = 0;

        int fboWidth = 0, fboHeight = 0;

        // Shadow FBO
        bool enableShadow = false;
        kShader *shadowShader = nullptr;
        uint32_t shadowFbo = 0;
        const unsigned int shadowWidth = 1024, shadowHeight = 1024;
        uint32_t shadowFboTex = 0;
        mat4 lightSpaceMatrix;

        // Auto exposure
        bool enableAutoExposure = false;
        float averageLuminance = 0.0f;
        float averageLuminanceColor[4] = {};
        float exposureKey = 0.18f;
        float exposureAdaptationRate = 2.0f;
        float exposure = 1.0f;
    };
}

#endif // KRENDERER_H
