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

    void kCamera::setLookAt(kVec3 newLookAt)
    {
        // Look at is always the front of the camera, otherwise gizmo and icons will display at the wrong position
        if (cameraType == kCameraType::CAMERA_TYPE_FREE)
		{
			kVec3 forward = glm::normalize(newLookAt - getPosition());
			kVec3 defaultForward(0.0f, 0.0f, -1.0f);
			kQuat rotQuat = glm::rotation(defaultForward, forward);
			
			setRotation(rotQuat);
		}
		else
			lookAt = newLookAt;
    }

    kVec3 kCamera::getLookAt()
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

    kMat4 kCamera::calculateMVP(kMesh *mesh)
    {
        kMat4 model = mesh->getModelMatrixWorld();
        kMat4 view = glm::lookAt(getPosition(), lookAt, calculateUp());
        kMat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);

        return projection * view * model;
    }

    kMat4 kCamera::getViewMatrix()
    {
        kMat4 view;

        if (cameraType == kCameraType::CAMERA_TYPE_FREE)
            view = glm::lookAt(getPosition(), getPosition() + calculateForward(), calculateUp());
        else if (cameraType == kCameraType::CAMERA_TYPE_LOCKED)
            view = glm::lookAt(getPosition(), lookAt, calculateUp());

        return view;
    }

    kMat4 kCamera::getProjectionMatrix()
    {
        kMat4 projection = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        return projection;
    }

    void kCamera::rotateByMouse(kQuat rotation, float deltaX, float deltaY, float sensitivity, float pitchLimit)
    {
        float yawAngle = -deltaX * sensitivity;
        float pitchAngle = -deltaY * sensitivity;

        // Clamp pitch to prevent flipping
        const float pitchLimitRadians = glm::radians(89.0f);
        pitchAngle = glm::clamp(pitchAngle, -pitchLimitRadians, pitchLimitRadians);

        // Step 1: Yaw - rotate around global Y axis
        kQuat qYaw = glm::angleAxis(yawAngle, kVec3(0, 1, 0));
        kQuat yawedRotation = qYaw * rotation;

        // Step 2: Pitch - rotate around local right axis (after yaw)
        kVec3 right = yawedRotation * kVec3(1, 0, 0);
        kQuat qPitch = glm::angleAxis(pitchAngle, right);

        kQuat finalRotation = qPitch * yawedRotation;

        setRotation(finalRotation);
    }

    void kCamera::screenToRay(float mouseX, float mouseY,
                              float viewWidth, float viewHeight,
                              kVec3 &outOrigin, kVec3 &outDirection)
    {
        // Convert pixel coordinate to NDC [-1, 1].
        // Screen Y is top-down; NDC Y is bottom-up, so flip.
        float ndcX = (2.0f * mouseX / viewWidth)  - 1.0f;
        float ndcY =  1.0f - (2.0f * mouseY / viewHeight);

        kMat4 invVP = glm::inverse(getProjectionMatrix() * getViewMatrix());

        kVec4 nearClip = invVP * kVec4(ndcX, ndcY, -1.0f, 1.0f);
        kVec4 farClip  = invVP * kVec4(ndcX, ndcY,  1.0f, 1.0f);

        nearClip /= nearClip.w;
        farClip  /= farClip.w;

        outOrigin    = kVec3(nearClip);
        outDirection = glm::normalize(kVec3(farClip) - kVec3(nearClip));
    }

    void kCamera::setPosition(kVec3 newPosition)
    {
        kObject::setPosition(newPosition);
    }

    void kCamera::setRotation(kQuat newRotation)
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

        kString typeDisplay = "unknown";
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
                {"scene_uuid", sceneUuid},
            };

        if (!getPrefabRef().empty())    data["prefab_ref"]    = getPrefabRef();
        if (!getTemplateUuid().empty()) data["template_uuid"] = getTemplateUuid();

        return data;
    }

    void kCamera::deserialize(json data)
    {
    }
}
