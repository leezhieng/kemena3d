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

        string getUuid();
        void setUuid(string newUuid);

        string getName();
        void setName(string newName);

        unsigned int getIncrement();
        void setIncrement(unsigned int newIncrement);

        std::vector<kObject *> getObjects();
		
        std::vector<kMesh *> getMeshes();
        std::vector<kLight *> getLights();

        kObject *getRootNode();

        void addObject(kObject *object, string objectUuid = "");

        kMesh *addMesh(string fileName, string objectUuid = "");
        void addMesh(kMesh *mesh, string objectUuid = "");

        vec3 getAmbientLightColor();
        void setAmbientLightColor(vec3 newColor);

        kLight *addSunLight(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 direction = vec3(0.0f, -1.0f, 0.0f), vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f), vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f), vec3 specularColor = vec3(1.0f, 1.0f, 1.0f), string objectUuid = "");
        kLight *addPointLight(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f), vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f), vec3 specularColor = vec3(1.0f, 1.0f, 1.0f), string objectUuid = "");
        kLight *addSpotLight(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 direction = vec3(0.0f, 1.0f, 0.0f), vec3 ambientColor = vec3(1.0f, 1.0f, 1.0f), vec3 diffuseColor = vec3(1.0f, 1.0f, 1.0f), vec3 specularColor = vec3(1.0f, 1.0f, 1.0f), string objectUuid = "");

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

        string uuid;
        string name;
		
		std::vector<kObject *> objects;

        std::vector<kMesh *> meshes;
        std::vector<kLight *> lights;

        kObject *rootNode = nullptr;

        vec3 ambientLightColor = vec3(0.0f, 0.0f, 0.0f);

        kMaterial *skyMaterial = nullptr;
        kMesh *skyMesh = nullptr;
    };
}

#endif // KSCENE_H
