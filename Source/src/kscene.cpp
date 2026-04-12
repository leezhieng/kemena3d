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

    string kScene::getUuid()
    {
        return uuid;
    }

    void kScene::setUuid(string newUuid)
    {
        uuid = newUuid;
    }

    string kScene::getName()
    {
        return name;
    }

    void kScene::setName(string newName)
    {
        name = newName;
    }
	
	std::vector<kObject *> kScene::getObjects()
	{
		return objects;
	}
	
	std::vector<kMesh *> kScene::getMeshes()
    {
        return meshes;
    }

    std::vector<kLight *> kScene::getLights()
    {
        return lights;
    }

    kObject *kScene::getRootNode()
    {
        return rootNode;
    }

    void kScene::addObject(kObject *object, string objectUuid)
    {
        object->setParent(rootNode);

        if (objectUuid.empty())
            object->setUuid(generateUuid());
        else
            object->setUuid(objectUuid);
    }

    kMesh *kScene::addMesh(string fileName, string objectUuid)
    {
        kMesh *mesh = assetManager->loadMesh(fileName);
        mesh->setParent(rootNode);

        if (objectUuid.empty())
            mesh->setUuid(generateUuid());
        else
            mesh->setUuid(objectUuid);

        return mesh;
    }

    void kScene::addMesh(kMesh *mesh, string objectUuid)
    {
        mesh->setParent(rootNode);

        if (objectUuid.empty())
            mesh->setUuid(generateUuid());
        else
            mesh->setUuid(objectUuid);
    }

    vec3 kScene::getAmbientLightColor()
    {
        return ambientLightColor;
    }

    void kScene::setAmbientLightColor(vec3 newColor)
    {
        ambientLightColor = newColor;
    }

    kLight *kScene::addSunLight(vec3 position, vec3 direction, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, string objectUuid)
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

    kLight *kScene::addPointLight(vec3 position, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, string objectUuid)
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

    kLight *kScene::addSpotLight(vec3 position, vec3 direction, vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, string objectUuid)
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
