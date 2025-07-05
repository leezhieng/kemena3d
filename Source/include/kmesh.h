#ifndef KMESH_H
#define KMESH_H

#include <map>
#include <vector>
#include <string>

#include "kobject.h"
#include "kbone.h"
#include "kanimation.h"
#include "kanimator.h"

namespace kemena
{
    class kAnimator;

    class kMesh : public kObject
    {
        public:
            kMesh(kObject* parentNode = nullptr);

            void setLoaded(bool newLoaded);
            bool getLoaded();

            void setFileName(std::string newFileName);
            std::string getFileName();

            void setRefName(std::string newRefName);
            std::string getRefName();

            void setPosition(glm::vec3 newPosition);
            void setRotation(glm::quat newRotation);
            void setScale(glm::vec3 newScale);

            void reserveBoneData(size_t vertexCount);
            void reserveSpace(size_t vertexCount);

            void setBoneInfoMap(std::map<std::string, kBoneInfo> newBoneInfoMap);
            std::map<std::string, kBoneInfo>& getBoneInfoMap();
            int& getBoneCount();
            void setBoneCount(int newBoneCount);

            void addIndex(uint32_t index);
            std::vector<uint32_t> getIndices();

            void addVertex(glm::vec3 vertex);
            std::vector<glm::vec3> getVertices();

            void addUV(glm::vec2 uv);
            std::vector<glm::vec2> getUVs();

            void addVertexColor(glm::vec3 color);
            std::vector<glm::vec3> getVertexColors();

            void addNormal(glm::vec3 normal);
            std::vector<glm::vec3> getNormals();

            void addTangent(glm::vec3 tangent);
            std::vector<glm::vec3> getTangents();

            void addBitangent(glm::vec3 bitangent);
            std::vector<glm::vec3> getBitangents();

            void addBoneID(const glm::ivec4& boneID);
            void setBoneID(size_t vertexIndex, const glm::ivec4& boneID);
            glm::ivec4 getBoneID(size_t vertexIndex);
            std::vector<glm::ivec4> getBoneIDs();
            void setBoneIDs(std::vector<glm::ivec4> newBoneIDs);

            void addWeight(const glm::vec4& weight);
            void setWeight(size_t vertexIndex, const glm::vec4& weight);
            glm::vec4 getWeight(size_t vertexIndex);
            std::vector<glm::vec4> getWeights();
            void setWeights(std::vector<glm::vec4> newWeights);

            int getVertexCount();

            GLuint getVertexArrayObject();
            GLuint getVertexBuffer();
            GLuint getVertexColorBuffer();

            void setNormalMatrix(glm::mat4 newNormalMatrix);
            glm::mat4 getNormalMatrix();

            void generateVbo();
            void calculateNormalMatrix();

            void draw();

            json serialize();
            void deserialize(json data);

            void setStatic(bool newStatic);
            bool getStatic();

            void setVisible(bool newVisible);
            bool getVisible();

            void setCastShadow(bool newCastShadow);
            bool getCastShadow();

            void setVertexBoneData(size_t vertexID, int boneID, float weight);

            void setAnimator(kAnimator* newAnimator);
            kAnimator* getAnimator();

            void setSkinned(bool newSkinned);
            bool getSkinned();

        protected:

        private:
            bool loaded = false;

            std::string fileName;
            std::string refName;

            std::vector<vec3> vertices;
            std::vector<uint32_t> indices;
            std::vector<vec2> uvs;
            std::vector<vec3> vertexColors;
            std::vector<vec3> normals;
            std::vector<vec3> tangents;
            std::vector<vec3> bitangents;
            std::vector<ivec4> boneIDs;
            std::vector<vec4> weights;

            GLuint vao;
            GLuint indicesEbo;

            GLuint vertexBuffer;
            GLuint vertexColorBuffer;
            GLuint uvBuffer;
            GLuint normalBuffer;
            GLuint tangentBuffer;
            GLuint bitangentBuffer;
            GLuint boneIDBuffer;
            GLuint weightBuffer;

            mat3 normalMatrix;

            bool isStatic = false;
            bool isVisible = true;
            bool isCastShadow = true;

            std::map<string, kBoneInfo> boneInfoMap;
            int boneCount = 0;

            kAnimator* animator = nullptr;
            bool isSkinned = false;
    };
}

#endif // KMESH_H
