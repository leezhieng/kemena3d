/**
 * @file kworld.h
 * @brief Top-level container holding scenes and cameras.
 */

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

    /**
     * @brief Root container for the entire simulation environment.
     *
     * A kWorld owns one or more kScene instances and a camera list.
     * The renderer iterates over active scenes and draws them through the
     * main camera returned by getMainCamera().
     *
     * Typical setup:
     * @code
     *   kWorld world;
     *   world.setAssetManager(&assetMgr);
     *   kScene *scene = world.createScene("Main");
     *   kCamera *cam  = world.addCamera(vec3(0,0,5));
     *   world.setMainCamera(cam);
     * @endcode
     */
    class KEMENA3D_API kWorld
    {
    public:
        kWorld();
        virtual ~kWorld();

        /**
         * @brief Returns the UUID of this world.
         * @return UUID v4 string.
         */
        string getUuid();

        /**
         * @brief Sets the UUID of this world.
         * @param newUuid UUID v4 string.
         */
        void setUuid(string newUuid);

        /**
         * @brief Creates a new scene and registers it in this world.
         * @param sceneName Human-readable name for the scene.
         * @param sceneUuid Optional UUID; auto-generated if empty.
         * @return Pointer to the newly created kScene.
         */
        kScene *createScene(string sceneName, string sceneUuid = "");

        /**
         * @brief Registers an existing scene in this world.
         * @param scene     Pre-constructed scene to add.
         * @param sceneUuid Optional UUID override.
         */
        void addScene(kScene *scene, string sceneUuid = "");

        /**
         * @brief Creates and registers a camera in this world.
         * @param position   World-space initial position.
         * @param lookAt     Initial look-at target (for locked cameras).
         * @param type       Camera mode (free or locked).
         * @param objectUuid Optional UUID for the camera node.
         * @return Pointer to the newly created kCamera.
         */
        kCamera *addCamera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 lookAt = vec3(0.0f, 0.0f, 0.0f), kCameraType type = kCameraType::CAMERA_TYPE_FREE, string objectUuid = "");

        /**
         * @brief Registers an existing camera in this world.
         * @param camera     Pre-constructed camera to add.
         * @param objectUuid Optional UUID override.
         */
        void addCamera(kCamera *camera, string objectUuid = "");

        /**
         * @brief Returns the camera used by the renderer for the main view.
         * @return Pointer to the main camera, or nullptr if not set.
         */
        kCamera *getMainCamera();

        /**
         * @brief Sets the main camera used by the renderer.
         * @param camera Pointer to the desired camera.
         */
        void setMainCamera(kCamera *camera);

        /**
         * @brief Assigns the asset manager used by scenes in this world.
         * @param manager Asset manager instance; must outlive the world.
         */
        void setAssetManager(kAssetManager *manager);

        /**
         * @brief Returns the asset manager.
         * @return Pointer to the asset manager, or nullptr if not set.
         */
        kAssetManager *getAssetManager();

        /**
         * @brief Returns all scenes registered in this world.
         * @return Copy of the internal scene vector.
         */
        std::vector<kScene *> getScenes();

        /**
         * @brief Returns all cameras registered in this world.
         * @return Copy of the internal camera vector.
         */
        std::vector<kCamera *> getCameras();

        /**
         * @brief Serialises the world to JSON.
         * @param startScene Index of the first scene to include (default 0).
         * @return JSON object with UUID, scenes, and cameras.
         */
        virtual json serialize(int startScene = 0);

        /**
         * @brief Restores the world from a JSON object.
         * @param data JSON produced by serialize().
         */
        virtual void deserialize(json data);

    protected:
    private:
        kAssetManager *assetManager = nullptr; ///< Asset loader reference.

        std::vector<kScene *>  scenes;  ///< Registered scenes.
        std::vector<kCamera *> cameras; ///< Registered cameras.

        kCamera *mainCamera = nullptr; ///< Active render camera.

        string uuid; ///< World UUID.
    };
}

#endif // KWORLD_H
