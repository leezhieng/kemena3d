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

    string kObject::getUuid()
    {
        return uuid;
    }

    void kObject::setUuid(string newUuid)
    {
        uuid = newUuid;
    }

    string kObject::getName()
    {
        return name;
    }

    void kObject::setName(string newName)
    {
        name = newName;
    }

    vec3 kObject::getPosition()
    {
        return position;
    }

    void kObject::setPosition(vec3 newPosition)
    {
        position = newPosition;
    }

    quat kObject::getRotation()
    {
        return rotation;
    }

    vec3 kObject::getRotationEuler()
    {
        vec3 eulerAngles = glm::eulerAngles(rotation);
        vec3 eulerAnglesDegrees = glm::degrees(eulerAngles);

        return eulerAnglesDegrees;
    }

    void kObject::setRotation(quat newRotation)
    {
        rotation = glm::normalize(newRotation);
    }

    vec3 kObject::getScale()
    {
        return scale;
    }

    void kObject::setScale(vec3 newScale)
    {
        scale = newScale;
    }

    vec3 kObject::calculateRight()
    {
        return glm::normalize(glm::cross(calculateUp(), calculateForward()));
    }

    vec3 kObject::calculateForward()
    {
        vec3 worldForwardAxis = vec3(0.0f, 0.0f, -1.0f); // Default forward direction in OpenGL (negative Z axis)

        // Rotate forward vector by quaternion
        vec3 front = getRotation() * worldForwardAxis;

        return glm::normalize(front);
    }

    vec3 kObject::calculateUp()
    {
        vec3 worldUpAxis = vec3(0.0f, 1.0f, 0.0f);

        return getRotation() * worldUpAxis;
    }

    void kObject::rotate(vec3 rotationAxis, float angularSpeed)
    {
        /*
        // Convert Euler angles from degrees to radians
        vec3 eulerAnglesRadians = glm::radians(eulerAnglesDegrees);

        // Create a quaternion from the Euler angles
        quat newRotation(eulerAnglesRadians);

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
        quat deltaRotation = glm::angleAxis(angle, glm::normalize(rotationAxis));

        // Apply the incremental rotation
        setRotation(deltaRotation * getRotation());
    }

    vec3 kObject::getGlobalPosition()
    {
        // return globalPosition;

        vec3 globalPos = vec3(worldTransform[3]);

        return globalPos;
    }

    quat kObject::getGlobalRotation()
    {
        // return glm::normalize(globalRotation);

        // Extract global scale
        vec3 globalSc = vec3(
            glm::length(vec3(worldTransform[0])), // X axis scale
            glm::length(vec3(worldTransform[1])), // Y axis scale
            glm::length(vec3(worldTransform[2]))  // Z axis scale
        );

        // Normalize rotation matrix by removing scaling
        mat3 rotationMat = mat3(
            vec3(worldTransform[0]) / globalSc.x,
            vec3(worldTransform[1]) / globalSc.y,
            vec3(worldTransform[2]) / globalSc.z);

        // Convert to quaternion
        quat globalRot = glm::normalize(glm::quat_cast(rotationMat));

        return globalRot;
    }

    vec3 kObject::getGlobalScale()
    {
        // return globalScale;

        // Extract global scale
        vec3 globalSc = vec3(
            glm::length(vec3(worldTransform[0])), // X axis scale
            glm::length(vec3(worldTransform[1])), // Y axis scale
            glm::length(vec3(worldTransform[2]))  // Z axis scale
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
        mat4 trans = glm::translate(mat4(1.0), getPosition());
        quat quat = quat(getRotation());
        mat4 rot = glm::toMat4(quat);
        mat4 scale = glm::scale(mat4(1.0), getScale());

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

    mat4 kObject::getModelMatrixWorld()
    {
        return worldTransform;
    }

    mat4 kObject::getModelMatrixLocal()
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
