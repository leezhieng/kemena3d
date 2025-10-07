#ifndef KSCENE_H
#define KSCENE_H

#include "kexport.h"

#include <string>
#include <iostream>
#include <vector>
#include <typeinfo>

#include "kdatatype.h"
#include "kassetmanager.h"
#include "kworld.h"
#include "kobject.h"
#include "kmesh.h"
#include "kcamera.h"
#include "klight.h"

namespace kemena
{
    class kWorld;

    class KEMENA3D_API kScene
    {
    public:
        kScene();
        virtual ~kScene();

        void setAssetManager(kAssetManager *manager);
        kAssetManager *getAssetManager();

        void setWorld(kWorld *newWorld);
        kWorld *getWorld();

        bool getActive();
        void setActive(bool newActive);

        std::string getUuid();
        void setUuid(std::string newUuid);

        std::string getName();
        void setName(std::string newName);

        unsigned int getIncrement();
        void setIncrement(unsigned int newIncrement);

        std::vector<kObject *> getObjects();
		
        std::vector<kMesh *> getMeshes();
        std::vector<kLight *> getLights();

        kObject *getRootNode();

        void addObject(kObject *object, std::string objectUuid = "");

        kMesh *addMesh(std::string fileName, std::string objectUuid = "");
        void addMesh(kMesh *mesh, std::string objectUuid = "");

        glm::vec3 getAmbientLightColor();
        void setAmbientLightColor(glm::vec3 newColor);

        kLight *addSunLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f), std::string objectUuid = "");
        kLight *addPointLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f), std::string objectUuid = "");
        kLight *addSpotLight(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f), std::string objectUuid = "");

        void setSkybox(kMaterial *newMaterial, kMesh *newMesh);
        kMaterial *getSkyboxMaterial();
        kMesh *getSkyboxMesh();

        virtual json serialize();
        virtual void deserialize(json data);

    protected:
    private:
        kAssetManager *assetManager = nullptr;
        kWorld *world = nullptr;

        bool isActive = true;

        std::string uuid;
        std::string name;
		
		std::vector<kObject *> objects;

        std::vector<kMesh *> meshes;
        std::vector<kLight *> lights;

        kObject *rootNode = nullptr;

        glm::vec3 ambientLightColor = glm::vec3(0.0f, 0.0f, 0.0f);

        kMaterial *skyMaterial = nullptr;
        kMesh *skyMesh = nullptr;
    };
}

#endif // KSCENE_H
