#ifndef KWORLD_H
#define KWORLD_H

#include <string>
#include <iostream>
#include <vector>

#include "kdatatype.h"
#include "kassetmanager.h"
#include "kscene.h"
#include "kcamera.h"

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

    kScene *createScene(std::string sceneName, std::string sceneUuid = "");
    void addScene(kScene *scene, std::string sceneUuid = "");
	
	kCamera *addCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 lookAt = glm::vec3(0.0f, 0.0f, 0.0f), kCameraType type = kCameraType::CAMERA_TYPE_FREE, std::string objectUuid = "");
    void addCamera(kCamera *camera, std::string objectUuid = "");

    kCamera *getMainCamera();
    void setMainCamera(kCamera *camera);

    void setAssetManager(kAssetManager *manager);
    kAssetManager *getAssetManager();

    std::vector<kScene *> getScenes();
    std::vector<kCamera *> getCameras();

    virtual json serialize(int startScene = 0);
    virtual void deserialize(json data);

  protected:
  private:
    kAssetManager *assetManager = nullptr;
	
    std::vector<kScene *> scenes;
    std::vector<kCamera *> cameras;
	
	kCamera *mainCamera = nullptr;

    std::string uuid;
  };
}

#endif // KWORLD_H
