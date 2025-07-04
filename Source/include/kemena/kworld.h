#ifndef KWORLD_H
#define KWORLD_H

#include <string>
#include <iostream>
#include <vector>

#include "kdatatype.h"
#include "kassetmanager.h"
#include "kscene.h"

namespace kemena
{
    class kScene;

    class kWorld
    {
        public:
            kWorld();
            virtual ~kWorld();

            std::string getUuid();
            void setUuid(std::string newUuid);

            kScene* createScene(std::string sceneName, std::string sceneUuid = "");
            void addScene(kScene* scene, std::string sceneUuid = "");

            void setAssetManager(kAssetManager* manager);
            kAssetManager* getAssetManager();

            std::vector<kScene*> getScenes();

            virtual json serialize(int startScene = 0);
            virtual void deserialize(json data);

        protected:

        private:
            kAssetManager* assetManager = nullptr;
            std::vector<kScene*> scenes;

            std::string uuid;
    };
}

#endif // KWORLD_H
