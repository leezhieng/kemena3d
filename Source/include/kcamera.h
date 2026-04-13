/**
 * @file kcamera.h
 * @brief Perspective camera node (free-look or look-at).
 */

#ifndef KCAMERA_H
#define KCAMERA_H

#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "kdatatype.h"
#include "kmesh.h"

namespace kemena
{
    /**
     * @brief Scene-graph camera node.
     *
     * Supports two modes selected at construction:
     * - @c CAMERA_TYPE_FREE  — orientation is controlled by position + rotation.
     * - @c CAMERA_TYPE_LOCKED — orientation always faces the look-at target.
     *
     * View and projection matrices are computed on demand via getViewMatrix()
     * and getProjectionMatrix().
     */
    class kCamera : public kObject
    {
    public:
        /**
         * @brief Constructs a camera and optionally attaches it to a parent.
         * @param parentNode Parent scene-graph node, or nullptr.
         * @param type       Camera mode (free-look or locked look-at).
         */
        kCamera(kObject *parentNode = nullptr, kCameraType type = kCameraType::CAMERA_TYPE_FREE);

        /**
         * @brief Sets the camera mode.
         * @param newType New camera mode.
         */
        void setCameraType(kCameraType newType);

        /**
         * @brief Returns the current camera mode.
         * @return Camera type enum value.
         */
        kCameraType getCameraType();

        /**
         * @brief Sets the look-at target used when the camera is locked.
         * @param newLookAt World-space point the camera faces.
         */
        void setLookAt(kVec3 newLookAt);

        /**
         * @brief Returns the look-at target.
         * @return World-space target point.
         */
        kVec3 getLookAt();

        /**
         * @brief Sets the vertical field of view.
         * @param newFOV FOV in degrees.
         */
        void setFOV(float newFOV);

        /**
         * @brief Returns the vertical field of view.
         * @return FOV in degrees.
         */
        float getFOV();

        /**
         * @brief Sets the near clipping plane distance.
         * @param newNearClip Near plane distance (positive, in world units).
         */
        void setNearClip(float newNearClip);

        /**
         * @brief Returns the near clipping plane distance.
         * @return Near plane distance.
         */
        float getNearClip();

        /**
         * @brief Sets the far clipping plane distance.
         * @param newFarClip Far plane distance (positive, in world units).
         */
        void setFarClip(float newFarClip);

        /**
         * @brief Returns the far clipping plane distance.
         * @return Far plane distance.
         */
        float getFarClip();

        /**
         * @brief Sets the viewport aspect ratio (width / height).
         * @param newAspectRatio Aspect ratio.
         */
        void setAspectRatio(float newAspectRatio);

        /**
         * @brief Returns the viewport aspect ratio.
         * @return Aspect ratio (width / height).
         */
        float getAspectRatio();

        /**
         * @brief Computes the combined Model-View-Projection matrix for a mesh.
         * @param mesh Target mesh whose model matrix is used.
         * @return MVP matrix.
         */
        kMat4 calculateMVP(kMesh *mesh);

        /**
         * @brief Returns the view matrix for this camera.
         * @return World-to-view transform.
         */
        kMat4 getViewMatrix();

        /**
         * @brief Returns the projection matrix for this camera.
         * @return Perspective projection matrix.
         */
        kMat4 getProjectionMatrix();

        /**
         * @brief Applies mouse-delta rotation to a free-look camera.
         * @param rotation    Current orientation quaternion.
         * @param deltaX      Horizontal mouse delta in pixels.
         * @param deltaY      Vertical mouse delta in pixels.
         * @param sensitivity Radians-per-pixel multiplier (default 0.005).
         * @param pitchLimit  Maximum pitch angle in degrees (default 89).
         */
        void rotateByMouse(kQuat rotation, float deltaX, float deltaY, float sensitivity = 0.005f, float pitchLimit = 89.0f);

        /** @brief Sets the local position and updates all dependent matrices. */
        void setPosition(kVec3 newPosition);
        /** @brief Sets the local rotation and updates all dependent matrices. */
        void setRotation(kQuat newRotation);

        /**
         * @brief Serialises the camera to JSON.
         * @return JSON object with all camera fields.
         */
        json serialize();

        /**
         * @brief Restores the camera state from a JSON object.
         * @param data JSON produced by serialize().
         */
        void deserialize(json data);

    protected:
    private:
        kCameraType cameraType;
        kVec3  lookAt      = kVec3(0.0f, 0.0f, 0.0f); ///< Look-at target (locked mode).
        float fov         = 45.0f;                   ///< Vertical FOV in degrees.
        float nearClip    = 0.1f;                    ///< Near clipping distance.
        float farClip     = 100.0f;                  ///< Far clipping distance.
        float aspectRatio = 1.0f;                    ///< Viewport aspect ratio.
    };
}

#endif // KCAMERA_H
