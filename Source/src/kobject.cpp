#include "kmesh.h"

namespace kemena
{
    kObject::kObject(kObject *parentNode)
    {
        if (parentNode != nullptr)
            setParent(parentNode);
        setType(kNodeType::NODE_TYPE_OBJECT);

        // Generate VBO for icon
        // UV for icon is not needed as it's calculated by shader
        // Vertices
        glGenBuffers(1, &iconVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, iconVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(glm::vec3), iconVertices, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    kObject::~kObject()
    {
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

    std::string kObject::getUuid()
    {
        return uuid;
    }

    void kObject::setUuid(std::string newUuid)
    {
        uuid = newUuid;
    }

    std::string kObject::getName()
    {
        return name;
    }

    void kObject::setName(std::string newName)
    {
        name = newName;
    }

    glm::vec3 kObject::getPosition()
    {
        return position;
    }

    void kObject::setPosition(glm::vec3 newPosition)
    {
        position = newPosition;
    }

    glm::quat kObject::getRotation()
    {
        return rotation;
    }

    glm::vec3 kObject::getRotationEuler()
    {
        glm::vec3 eulerAngles = glm::eulerAngles(rotation);
        glm::vec3 eulerAnglesDegrees = glm::degrees(eulerAngles);

        return eulerAnglesDegrees;
    }

    void kObject::setRotation(glm::quat newRotation)
    {
        rotation = glm::normalize(newRotation);
    }

    glm::vec3 kObject::getScale()
    {
        return scale;
    }

    void kObject::setScale(glm::vec3 newScale)
    {
        scale = newScale;
    }

    glm::vec3 kObject::calculateRight()
    {
        return glm::normalize(glm::cross(calculateUp(), calculateForward()));
    }

    glm::vec3 kObject::calculateForward()
    {
        vec3 worldForwardAxis = vec3(0.0f, 0.0f, -1.0f); // Default forward direction in OpenGL (negative Z axis)

        // Rotate forward vector by quaternion
        glm::vec3 front = getRotation() * worldForwardAxis;

        return glm::normalize(front);
    }

    glm::vec3 kObject::calculateUp()
    {
        vec3 worldUpAxis = vec3(0.0f, 1.0f, 0.0f);

        return getRotation() * worldUpAxis;
    }

    void kObject::rotate(glm::vec3 rotationAxis, float angularSpeed)
    {
        /*
        // Convert Euler angles from degrees to radians
        glm::vec3 eulerAnglesRadians = glm::radians(eulerAnglesDegrees);

        // Create a quaternion from the Euler angles
        glm::quat newRotation(eulerAnglesRadians);

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
        glm::quat deltaRotation = glm::angleAxis(angle, glm::normalize(rotationAxis));

        // Apply the incremental rotation
        setRotation(deltaRotation * getRotation());
    }

    glm::vec3 kObject::getGlobalPosition()
    {
        // return globalPosition;

        glm::vec3 globalPos = glm::vec3(worldTransform[3]);

        return globalPos;
    }

    glm::quat kObject::getGlobalRotation()
    {
        // return glm::normalize(globalRotation);

        // Extract global scale
        glm::vec3 globalSc = glm::vec3(
            glm::length(glm::vec3(worldTransform[0])), // X axis scale
            glm::length(glm::vec3(worldTransform[1])), // Y axis scale
            glm::length(glm::vec3(worldTransform[2]))  // Z axis scale
        );

        // Normalize rotation matrix by removing scaling
        glm::mat3 rotationMat = glm::mat3(
            glm::vec3(worldTransform[0]) / globalSc.x,
            glm::vec3(worldTransform[1]) / globalSc.y,
            glm::vec3(worldTransform[2]) / globalSc.z);

        // Convert to quaternion
        glm::quat globalRot = glm::normalize(glm::quat_cast(rotationMat));

        return globalRot;
    }

    glm::vec3 kObject::getGlobalScale()
    {
        // return globalScale;

        // Extract global scale
        glm::vec3 globalSc = glm::vec3(
            glm::length(glm::vec3(worldTransform[0])), // X axis scale
            glm::length(glm::vec3(worldTransform[1])), // Y axis scale
            glm::length(glm::vec3(worldTransform[2]))  // Z axis scale
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
        glm::mat4 trans = glm::translate(glm::mat4(1.0), getPosition());
        glm::quat quat = glm::quat(getRotation());
        glm::mat4 rot = glm::toMat4(quat);
        glm::mat4 scale = glm::scale(glm::mat4(1.0), getScale());

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

    glm::mat4 kObject::getModelMatrixWorld()
    {
        return worldTransform;
    }

    glm::mat4 kObject::getModelMatrixLocal()
    {
        return localTransform;
    }

    void kObject::draw()
    {
        // Draw icon
        if (material != nullptr)
        {
            // Vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, iconVertexBuffer);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            glDisableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
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
