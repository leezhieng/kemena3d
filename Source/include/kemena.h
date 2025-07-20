#ifndef KEMENA_H
#define KEMENA_H

#include "kexport.h"

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

    KEMENA3D_API kWindow* createWindow(int width, int height, std::string title);
	KEMENA3D_API kWindow* createWindow(int width, int height, std::string title, void* nativeHandle);
    KEMENA3D_API kRenderer* createRenderer(kWindow* window);
    KEMENA3D_API kAssetManager* createAssetManager();
    KEMENA3D_API kWorld* createWorld(kAssetManager* assetManager);
    KEMENA3D_API kScriptManager* createScriptManager();
}

#endif // KEMENA_H