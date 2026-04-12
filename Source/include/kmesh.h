#ifndef KMESH_H
#define KMESH_H

#include "kexport.h"
#include "kdriver.h"

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

    class KEMENA3D_API kMesh : public kObject
    {
    public:
        kMesh(kObject *parentNode = nullptr);
        ~kMesh();

        void setLoaded(bool newLoaded);
        bool getLoaded();

        void setFileName(string newFileName);
        string getFileName();

        void setRefName(string newRefName);
        string getRefName();

        void setPosition(vec3 newPosition);
        void setRotation(quat newRotation);
        void setScale(vec3 newScale);

        void reserveBoneData(size_t vertexCount);
        void reserveSpace(size_t vertexCount);

        void setBoneInfoMap(std::map<string, kBoneInfo> newBoneInfoMap);
        std::map<string, kBoneInfo> &getBoneInfoMap();
        int &getBoneCount();
        void setBoneCount(int newBoneCount);

        void addIndex(uint32_t index);
        std::vector<uint32_t> getIndices();

        void addVertex(vec3 vertex);
        std::vector<vec3> getVertices();

        void addUV(vec2 uv);
        std::vector<vec2> getUVs();

        void addVertexColor(vec3 color);
        std::vector<vec3> getVertexColors();

        void addNormal(vec3 normal);
        std::vector<vec3> getNormals();

        void addTangent(vec3 tangent);
        std::vector<vec3> getTangents();

        void addBitangent(vec3 bitangent);
        std::vector<vec3> getBitangents();

        void addBoneID(const ivec4 &boneID);
        void setBoneID(size_t vertexIndex, const ivec4 &boneID);
        ivec4 getBoneID(size_t vertexIndex);
        std::vector<ivec4> getBoneIDs();
        void setBoneIDs(std::vector<ivec4> newBoneIDs);

        void addWeight(const vec4 &weight);
        void setWeight(size_t vertexIndex, const vec4 &weight);
        vec4 getWeight(size_t vertexIndex);
        std::vector<vec4> getWeights();
        void setWeights(std::vector<vec4> newWeights);

        int getVertexCount();

        uint32_t getVertexArrayObject();
        uint32_t getVertexBuffer();
        uint32_t getVertexColorBuffer();

        void setNormalMatrix(mat4 newNormalMatrix);
        mat4 getNormalMatrix();

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

        void setAnimator(kAnimator *newAnimator);
        kAnimator *getAnimator();

        void setSkinned(bool newSkinned);
        bool getSkinned();

    protected:
    private:
        bool loaded = false;

        string fileName;
        string refName;

        std::vector<vec3> vertices;
        std::vector<uint32_t> indices;
        std::vector<vec2> uvs;
        std::vector<vec3> vertexColors;
        std::vector<vec3> normals;
        std::vector<vec3> tangents;
        std::vector<vec3> bitangents;
        std::vector<ivec4> boneIDs;
        std::vector<vec4> weights;

        uint32_t vao = 0;
        uint32_t indicesEbo = 0;

        uint32_t vertexBuffer = 0;
        uint32_t vertexColorBuffer = 0;
        uint32_t uvBuffer = 0;
        uint32_t normalBuffer = 0;
        uint32_t tangentBuffer = 0;
        uint32_t bitangentBuffer = 0;
        uint32_t boneIDBuffer = 0;
        uint32_t weightBuffer = 0;

        mat3 normalMatrix;

        bool isStatic = false;
        bool isVisible = true;
        bool isCastShadow = true;

        std::map<string, kBoneInfo> boneInfoMap;
        int boneCount = 0;

        kAnimator *animator = nullptr;
        bool isSkinned = false;
    };
}

#endif // KMESH_H
