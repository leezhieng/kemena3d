#include "kworld.h"
#include <algorithm>

namespace kemena
{
    kWorld::kWorld()
    {
        // Create a default UUID until it gets replaced (if any)
        uuid = generateUuid();
    }

    kWorld::~kWorld()
    {
    }

    kString kWorld::getUuid()
    {
        return uuid;
    }

    void kWorld::setUuid(kString newUuid)
    {
        uuid = newUuid;
    }

    kScene *kWorld::createScene(kString sceneName, kString sceneUuid)
    {
        kScene *newScene = new kScene();
        newScene->setAssetManager(assetManager);
        newScene->setWorld(this);
        newScene->setName(sceneName);

        addScene(newScene, sceneUuid);

        return newScene;
    }

    void kWorld::addScene(kScene *scene, kString sceneUuid)
    {
        if (sceneUuid.empty())
            scene->setUuid(generateUuid());
        else
            scene->setUuid(sceneUuid);

        scene->setWorld(this);
        scenes.push_back(scene);
    }
	
	kCamera *kWorld::addCamera(kVec3 position, kVec3 lookAt, kCameraType type, kString objectUuid)
    {
        kCamera *camera = new kCamera();
        camera->setCameraType(type);
        camera->setPosition(position);
        camera->setLookAt(lookAt);
        //camera->setParent(rootNode);

        if (objectUuid.empty())
            camera->setUuid(generateUuid());
        else
            camera->setUuid(objectUuid);

        cameras.push_back(camera);

        // Set it as main camera if there is no main camera
        if (mainCamera == nullptr)
            mainCamera = camera;

        return camera;
    }

    void kWorld::addCamera(kCamera *camera, kString objectUuid)
    {
        //camera->setParent(rootNode);

        if (objectUuid.empty())
            camera->setUuid(generateUuid());
        else
            camera->setUuid(objectUuid);

        cameras.push_back(camera);

        // Set it as main camera if there is no main camera
        if (mainCamera == nullptr)
            mainCamera = camera;
    }

    kCamera *kWorld::getMainCamera()
    {
        return mainCamera;
    }

    void kWorld::setMainCamera(kCamera *camera)
    {
        mainCamera = camera;
    }

    void kWorld::setAssetManager(kAssetManager *manager)
    {
        assetManager = manager;
    }

    kAssetManager *kWorld::getAssetManager()
    {
        return assetManager;
    }

    void kWorld::removeCamera(kCamera *camera)
    {
        cameras.erase(std::remove(cameras.begin(), cameras.end(), camera), cameras.end());
        if (mainCamera == camera)
            mainCamera = cameras.empty() ? nullptr : cameras[0];
    }

    void kWorld::removeScene(kScene *scene)
    {
        scenes.erase(std::remove(scenes.begin(), scenes.end(), scene), scenes.end());
    }

    std::vector<kScene *> kWorld::getScenes()
    {
        return scenes;
    }
	
	std::vector<kCamera *> kWorld::getCameras()
    {
        return cameras;
    }

    json kWorld::serialize(int startScene)
    {
        json scenesData = json::array();

        if (scenes.size() > 0)
        {
            // startScene is for skipping scene which you don't want to be serialized (eg. scene used internally by editor)
            for (size_t i = startScene; i < scenes.size(); ++i)
            {
                scenesData.push_back(scenes.at(i)->serialize());
            }
        }

        json data =
            {
                {"uuid", getUuid()},
                {"scenes", scenesData},
            };

        return data;
    }

    void kWorld::deserialize(json data)
    {
    }
}
