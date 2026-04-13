#include "kmesh.h"

namespace kemena
{
    kObject::kObject(kObject *parentNode)
    {
        if (parentNode != nullptr)
            setParent(parentNode);
        setType(kNodeType::NODE_TYPE_OBJECT);
    }

    kObject::~kObject()
    {
        kDriver *driver = kDriver::getCurrent();
        if (driver == nullptr) return;

        if (iconVAO)          driver->deleteVertexArray(iconVAO);
        if (iconVertexBuffer) driver->deleteBuffer(iconVertexBuffer);
    }

    kObject *kObject::getParent()
    {
        return parent;
    }

    void kObject::setParent(kObject *newParent)
    {
        parent = newParent;
        parent->children.push_back(this);
    }

    std::vector<kObject *> kObject::getChildren()
    {
        return children;
    }

    std::vector<kScript> kObject::getScripts()
    {
        return scripts;
    }

    kNodeType kObject::getType()
    {
        return type;
    }

    void kObject::setType(kNodeType newType)
    {
        type = newType;
    }

    bool kObject::getActive()
    {
        return isActive;
    }

    void kObject::setActive(bool newActive)
    {
        isActive = newActive;
    }

    unsigned int kObject::getId()
    {
        return id;
    }

    void kObject::setId(unsigned int newId)
    {
        id = newId;
    }

    kString kObject::getUuid()
    {
        return uuid;
    }

    void kObject::setUuid(kString newUuid)
    {
        uuid = newUuid;
    }

    kString kObject::getName()
    {
        return name;
    }

    void kObject::setName(kString newName)
    {
        name = newName;
    }

    kVec3 kObject::getPosition()
    {
        return position;
    }

    void kObject::setPosition(kVec3 newPosition)
    {
        position = newPosition;
    }

    kQuat kObject::getRotation()
    {
        return rotation;
    }

    kVec3 kObject::getRotationEuler()
    {
        kVec3 eulerAngles = glm::eulerAngles(rotation);
        kVec3 eulerAnglesDegrees = glm::degrees(eulerAngles);

        return eulerAnglesDegrees;
    }

    void kObject::setRotation(kQuat newRotation)
    {
        rotation = glm::normalize(newRotation);
    }

    kVec3 kObject::getScale()
    {
        return scale;
    }

    void kObject::setScale(kVec3 newScale)
    {
        scale = newScale;
    }

    kVec3 kObject::calculateRight()
    {
        return glm::normalize(glm::cross(calculateUp(), calculateForward()));
    }

    kVec3 kObject::calculateForward()
    {
        kVec3 worldForwardAxis = kVec3(0.0f, 0.0f, -1.0f); // Default forward direction in OpenGL (negative Z axis)

        // Rotate forward vector by quaternion
        kVec3 front = getRotation() * worldForwardAxis;

        return glm::normalize(front);
    }

    kVec3 kObject::calculateUp()
    {
        kVec3 worldUpAxis = kVec3(0.0f, 1.0f, 0.0f);

        return getRotation() * worldUpAxis;
    }

    void kObject::rotate(kVec3 rotationAxis, float angularSpeed)
    {
        /*
        // Convert Euler angles from degrees to radians
        kVec3 eulerAnglesRadians = glm::radians(eulerAnglesDegrees);

        // Create a quaternion from the Euler angles
        kQuat newRotation(eulerAnglesRadians);

        if (newRotation.w < 0)
        {
            newRotation = -newRotation; // Ensure positive w component
        }

        // Combine the new rotation with the current rotation
        rotation = newRotation * getRotation(); // Apply new rotation relative to current rotation
        */

        // Compute the amount of rotation in radians
        float angle = angularSpeed;

        // Create a quaternion representing the small rotation
        kQuat deltaRotation = glm::angleAxis(angle, glm::normalize(rotationAxis));

        // Apply the incremental rotation
        setRotation(deltaRotation * getRotation());
    }

    kVec3 kObject::getGlobalPosition()
    {
        // return globalPosition;

        kVec3 globalPos = kVec3(worldTransform[3]);

        return globalPos;
    }

    kQuat kObject::getGlobalRotation()
    {
        // return glm::normalize(globalRotation);

        // Extract global scale
        kVec3 globalSc = kVec3(
            glm::length(kVec3(worldTransform[0])), // X axis scale
            glm::length(kVec3(worldTransform[1])), // Y axis scale
            glm::length(kVec3(worldTransform[2]))  // Z axis scale
        );

        // Normalize rotation matrix by removing scaling
        kMat3 rotationMat = kMat3(
            kVec3(worldTransform[0]) / globalSc.x,
            kVec3(worldTransform[1]) / globalSc.y,
            kVec3(worldTransform[2]) / globalSc.z);

        // Convert to quaternion
        kQuat globalRot = glm::normalize(glm::quat_cast(rotationMat));

        return globalRot;
    }

    kVec3 kObject::getGlobalScale()
    {
        // return globalScale;

        // Extract global scale
        kVec3 globalSc = kVec3(
            glm::length(kVec3(worldTransform[0])), // X axis scale
            glm::length(kVec3(worldTransform[1])), // Y axis scale
            glm::length(kVec3(worldTransform[2]))  // Z axis scale
        );

        return globalSc;
    }

    void kObject::setMaterial(kMaterial *newMaterial, bool setChildren)
    {
        material = newMaterial;

        if (setChildren)
        {
            if (getChildren().size() > 0)
            {
                for (size_t i = 0; i < getChildren().size(); ++i)
                {
                    getChildren().at(i)->setMaterial(newMaterial);
                }
            }
        }
    }

    kMaterial *kObject::getMaterial()
    {
        return material;
    }

    void kObject::calculateModelMatrix()
    {
        // Local transformations
        kMat4 trans = glm::translate(kMat4(1.0), getPosition());
        kQuat quat = kQuat(getRotation());
        kMat4 rot = glm::toMat4(quat);
        kMat4 scale = glm::scale(kMat4(1.0), getScale());

        localTransform = trans * rot * scale;

        // Parent global transform
        if (parent != nullptr)
        {
            // Combine parent transformations
            worldTransform = parent->getModelMatrixWorld() * localTransform;
        }
        else
        {
            worldTransform = localTransform;
        }
    }

    kMat4 kObject::getModelMatrixWorld()
    {
        return worldTransform;
    }

    kMat4 kObject::getModelMatrixLocal()
    {
        return localTransform;
    }

    void kObject::draw()
    {
        if (material == nullptr) return;

        kDriver *driver = kDriver::getCurrent();
        if (driver == nullptr) return;

        // Lazy-init icon VAO/VBO
        if (iconVAO == 0)
        {
            iconVAO = driver->createVertexArray();
            driver->bindVertexArray(iconVAO);
            iconVertexBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(iconVertexBuffer, iconVertices, sizeof(iconVertices));
            driver->setVertexAttribFloat(0, 3, 0, 0);
            driver->unbindVertexArray();
        }

        driver->drawArrays(iconVAO, kPrimitiveType::TRIANGLE_STRIP, 4);
    }

    json kObject::serialize()
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

        json data =
            {
                {"type", "object"},
                {"uuid", getUuid()},
                {"name", getName()},
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
            };

        return data;
    }

    void kObject::deserialize(json data)
    {
    }
}
