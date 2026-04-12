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
         * @brief Returns the UUID string of this object.
         * @return UUID v4 string.
         */
        string getUuid();

        /**
         * @brief Sets the UUID string.
         * @param newUuid UUID v4 string.
         */
        void setUuid(string newUuid);

        /**
         * @brief Returns the display name of this object.
         * @return Human-readable name.
         */
        string getName();

        /**
         * @brief Sets the display name.
         * @param newName New name string.
         */
        void setName(string newName);

        /**
         * @brief Returns the local position.
         * @return Position in parent space (or world space if root).
         */
        vec3 getPosition();

        /**
         * @brief Sets the local position.
         * @param newPosition Position in parent space.
         */
        virtual void setPosition(vec3 newPosition);

        /**
         * @brief Returns the local rotation as a quaternion.
         * @return Unit quaternion representing the local orientation.
         */
        quat getRotation();

        /**
         * @brief Returns the local rotation as Euler angles in degrees.
         * @return XYZ Euler angles in degrees.
         */
        vec3 getRotationEuler();

        /**
         * @brief Sets the local rotation from a quaternion.
         * @param newRotation Unit quaternion.
         */
        virtual void setRotation(quat newRotation);

        /**
         * @brief Returns the local scale.
         * @return Per-axis scale factors.
         */
        vec3 getScale();

        /**
         * @brief Sets the local scale.
         * @param newScale Per-axis scale factors.
         */
        virtual void setScale(vec3 newScale);

        /**
         * @brief Computes the local right (+X) direction in world space.
         * @return Normalised right vector.
         */
        vec3 calculateRight();

        /**
         * @brief Computes the local forward (-Z) direction in world space.
         * @return Normalised forward vector.
         */
        vec3 calculateForward();

        /**
         * @brief Computes the local up (+Y) direction in world space.
         * @return Normalised up vector.
         */
        vec3 calculateUp();

        /**
         * @brief Rotates the object around an axis by a given angular speed.
         * @param rotationAxis  World-space rotation axis (normalised).
         * @param angularSpeed  Rotation magnitude in radians.
         */
        void rotate(vec3 rotationAxis, float angularSpeed);

        /**
         * @brief Returns the world-space position derived from the world transform.
         * @return Translation column of the world transform matrix.
         */
        vec3 getGlobalPosition();

        /**
         * @brief Returns the world-space rotation derived from the world transform.
         * @return Normalised world-space quaternion.
         */
        quat getGlobalRotation();

        /**
         * @brief Returns the world-space scale derived from the world transform.
         * @return Per-axis world scale.
         */
        vec3 getGlobalScale();

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
        mat4 getModelMatrixWorld();

        /**
         * @brief Returns the local model matrix (relative to the parent).
         * @return 4x4 model matrix in parent/local space.
         */
        mat4 getModelMatrixLocal();

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

    protected:
    private:
        kObject *parent = nullptr;
        std::vector<kObject *> children;

        bool isActive = true;

        kNodeType type = NODE_TYPE_OBJECT;
        unsigned int id;
        string uuid;
        string name;

        vec3 position = vec3(0.0f, 0.0f, 0.0f);
        quat rotation = quat(vec3(0.0f, 0.0f, 0.0f));
        vec3 scale    = vec3(1.0f, 1.0f, 1.0f);

        mat4 localTransform = mat4(1.0f); ///< Model matrix (local space).
        mat4 worldTransform = mat4(1.0f); ///< Model matrix (world space).

        kMaterial *material = nullptr;

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
