#include "kmesh.h"

namespace kemena
{
    kMesh::kMesh(kObject *parentNode)
    {
        if (parentNode != nullptr)
            setParent(parentNode);
        setType(kNodeType::NODE_TYPE_MESH);

        calculateNormalMatrix();
    }

    kMesh::~kMesh()
    {
        kDriver *driver = kDriver::getCurrent();
        if (driver == nullptr) return;

        if (vao)               driver->deleteVertexArray(vao);
        if (indicesEbo)        driver->deleteBuffer(indicesEbo);
        if (vertexBuffer)      driver->deleteBuffer(vertexBuffer);
        if (vertexColorBuffer) driver->deleteBuffer(vertexColorBuffer);
        if (uvBuffer)          driver->deleteBuffer(uvBuffer);
        if (normalBuffer)      driver->deleteBuffer(normalBuffer);
        if (tangentBuffer)     driver->deleteBuffer(tangentBuffer);
        if (bitangentBuffer)   driver->deleteBuffer(bitangentBuffer);
        if (boneIDBuffer)      driver->deleteBuffer(boneIDBuffer);
        if (weightBuffer)      driver->deleteBuffer(weightBuffer);
    }

    void kMesh::setLoaded(bool newLoaded)
    {
        loaded = newLoaded;
    }

    bool kMesh::getLoaded()
    {
        return loaded;
    }

    void kMesh::setFileName(string newFileName)
    {
        fileName = newFileName;
    }

    string kMesh::getFileName()
    {
        return fileName;
    }

    void kMesh::setRefName(string newRefName)
    {
        refName = newRefName;
    }

    string kMesh::getRefName()
    {
        return refName;
    }

    void kMesh::setPosition(vec3 newPosition)
    {
        kObject::setPosition(newPosition);
        calculateNormalMatrix();
    }

    void kMesh::setRotation(quat newRotation)
    {
        kObject::setRotation(newRotation);
        calculateNormalMatrix();
    }

    void kMesh::setScale(vec3 newScale)
    {
        kObject::setScale(newScale);
        calculateNormalMatrix();
    }

    void kMesh::reserveBoneData(size_t vertexCount)
    {
        // Reserve space for bone IDs and weights
        boneIDs.reserve(vertexCount);
        weights.reserve(vertexCount);
    }

    void kMesh::reserveSpace(size_t vertexCount)
    {
        // Reserve space for vertices, normals, and UVs
        vertices.reserve(vertexCount);
        normals.reserve(vertexCount);
        uvs.reserve(vertexCount);

        // Reserve space for bone IDs and weights
        boneIDs.reserve(vertexCount);
        weights.reserve(vertexCount);

        // Reserve space for indices (if needed)
        indices.reserve(vertexCount * 3); // Assuming 3 indices per face (triangles)
    }

    void kMesh::setBoneInfoMap(std::map<string, kBoneInfo> newBoneInfoMap)
    {
        boneInfoMap = newBoneInfoMap;
    }

    std::map<string, kBoneInfo> &kMesh::getBoneInfoMap()
    {
        return boneInfoMap;
    }

    int &kMesh::getBoneCount()
    {
        return boneCount;
    }

    void kMesh::setBoneCount(int newBoneCount)
    {
        boneCount = newBoneCount;
    }

    void kMesh::addIndex(uint32_t index)
    {
        indices.push_back(index);
    }

    std::vector<uint32_t> kMesh::getIndices()
    {
        return indices;
    }

    void kMesh::addVertex(vec3 vertex)
    {
        vertices.push_back(vertex);
    }

    std::vector<vec3> kMesh::getVertices()
    {
        return vertices;
    }

    void kMesh::addUV(vec2 uv)
    {
        uvs.push_back(uv);
    }

    std::vector<vec2> kMesh::getUVs()
    {
        return uvs;
    }

    void kMesh::addVertexColor(vec3 color)
    {
        vertexColors.push_back(color);
    }

    std::vector<vec3> kMesh::getVertexColors()
    {
        return vertexColors;
    }

    void kMesh::addNormal(vec3 normal)
    {
        normals.push_back(normal);
    }

    std::vector<vec3> kMesh::getNormals()
    {
        return normals;
    }

    void kMesh::addTangent(vec3 tangent)
    {
        tangents.push_back(tangent);
    }

    std::vector<vec3> kMesh::getTangents()
    {
        return tangents;
    }

    void kMesh::addBitangent(vec3 bitangent)
    {
        bitangents.push_back(bitangent);
    }

    std::vector<vec3> kMesh::getBitangents()
    {
        return bitangents;
    }

    void kMesh::addBoneID(const ivec4 &boneID)
    {
        boneIDs.push_back(boneID);
    }

    void kMesh::setBoneID(size_t vertexIndex, const ivec4 &boneID)
    {
        if (vertexIndex < boneIDs.size())
        {
            boneIDs[vertexIndex] = boneID;
        }
        else
        {
            throw std::out_of_range("Vertex index out of range in getBoneID.");
        }
    }

    ivec4 kMesh::getBoneID(size_t vertexIndex)
    {
        if (vertexIndex < boneIDs.size())
        {
            return boneIDs[vertexIndex];
        }
        else
        {
            throw std::out_of_range("Vertex index out of range in getBoneID.");
        }
    }

    std::vector<ivec4> kMesh::getBoneIDs()
    {
        return boneIDs;
    }

    void kMesh::setBoneIDs(std::vector<ivec4> newBoneIDs)
    {
        boneIDs = newBoneIDs;
    }

    void kMesh::addWeight(const vec4 &weight)
    {
        weights.push_back(weight);
    }

    void kMesh::setWeight(size_t vertexIndex, const vec4 &weight)
    {
        if (vertexIndex < weights.size())
        {
            weights[vertexIndex] = weight;
        }
        else
        {
            throw std::out_of_range("Vertex index out of range in getWeight.");
        }
    }

    vec4 kMesh::getWeight(size_t vertexIndex)
    {
        if (vertexIndex < weights.size())
        {
            return weights[vertexIndex];
        }
        else
        {
            throw std::out_of_range("Vertex index out of range in getWeight.");
        }
    }

    std::vector<vec4> kMesh::getWeights()
    {
        return weights;
    }

    void kMesh::setWeights(std::vector<vec4> newWeights)
    {
        weights = newWeights;
    }

    int kMesh::getVertexCount()
    {
        return vertices.size();
    }

    uint32_t kMesh::getVertexArrayObject()
    {
        return vao;
    }

    uint32_t kMesh::getVertexBuffer()
    {
        return vertexBuffer;
    }

    uint32_t kMesh::getVertexColorBuffer()
    {
        return vertexColorBuffer;
    }

    void kMesh::setNormalMatrix(mat4 newNormalMatrix)
    {
        normalMatrix = newNormalMatrix;
    }

    mat4 kMesh::getNormalMatrix()
    {
        return normalMatrix;
    }

    void kMesh::generateVbo()
    {
        assert(vertices.size() == normals.size());
        assert(vertices.size() == uvs.size());
        assert(vertices.size() == boneIDs.size());
        assert(vertices.size() == weights.size());

        kDriver *driver = kDriver::getCurrent();

        vao = driver->createVertexArray();
        driver->bindVertexArray(vao);

        // Index buffer
        if (!indices.empty())
        {
            indicesEbo = driver->createBuffer();
            driver->uploadIndexBuffer(indicesEbo, indices.data(), indices.size() * sizeof(uint32_t));
        }

        // Vertex position (location 0)
        if (!vertices.empty())
        {
            vertexBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(vertexBuffer, vertices.data(), vertices.size() * sizeof(vec3));
            driver->setVertexAttribFloat(0, 3, sizeof(vec3), 0);
        }

        // Vertex color (location 1)
        if (!vertexColors.empty())
        {
            vertexColorBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(vertexColorBuffer, vertexColors.data(), vertexColors.size() * sizeof(vec3));
            driver->setVertexAttribFloat(1, 3, sizeof(vec3), 0);
        }

        // UV (location 2)
        if (!uvs.empty())
        {
            uvBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(uvBuffer, uvs.data(), uvs.size() * sizeof(vec2));
            driver->setVertexAttribFloat(2, 2, sizeof(vec2), 0);
        }

        // Normals (location 3)
        if (!normals.empty())
        {
            normalBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(normalBuffer, normals.data(), normals.size() * sizeof(vec3));
            driver->setVertexAttribFloat(3, 3, sizeof(vec3), 0);
        }

        // Tangents (location 4)
        if (!tangents.empty())
        {
            tangentBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(tangentBuffer, tangents.data(), tangents.size() * sizeof(vec3));
            driver->setVertexAttribFloat(4, 3, sizeof(vec3), 0);
        }

        // Bitangents (location 5)
        if (!bitangents.empty())
        {
            bitangentBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(bitangentBuffer, bitangents.data(), bitangents.size() * sizeof(vec3));
            driver->setVertexAttribFloat(5, 3, sizeof(vec3), 0);
        }

        // Bone IDs (location 6) — integer attrib
        if (!boneIDs.empty())
        {
            boneIDBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(boneIDBuffer, boneIDs.data(), boneIDs.size() * sizeof(ivec4));
            driver->setVertexAttribInt(6, 4, sizeof(ivec4), 0);
        }

        // Weights (location 7)
        if (!weights.empty())
        {
            weightBuffer = driver->createBuffer();
            driver->uploadVertexBuffer(weightBuffer, weights.data(), weights.size() * sizeof(vec4));
            driver->setVertexAttribFloat(7, 4, sizeof(vec4), 0);
        }

        driver->unbindVertexArray();

        calculateModelMatrix();
        calculateNormalMatrix();
    }

    void kMesh::calculateNormalMatrix()
    {
        setNormalMatrix(glm::transpose(glm::inverse(getModelMatrixWorld())));
    }

    void kMesh::draw()
    {
        if (vao == 0 || indices.empty())
            return;

        kDriver::getCurrent()->drawIndexed(vao, (int)indices.size());
    }

    json kMesh::serialize()
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
                {"type", "mesh"},
                {"uuid", getUuid()},
                {"name", getName()},
                {"active", getActive()},
                {"visible", getVisible()},
                {"static", getStatic()},
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
                {"file_name", getFileName()},
                {"reference", getRefName()},
            };

        return data;
    }

    void kMesh::deserialize(json data)
    {
    }

    void kMesh::setStatic(bool newStatic)
    {
        isStatic = newStatic;
    }

    bool kMesh::getStatic()
    {
        return isStatic;
    }

    void kMesh::setVisible(bool newVisible)
    {
        isVisible = newVisible;
    }

    bool kMesh::getVisible()
    {
        return isVisible;
    }

    void kMesh::setCastShadow(bool newCastShadow)
    {
        isCastShadow = newCastShadow;
    }

    bool kMesh::getCastShadow()
    {
        return isCastShadow;
    }

    void kMesh::setVertexBoneData(size_t vertexID, int boneID, float weight)
    {
        if (vertexID >= boneIDs.size() || vertexID >= weights.size())
        {
            throw std::out_of_range("Vertex ID out of range in setVertexBoneData.");
        }

        // Find the first empty slot in the bone IDs and weights for this vertex
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
        {
            if (boneIDs[vertexID][i] == -1)
            {
                boneIDs[vertexID][i] = boneID;
                weights[vertexID][i] = weight;
                return;
            }
        }

        // If no empty slot is found, throw an error
        throw std::runtime_error("Vertex already reached maximum bone influences.");
    }

    void kMesh::setAnimator(kAnimator *newAnimator)
    {
        animator = newAnimator;
        setSkinned(true);

        if (getChildren().size() > 0)
        {
            for (size_t i = 0; i < getChildren().size(); ++i)
            {
                if (getChildren().at(i)->getType() == NODE_TYPE_MESH)
                {
                    kMesh *childMesh = (kMesh *)getChildren().at(i);

                    // childMesh->setAnimator(animator);
                    childMesh->setSkinned(true);
                }
            }
        }
    }

    kAnimator *kMesh::getAnimator()
    {
        return animator;
    }

    void kMesh::setSkinned(bool newSkinned)
    {
        isSkinned = newSkinned;
    }

    bool kMesh::getSkinned()
    {
        return isSkinned;
    }
}
