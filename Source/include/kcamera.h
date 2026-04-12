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
    class kCamera : public kObject
    {
    public:
        kCamera(kObject *parentNode = nullptr, kCameraType type = kCameraType::CAMERA_TYPE_FREE);

        void setCameraType(kCameraType newType);
        kCameraType getCameraType();

        void setLookAt(vec3 newLookAt);
        vec3 getLookAt();
        ;

        void setFOV(float newFOV);
        float getFOV();
        void setNearClip(float newNearClip);
        float getNearClip();
        void setFarClip(float newFarClip);
        float getFarClip();
        void setAspectRatio(float newAspectRatio);
        float getAspectRatio();

        mat4 calculateMVP(kMesh *mesh);
        mat4 getViewMatrix();
        mat4 getProjectionMatrix();

        void rotateByMouse(quat rotation, float deltaX, float deltaY, float sensitivity = 0.005f, float pitchLimit = 89.0f);

        void setPosition(vec3 newPosition);
        void setRotation(quat newRotation);

        json serialize();
        void deserialize(json data);

    protected:
    private:
        kCameraType cameraType;
        vec3 lookAt = vec3(0.0f, 0.0f, 0.0f); // Used by locked camera
        float fov = 45.0f;
        float nearClip = 0.1f;
        float farClip = 100.0f;
        float aspectRatio = 1.0f;
    };
}

#endif // KCAMERA_H
