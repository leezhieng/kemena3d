#include "kworld.h"

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

    string kWorld::getUuid()
    {
        return uuid;
    }

    void kWorld::setUuid(string newUuid)
    {
        uuid = newUuid;
    }

    kScene *kWorld::createScene(string sceneName, string sceneUuid)
    {
        kScene *newScene = new kScene();
        newScene->setAssetManager(assetManager);
        newScene->setWorld(this);
        newScene->setName(sceneName);

        addScene(newScene, sceneUuid);

        return newScene;
    }

    void kWorld::addScene(kScene *scene, string sceneUuid)
    {
        if (sceneUuid.empty())
            scene->setUuid(generateUuid());
        else
            scene->setUuid(sceneUuid);

        scene->setWorld(this);
        scenes.push_back(scene);
    }
	
	kCamera *kWorld::addCamera(vec3 position, vec3 lookAt, kCameraType type, string objectUuid)
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

    void kWorld::addCamera(kCamera *camera, string objectUuid)
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
