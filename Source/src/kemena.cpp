#include "kemena.h"

namespace kemena
{
    kWindow* createWindow(int width, int height, std::string title)
    {
        kWindow* window = new kWindow;
        bool done = window->init(width, height, title);
        if (done)
            return window;
        else
            return nullptr;
    }
	
	kWindow* createWindow(int width, int height, std::string title, void* nativeHandle)
    {
        kWindow* window = new kWindow;
        bool done = window->init(width, height, title, nativeHandle);
        if (done)
            return window;
        else
            return nullptr;
    }

    kRenderer* createRenderer(kWindow* window)
    {
        kRenderer* renderer = new kRenderer;
        renderer->setEngineInfo(engineName, engineVersion);
        bool done = renderer->init(window);
        if (done)
            return renderer;
        else
            return nullptr;
    }

    kAssetManager* createAssetManager()
    {
        kAssetManager* manager = new kAssetManager();
        return manager;
    }

    kWorld* createWorld(kAssetManager* assetManager)
    {
        kWorld* manager = new kWorld();
        manager->setAssetManager(assetManager);
        return manager;
    }

    kScriptManager* createScriptManager()
    {
        kScriptManager* manager = new kScriptManager();
        return manager;
    }
	
	kGuiManager* createGuiManager(kRenderer* renderer)
	{
		kGuiManager* manager = new kGuiManager();
		manager->init(renderer);
		return manager;
	}
}
