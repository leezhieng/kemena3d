/**
 * @file kobject.h
 * @brief Base class for all scene-graph nodes in the Kemena3D engine.
 */

#ifndef KOBJECT_H
#define KOBJECT_H

#include "kexport.h"
#include "kdriver.h"

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <nlohmann/json.hpp>

#include "kdatatype.h"
#include "kmaterial.h"
#include "kscriptmanager.h"
#include "kphysicsobject.h"

using json = nlohmann::json;

namespace kemena
{
    /**
     * @brief Base scene-graph node.
     *
     * Every entity in the world hierarchy — meshes, lights, cameras, and plain
     * empty objects — derives from kObject. It manages parent/child
     * relationships, a local and world-space transform, an optional material,
     * and a list of attached AngelScript scripts.
     */
    class KEMENA3D_API kObject
    {
    public:
        /**
         * @brief Constructs a kObject and optionally attaches it to a parent.
         * @param parentNode Parent to attach to, or nullptr for a root node.
         */
        kObject(kObject *parentNode = nullptr);
        virtual ~kObject();

        /**
         * @brief Returns the parent node.
         * @return Pointer to the parent, or nullptr if this is a root node.
         */
        kObject *getParent();

        /**
         * @brief Reparents this object under @p newParent.
         * @param newParent New parent node.
         */
        void setParent(kObject *newParent);

        /**
         * @brief Returns the list of direct children.
         * @return Copy of the children vector.
         */
        std::vector<kObject *> getChildren();

        /** @brief Returns the list of attached scripts. */
        std::vector<kScript> getScripts();

        /**
         * @brief Returns the scene-graph node type tag.
         * @return One of the kNodeType enum values.
         */
        kNodeType getType();

        /**
         * @brief Sets the node type tag.
         * @param newType Node type.
         */
        void setType(kNodeType newType);

        /**
         * @brief Returns whether this object is active (visible/processed).
         * @return true if the object and its children are rendered/updated.
         */
        bool getActive();

        /**
         * @brief Activates or deactivates the object.
         * @param newActive false to hide and skip the object during rendering.
         */
        void setActive(bool newActive);

        /**
         * @brief Returns the numeric identifier of this object.
         * @return Unique unsigned integer ID.
         */
        unsigned int getId();

        /**
         * @brief Sets the numeric identifier.
         * @param newId New ID value.
         */
        void setId(unsigned int newId);

        /**
         * @brief Returns the UUID kString of this object.
         * @return UUID v4 kString.
         */
        kString getUuid();

        /**
         * @brief Sets the UUID kString.
         * @param newUuid UUID v4 kString.
         */
        void setUuid(kString newUuid);

        /**
         * @brief Returns the display name of this object.
         * @return Human-readable name.
         */
        kString getName();

        /**
         * @brief Sets the display name.
         * @param newName New name kString.
         */
        void setName(kString newName);

        /**
         * @brief Returns the local position.
         * @return Position in parent space (or world space if root).
         */
        kVec3 getPosition();

        /**
         * @brief Sets the local position.
         * @param newPosition Position in parent space.
         */
        virtual void setPosition(kVec3 newPosition);

        /**
         * @brief Returns the local rotation as a quaternion.
         * @return Unit quaternion representing the local orientation.
         */
        kQuat getRotation();

        /**
         * @brief Returns the local rotation as Euler angles in degrees.
         * @return XYZ Euler angles in degrees.
         */
        kVec3 getRotationEuler();

        /**
         * @brief Sets the local rotation from a quaternion.
         * @param newRotation Unit quaternion.
         */
        virtual void setRotation(kQuat newRotation);

        /**
         * @brief Returns the local scale.
         * @return Per-axis scale factors.
         */
        kVec3 getScale();

        /**
         * @brief Sets the local scale.
         * @param newScale Per-axis scale factors.
         */
        virtual void setScale(kVec3 newScale);

        /**
         * @brief Computes the local right (+X) direction in world space.
         * @return Normalised right vector.
         */
        kVec3 calculateRight();

        /**
         * @brief Computes the local forward (-Z) direction in world space.
         * @return Normalised forward vector.
         */
        kVec3 calculateForward();

        /**
         * @brief Computes the local up (+Y) direction in world space.
         * @return Normalised up vector.
         */
        kVec3 calculateUp();

        /**
         * @brief Rotates the object around an axis by a given angular speed.
         * @param rotationAxis  World-space rotation axis (normalised).
         * @param angularSpeed  Rotation magnitude in radians.
         */
        void rotate(kVec3 rotationAxis, float angularSpeed);

        /**
         * @brief Returns the world-space position derived from the world transform.
         * @return Translation column of the world transform matrix.
         */
        kVec3 getGlobalPosition();

        /**
         * @brief Returns the world-space rotation derived from the world transform.
         * @return Normalised world-space quaternion.
         */
        kQuat getGlobalRotation();

        /**
         * @brief Returns the world-space scale derived from the world transform.
         * @return Per-axis world scale.
         */
        kVec3 getGlobalScale();

        /**
         * @brief Assigns a material to this object and optionally its children.
         * @param newMaterial  Material to assign.
         * @param setChildren  If true, the same material is assigned to all descendants.
         */
        void setMaterial(kMaterial *newMaterial, bool setChildren = true);

        /**
         * @brief Returns the assigned material.
         * @return Pointer to the material, or nullptr if none is set.
         */
        kMaterial *getMaterial();

        /**
         * @brief Recomputes the local and world-space model matrices.
         *
         * Combines position, rotation, and scale into localTransform, then
         * multiplies by the parent's world transform to produce worldTransform.
         */
        void calculateModelMatrix();

        /**
         * @brief Returns the world-space model matrix.
         * @return 4x4 model matrix in world space.
         */
        kMat4 getModelMatrixWorld();

        /**
         * @brief Returns the local model matrix (relative to the parent).
         * @return 4x4 model matrix in parent/local space.
         */
        kMat4 getModelMatrixLocal();

        /**
         * @brief Draws the object.
         *
         * For base kObject (non-mesh, non-light) nodes this renders a billboard
         * icon quad. Derived classes override this for their own geometry.
         */
        virtual void draw();

        /**
         * @brief Serialises the object to a JSON value.
         * @return JSON object containing all serialisable fields.
         */
        virtual json serialize();

        /**
         * @brief Restores the object state from a JSON value.
         * @param data JSON object produced by serialize().
         */
        virtual void deserialize(json data);

        // --- Physics ---------------------------------------------------------

        /**
         * @brief Attaches a physics body to this object.
         *
         * The kObject does NOT take ownership of @p physicsObj — it must be
         * managed (and destroyed) via kPhysicsManager::destroyObject().
         *
         * @param physicsObj Physics body created by kPhysicsManager::createObject().
         */
        void attachPhysics(kPhysicsObject *physicsObj);

        /**
         * @brief Detaches the physics body without destroying it.
         */
        void detachPhysics();

        /**
         * @brief Returns the attached physics body, or nullptr if none is attached.
         */
        kPhysicsObject *getPhysicsObject();

        /**
         * @brief Copies the physics body's current position and rotation into
         *        this object's local transform.
         *
         * Call this once per frame after kPhysicsManager::update() to keep the
         * scene node in sync with the simulation.
         *
         * Has no effect if no physics body is attached.
         */
        void syncFromPhysics();

    protected:
    private:
        kObject *parent = nullptr;
        std::vector<kObject *> children;

        bool isActive = true;

        kNodeType type = NODE_TYPE_OBJECT;
        unsigned int id;
        kString uuid;
        kString name;

        kVec3 position = kVec3(0.0f, 0.0f, 0.0f);
        kQuat rotation = kQuat(kVec3(0.0f, 0.0f, 0.0f));
        kVec3 scale    = kVec3(1.0f, 1.0f, 1.0f);

        kMat4 localTransform = kMat4(1.0f); ///< Model matrix (local space).
        kMat4 worldTransform = kMat4(1.0f); ///< Model matrix (world space).

        kMaterial       *material      = nullptr;
        kPhysicsObject  *physicsObject = nullptr;

        uint32_t iconVAO = 0;          ///< VAO for the billboard icon quad (lazy-init).
        uint32_t iconVertexBuffer = 0; ///< VBO for the billboard icon vertices (lazy-init).
        float iconVertices[12] =
        {
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
             0.5f,  0.5f, 0.0f,
        };

        std::vector<kScript> scripts;
    };
}

#endif // KOBJECT_H
