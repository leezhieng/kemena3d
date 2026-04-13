/**
 * @file kscene.h
 * @brief Container for a self-contained scene (objects, lights, skybox).
 */

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

    /**
     * @brief Holds all objects, lights, and rendering settings for one scene.
     *
     * A kScene owns a root scene-graph node under which all kObject, kMesh,
     * and kLight instances are organised.  Multiple scenes can coexist inside
     * a kWorld; only active scenes are rendered.
     */
    class KEMENA3D_API kScene
    {
    public:
        kScene();
        virtual ~kScene();

        /**
         * @brief Sets the asset manager used to load resources into this scene.
         * @param manager Pointer to the asset manager; must outlive the scene.
         */
        void setAssetManager(kAssetManager *manager);

        /**
         * @brief Returns the asset manager associated with this scene.
         * @return Pointer to the asset manager, or nullptr if not set.
         */
        kAssetManager *getAssetManager();

        /**
         * @brief Associates this scene with a parent world.
         * @param newWorld Owning world instance.
         */
        void setWorld(kWorld *newWorld);

        /**
         * @brief Returns the parent world.
         * @return Pointer to the owning kWorld.
         */
        kWorld *getWorld();

        /**
         * @brief Returns whether this scene is active (rendered and updated).
         * @return true if active.
         */
        bool getActive();

        /**
         * @brief Activates or deactivates this scene.
         * @param newActive false to skip this scene during rendering.
         */
        void setActive(bool newActive);

        /**
         * @brief Returns the UUID of this scene.
         * @return UUID v4 kString.
         */
        kString getUuid();

        /**
         * @brief Sets the UUID of this scene.
         * @param newUuid UUID v4 kString.
         */
        void setUuid(kString newUuid);

        /**
         * @brief Returns the human-readable scene name.
         * @return Scene name kString.
         */
        kString getName();

        /**
         * @brief Sets the human-readable scene name.
         * @param newName New name kString.
         */
        void setName(kString newName);

        /**
         * @brief Returns the auto-increment counter used to assign object IDs.
         * @return Current counter value.
         */
        unsigned int getIncrement();

        /**
         * @brief Sets the auto-increment counter (used when deserialising).
         * @param newIncrement New counter value.
         */
        void setIncrement(unsigned int newIncrement);

        /**
         * @brief Returns all generic scene-graph objects.
         * @return Copy of the internal object vector.
         */
        std::vector<kObject *> getObjects();

        /**
         * @brief Returns all mesh nodes in the scene.
         * @return Copy of the internal mesh vector.
         */
        std::vector<kMesh *> getMeshes();

        /**
         * @brief Returns all light nodes in the scene.
         * @return Copy of the internal light vector.
         */
        std::vector<kLight *> getLights();

        /**
         * @brief Returns the root node of the scene graph.
         * @return Pointer to the root kObject.
         */
        kObject *getRootNode();

        /**
         * @brief Adds a generic object to the scene graph.
         * @param object     Object to add; ownership is not transferred.
         * @param objectUuid Optional UUID to assign; auto-generated if empty.
         */
        void addObject(kObject *object, kString objectUuid = "");

        /**
         * @brief Loads a mesh asset and adds it to the scene.
         * @param fileName   Path to the mesh asset file.
         * @param objectUuid Optional UUID for the new mesh node.
         * @return Pointer to the created kMesh.
         */
        kMesh *addMesh(kString fileName, kString objectUuid = "");

        /**
         * @brief Adds an existing mesh node to the scene graph.
         * @param mesh       Pre-constructed mesh to add.
         * @param objectUuid Optional UUID override.
         */
        void addMesh(kMesh *mesh, kString objectUuid = "");

        /**
         * @brief Returns the scene-level ambient light colour.
         * @return RGB ambient colour.
         */
        kVec3 getAmbientLightColor();

        /**
         * @brief Sets the scene-level ambient light colour.
         * @param newColor RGB ambient colour.
         */
        void setAmbientLightColor(kVec3 newColor);

        /**
         * @brief Creates and adds a directional (sun) light.
         * @param position      World-space position of the light node.
         * @param direction     Normalised light direction.
         * @param ambientColor  Ambient colour component.
         * @param diffuseColor  Diffuse colour component.
         * @param specularColor Specular colour component.
         * @param objectUuid    Optional UUID for the new node.
         * @return Pointer to the created kLight.
         */
        kLight *addSunLight(kVec3 position = kVec3(0.0f, 0.0f, 0.0f), kVec3 direction = kVec3(0.0f, -1.0f, 0.0f), kVec3 ambientColor = kVec3(1.0f, 1.0f, 1.0f), kVec3 diffuseColor = kVec3(1.0f, 1.0f, 1.0f), kVec3 specularColor = kVec3(1.0f, 1.0f, 1.0f), kString objectUuid = "");

        /**
         * @brief Creates and adds an omnidirectional point light.
         * @param position      World-space position.
         * @param ambientColor  Ambient colour component.
         * @param diffuseColor  Diffuse colour component.
         * @param specularColor Specular colour component.
         * @param objectUuid    Optional UUID for the new node.
         * @return Pointer to the created kLight.
         */
        kLight *addPointLight(kVec3 position = kVec3(0.0f, 0.0f, 0.0f), kVec3 ambientColor = kVec3(1.0f, 1.0f, 1.0f), kVec3 diffuseColor = kVec3(1.0f, 1.0f, 1.0f), kVec3 specularColor = kVec3(1.0f, 1.0f, 1.0f), kString objectUuid = "");

        /**
         * @brief Creates and adds a cone spotlight.
         * @param position      World-space position.
         * @param direction     Normalised spotlight direction.
         * @param ambientColor  Ambient colour component.
         * @param diffuseColor  Diffuse colour component.
         * @param specularColor Specular colour component.
         * @param objectUuid    Optional UUID for the new node.
         * @return Pointer to the created kLight.
         */
        kLight *addSpotLight(kVec3 position = kVec3(0.0f, 0.0f, 0.0f), kVec3 direction = kVec3(0.0f, 1.0f, 0.0f), kVec3 ambientColor = kVec3(1.0f, 1.0f, 1.0f), kVec3 diffuseColor = kVec3(1.0f, 1.0f, 1.0f), kVec3 specularColor = kVec3(1.0f, 1.0f, 1.0f), kString objectUuid = "");

        /**
         * @brief Sets the skybox material and mesh.
         * @param newMaterial Material with a cube-map texture.
         * @param newMesh     Unit-cube mesh used to render the skybox.
         */
        void setSkybox(kMaterial *newMaterial, kMesh *newMesh);

        /**
         * @brief Returns the skybox material.
         * @return Pointer to the skybox material, or nullptr if not set.
         */
        kMaterial *getSkyboxMaterial();

        /**
         * @brief Returns the skybox mesh.
         * @return Pointer to the skybox mesh, or nullptr if not set.
         */
        kMesh *getSkyboxMesh();

        /**
         * @brief Serialises the scene to JSON.
         * @return JSON object with all scene data.
         */
        virtual json serialize();

        /**
         * @brief Restores the scene from a JSON object.
         * @param data JSON produced by serialize().
         */
        virtual void deserialize(json data);

    protected:
    private:
        kAssetManager *assetManager = nullptr; ///< Asset loader reference.
        kWorld        *world        = nullptr; ///< Owning world reference.

        bool isActive = true; ///< Scene active flag.

        kString uuid; ///< Scene UUID.
        kString name; ///< Human-readable scene name.

        std::vector<kObject *> objects; ///< Generic scene-graph nodes.
        std::vector<kMesh *>   meshes;  ///< Mesh nodes.
        std::vector<kLight *>  lights;  ///< Light nodes.

        kObject *rootNode = nullptr; ///< Scene-graph root.

        kVec3 ambientLightColor = kVec3(0.0f, 0.0f, 0.0f); ///< Scene ambient colour.

        kMaterial *skyMaterial = nullptr; ///< Skybox material.
        kMesh     *skyMesh     = nullptr; ///< Skybox geometry.
    };
}

#endif // KSCENE_H
