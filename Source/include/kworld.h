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

    string getUuid();
    void setUuid(string newUuid);

    kScene *createScene(string sceneName, string sceneUuid = "");
    void addScene(kScene *scene, string sceneUuid = "");
	
	kCamera *addCamera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 lookAt = vec3(0.0f, 0.0f, 0.0f), kCameraType type = kCameraType::CAMERA_TYPE_FREE, string objectUuid = "");
    void addCamera(kCamera *camera, string objectUuid = "");

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

    string uuid;
  };
}

#endif // KWORLD_H
