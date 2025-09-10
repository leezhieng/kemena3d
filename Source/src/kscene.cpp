#include "kscene.h"

namespace kemena
{
    kScene::kScene()
    {
        rootNode = new kObject();
    }

    kScene::~kScene()
    {
    }

    void kScene::setAssetManager(kAssetManager *manager)
    {
        assetManager = manager;
    }

    kAssetManager *kScene::getAssetManager()
    {
        return assetManager;
    }

    void kScene::setWorld(kWorld *newWorld)
    {
        world = newWorld;
    }

    kWorld *kScene::getWorld()
    {
        return world;
    }

    bool kScene::getActive()
    {
        return isActive;
    }

    void kScene::setActive(bool newActive)
    {
        isActive = newActive;
    }

    std::string kScene::getUuid()
    {
        return uuid;
    }

    void kScene::setUuid(std::string newUuid)
    {
        uuid = newUuid;
    }

    std::string kScene::getName()
    {
        return name;
    }

    void kScene::setName(std::string newName)
    {
        name = newName;
    }

    std::vector<kCamera *> kScene::getCameras()
    {
        return cameras;
    }

    std::vector<kLight *> kScene::getLights()
    {
        return lights;
    }

    kObject *kScene::getRootNode()
    {
        return rootNode;
    }

    void kScene::addObject(kObject *object, std::string objectUuid)
    {
        object->setParent(rootNode);

        if (objectUuid.empty())
            object->setUuid(generateUuid());
        else
            object->setUuid(objectUuid);
    }

    kMesh *kScene::addMesh(std::string fileName, std::string objectUuid)
    {
        kMesh *mesh = assetManager->loadMesh(fileName);
        mesh->setParent(rootNode);

        if (objectUuid.empty())
            mesh->setUuid(generateUuid());
        else
            mesh->setUuid(objectUuid);

        return mesh;
    }

    void kScene::addMesh(kMesh *mesh, std::string objectUuid)
    {
        mesh->setParent(rootNode);

        if (objectUuid.empty())
            mesh->setUuid(generateUuid());
        else
            mesh->setUuid(objectUuid);
    }

    kCamera *kScene::addCamera(glm::vec3 position, glm::vec3 lookAt, kCameraType type, std::string objectUuid)
    {
        kCamera *camera = new kCamera();
        camera->setCameraType(type);
        camera->setPosition(position);
        camera->setLookAt(lookAt);
        camera->setParent(rootNode);

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

    void kScene::addCamera(kCamera *camera, std::string objectUuid)
    {
        camera->setParent(rootNode);

        if (objectUuid.empty())
            camera->setUuid(generateUuid());
        else
            camera->setUuid(objectUuid);

        cameras.push_back(camera);

        // Set it as main camera if there is no main camera
        if (mainCamera == nullptr)
            mainCamera = camera;
    }

    kCamera *kScene::getMainCamera()
    {
        return mainCamera;
    }

    void kScene::setMainCamera(kCamera *camera)
    {
        mainCamera = camera;
    }

    glm::vec3 kScene::getAmbientLightColor()
    {
        return ambientLightColor;
    }

    void kScene::setAmbientLightColor(glm::vec3 newColor)
    {
        ambientLightColor = newColor;
    }

    kLight *kScene::addSunLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, std::string objectUuid)
    {
        kLight *light = new kLight();
        light->setLightType(kLightType::LIGHT_TYPE_SUN);
        light->setPosition(position);
        light->setDirection(direction);
        light->setAmbientColor(ambientColor);
        light->setDiffuseColor(diffuseColor);
        light->setSpecularColor(specularColor);
        light->setParent(rootNode);

        if (objectUuid.empty())
            light->setUuid(generateUuid());
        else
            light->setUuid(objectUuid);

        lights.push_back(light);

        return light;
    }

    kLight *kScene::addPointLight(glm::vec3 position, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, std::string objectUuid)
    {
        kLight *light = new kLight();
        light->setLightType(kLightType::LIGHT_TYPE_POINT);
        light->setPosition(position);
        light->setAmbientColor(ambientColor);
        light->setDiffuseColor(diffuseColor);
        light->setSpecularColor(specularColor);
        light->setParent(rootNode);

        if (objectUuid.empty())
            light->setUuid(generateUuid());
        else
            light->setUuid(objectUuid);

        lights.push_back(light);

        return light;
    }

    kLight *kScene::addSpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambientColor, glm::vec3 diffuseColor, glm::vec3 specularColor, std::string objectUuid)
    {
        kLight *light = new kLight();
        light->setLightType(kLightType::LIGHT_TYPE_SPOT);
        light->setPosition(position);
        light->setDirection(direction);
        light->setAmbientColor(ambientColor);
        light->setDiffuseColor(diffuseColor);
        light->setSpecularColor(specularColor);
        light->setParent(rootNode);

        if (objectUuid.empty())
            light->setUuid(generateUuid());
        else
            light->setUuid(objectUuid);

        lights.push_back(light);

        return light;
    }

    void kScene::setSkybox(kMaterial *newMaterial, kMesh *newMesh)
    {
        skyMaterial = newMaterial;
        skyMesh = newMesh;
    }

    kMaterial *kScene::getSkyboxMaterial()
    {
        return skyMaterial;
    }

    kMesh *kScene::getSkyboxMesh()
    {
        return skyMesh;
    }

    json kScene::serialize()
    {
        json objectsData = json::array();

        for (size_t i = 0; i < rootNode->getChildren().size(); ++i)
        {
            objectsData.push_back(rootNode->getChildren().at(i)->serialize());
        }

        json data =
            {
                {"uuid", getUuid()},
                {"name", getName()},
                {"active", getActive()},
                {"objects", objectsData},
            };

        return data;
    }

    void kScene::deserialize(json data)
    {
    }
}
