#ifndef KWORLD_H
#define KWORLD_H

#include <string>
#include <iostream>
#include <vector>

#include "kdatatype.h"
#include "kassetmanager.h"
#include "kscene.h"

// Export macro
#ifdef _WIN32
  #ifdef KEMENA3D_STATIC
    #define KEMENA3D_API
  #elif defined(KEMENA3D_EXPORTS)
    #define KEMENA3D_API __declspec(dllexport)
  #else
    #define KEMENA3D_API __declspec(dllimport)
  #endif
#else
  #define KEMENA3D_API
#endif

namespace kemena
{
    class kScene;

    class KEMENA3D_API kWorld
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
