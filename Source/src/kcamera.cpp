#include "kcamera.h"

namespace kemena
{
    kCamera::kCamera(kObject *parentNode, kCameraType type)
    {
        if (parentNode != nullptr)
            setParent(parentNode);
        setType(kNodeType::NODE_TYPE_CAMERA);

        setCameraType(type);
    }

    void kCamera::setCameraType(kCameraType newType)
    {
        cameraType = newType;

        //setLookAt(getLookAt());
    }

    kCameraType kCamera::getCameraType()
    {
        return cameraType;
    }

    void kCamera::setLookAt(glm::vec3 newLookAt)
    {
        // Look at is always the front of the camera, otherwise gizmo and icons will display at the wrong position
        if (cameraType == kCameraType::CAMERA_TYPE_FREE)
		{
			glm::vec3 forward = glm::normalize(newLookAt - getPosition());
			glm::vec3 defaultForward(0.0f, 0.0f, -1.0f);
			glm::quat rotQuat = glm::rotation(defaultForward, forward);
			
			setRotation(rotQuat);
		}
		else
			lookAt = newLookAt;
    }

    glm::vec3 kCamera::getLookAt()
    {
		// Free camera will always return the forward direction as lookAt
		
		if (cameraType == kCameraType::CAMERA_TYPE_LOCKED)
			return lookAt;
		else if (cameraType == kCameraType::CAMERA_TYPE_FREE)
			return getPosition() + calculateForward();
		
		return lookAt;
    }

    void kCamera::setFOV(float newFOV)
    {
        fov = newFOV;
    }

    float kCamera::getFOV()
    {
        return fov;
    }

    void kCamera::setNearClip(float newNearClip)
    {
        nearClip = newNearClip;
    }

    float kCamera::getNearClip()
    {
        return nearClip;
    }

    void kCamera::setFarClip(float newFarClip)
    {
        farClip = newFarClip;
    }

    float kCamera::getFarClip()
    {
        return farClip;
    }

    void kCamera::setAspectRatio(float newAspecRatio)
    {
        aspectRatio = newAspecRatio;
    }

    float kCamera::getAspectRatio()
    {
        return aspectRatio;
    }

    glm::mat4 kCamera::calculateMVP(kMesh *mesh)
    {
        glm::mat4 model = mesh->getModelMatrixWorld();
        glm::mat4 view = glm::lookAt(getPosition(), lookAt, calculateUp());
        glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);

        return projection * view * model;
    }

    glm::mat4 kCamera::getViewMatrix()
    {
        glm::mat4 view;

        if (cameraType == kCameraType::CAMERA_TYPE_FREE)
            view = glm::lookAt(getPosition(), getPosition() + calculateForward(), calculateUp());
        else if (cameraType == kCameraType::CAMERA_TYPE_LOCKED)
            view = glm::lookAt(getPosition(), lookAt, calculateUp());

        return view;
    }

    glm::mat4 kCamera::getProjectionMatrix()
    {
        glm::mat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        return projection;
    }

    void kCamera::rotateByMouse(quat rotation, float deltaX, float deltaY, float sensitivity, float pitchLimit)
    {
        float yawAngle = -deltaX * sensitivity;
        float pitchAngle = -deltaY * sensitivity;

        // Clamp pitch to prevent flipping
        const float pitchLimitRadians = glm::radians(89.0f);
        pitchAngle = glm::clamp(pitchAngle, -pitchLimitRadians, pitchLimitRadians);

        // Step 1: Yaw - rotate around global Y axis
        glm::quat qYaw = glm::angleAxis(yawAngle, glm::vec3(0, 1, 0));
        glm::quat yawedRotation = qYaw * rotation;

        // Step 2: Pitch - rotate around local right axis (after yaw)
        glm::vec3 right = yawedRotation * glm::vec3(1, 0, 0);
        glm::quat qPitch = glm::angleAxis(pitchAngle, right);

        glm::quat finalRotation = qPitch * yawedRotation;

        setRotation(finalRotation);
    }

    void kCamera::setPosition(glm::vec3 newPosition)
    {
        kObject::setPosition(newPosition);
    }

    void kCamera::setRotation(glm::quat newRotation)
    {
        kObject::setRotation(newRotation);
    }

    json kCamera::serialize()
    {
        json childrenData = json::array();
        if (getChildren().size() > 0)
        {
            for (size_t i = 0; i < getChildren().size(); ++i)
            {
                // Make sure UUID is not empty (means it's added by engine and children from import)
                if (!getChildren().at(i)->getUuid().empty())
                    childrenData.push_back(getChildren().at(i)->serialize());
            }
        }

        json scriptsData = json::array();
        if (getScripts().size() > 0)
        {
            for (size_t j = 0; j < getScripts().size(); ++j)
            {
                scriptsData.push_back({
                    {"uuid", getScripts().at(j).uuid},
                    {"active", getScripts().at(j).isActive},
                });
            }
        }

        std::string typeDisplay = "unknown";
        if (getCameraType() == kCameraType::CAMERA_TYPE_FREE)
            typeDisplay = "free";
        else if (getCameraType() == kCameraType::CAMERA_TYPE_LOCKED)
            typeDisplay = "locked";

        json data =
            {
                {"type", "camera"},
                {"uuid", getUuid()},
                {"name", getName()},
                {"camera_type", typeDisplay},
                {"active", getActive()},
                {"position",
                 {{"x", getPosition().x},
                  {"y", getPosition().y},
                  {"z", getPosition().z}}},
                {"rotation",
                 {{"x", getRotationEuler().x},
                  {"y", getRotationEuler().y},
                  {"z", getRotationEuler().z}}},
                {"scale",
                 {{"x", getScale().x},
                  {"y", getScale().y},
                  {"z", getScale().z}}},
                {"children", childrenData},
                {"script", scriptsData},
                {"look_at",
                 {{"x", getLookAt().x},
                  {"y", getLookAt().y},
                  {"z", getLookAt().z}}},
                {"up_axis",
                 {{"x", calculateUp().x},
                  {"y", calculateUp().y},
                  {"z", calculateUp().z}}},
                {"fov", getFOV()},
                {"near_clip", getNearClip()},
                {"far_clip", getFarClip()},
                {"aspect_ratio", getAspectRatio()},
            };

        return data;
    }

    void kCamera::deserialize(json data)
    {
    }
}
