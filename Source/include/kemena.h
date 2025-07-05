#ifndef KEMENA_H
#define KEMENA_H

#include "kdatatype.h"
#include "kwindow.h"
#include "krenderer.h"
#include "kassetmanager.h"
#include "kworld.h"
#include "kscriptmanager.h"

namespace kemena
{
    const std::string engineName = "Kemena3D";
    const uint32_t engineVersion = 1;

    kWindow* createWindow(int width, int height, std::string title);
    kRenderer* createRenderer(kWindow* window);
    kAssetManager* createAssetManager();
    kWorld* createWorld(kAssetManager* assetManager);
    kScriptManager* createScriptManager();
}

#endif // KEMENA_H
