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
    class KEMENA3D_API kObject
    {
    public:
        kObject(kObject *parentNode = nullptr);
        virtual ~kObject();

        kObject *getParent();
        void setParent(kObject *newParent);
        std::vector<kObject *> getChildren();

        std::vector<kScript> getScripts();

        kNodeType getType();
        void setType(kNodeType newType);

        bool getActive();
        void setActive(bool newActive);

        unsigned int getId();
        void setId(unsigned int newId);

        string getUuid();
        void setUuid(string newUuid);

        string getName();
        void setName(string newName);

        vec3 getPosition();
        virtual void setPosition(vec3 newPosition);
        quat getRotation();
        vec3 getRotationEuler();
        virtual void setRotation(quat newRotation);
        vec3 getScale();
        virtual void setScale(vec3 newScale);

        vec3 calculateRight();
        vec3 calculateForward();
        vec3 calculateUp();

        void rotate(vec3 rotationAxis, float angularSpeed);

        vec3 getGlobalPosition();
        quat getGlobalRotation();
        vec3 getGlobalScale();

        void setMaterial(kMaterial *newMaterial, bool setChildren = true);
        kMaterial *getMaterial();

        // Moved from kMesh to kObject, sometime object also need model matrix
        void calculateModelMatrix();
        mat4 getModelMatrixWorld();
        mat4 getModelMatrixLocal();

        virtual void draw();

        virtual json serialize();
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
        vec3 scale = vec3(1.0f, 1.0f, 1.0f);

        mat4 localTransform = mat4(1.0f); // Model matrix (local space)
        mat4 worldTransform = mat4(1.0f); // Model matrix (world space)

        kMaterial *material = nullptr;

        uint32_t iconVAO = 0;
        uint32_t iconVertexBuffer = 0;
        float iconVertices[12] =
            {
                -0.5f,
                -0.5f,
                0.0f,
                0.5f,
                -0.5f,
                0.0f,
                -0.5f,
                0.5f,
                0.0f,
                0.5f,
                0.5f,
                0.0f,
        };

        std::vector<kScript> scripts;
    };
}

#endif // KOBJECT_H
