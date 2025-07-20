#ifndef KOBJECT_H
#define KOBJECT_H

#include "kexport.h"

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
    class KEMENA3D_API kObject
    {
        public:
            kObject(kObject* parentNode = nullptr);
            virtual ~kObject();

            kObject* getParent();
            void setParent(kObject* newParent);
            std::vector<kObject*> getChildren();

            std::vector<kScript> getScripts();

            kNodeType getType();
            void setType(kNodeType newType);

            bool getActive();
            void setActive(bool newActive);

            unsigned int getId();
            void setId(unsigned int newId);

            std::string getUuid();
            void setUuid(std::string newUuid);

            std::string getName();
            void setName(std::string newName);

            glm::vec3 getPosition();
            virtual void setPosition(glm::vec3 newPosition);
            glm::quat getRotation();
            glm::vec3 getRotationEuler();
            virtual void setRotation(glm::quat newRotation);
            glm::vec3 getScale();
            virtual void setScale(glm::vec3 newScale);

            glm::vec3 calculateRight();
            glm::vec3 calculateForward();
            glm::vec3 calculateUp();

            void rotate(glm::vec3 rotationAxis, float angularSpeed);

            glm::vec3 getGlobalPosition();
            glm::quat getGlobalRotation();
            glm::vec3 getGlobalScale();

            void setMaterial(kMaterial* newMaterial, bool setChildren = true);
            kMaterial* getMaterial();

            // Moved from kMesh to kObject, sometime object also need model matrix
            void calculateModelMatrix();
            glm::mat4 getModelMatrixWorld();
            glm::mat4 getModelMatrixLocal();

            virtual void draw();

            virtual json serialize();
            virtual void deserialize(json data);

        protected:

        private:
            kObject* parent = nullptr;
            std::vector<kObject*> children;

            bool isActive = true;

            kNodeType type = NODE_TYPE_OBJECT;
            unsigned int id;
            std::string uuid;
            std::string name;

            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
            glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

            glm::mat4 localTransform = glm::mat4(1.0f);           // Model matrix (local space)
            glm::mat4 worldTransform = glm::mat4(1.0f);          // Model matrix (world space)

            kMaterial* material = nullptr;

            GLuint iconVertexBuffer;
            GLfloat iconVertices[12] =
            {
                -0.5f, -0.5f, 0.0f,
                0.5f, -0.5f, 0.0f,
                -0.5f, 0.5f, 0.0f,
                0.5f, 0.5f, 0.0f,
            };

            std::vector<kScript> scripts;
    };
}

#endif // KOBJECT_H
