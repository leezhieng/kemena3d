#include "kmesh.h"

namespace kemena
{
    kMesh::kMesh(kObject* parentNode)
    {
        if (parentNode != nullptr)
            setParent(parentNode);
        setType(kNodeType::NODE_TYPE_MESH);

        calculateNormalMatrix();
    }

    void kMesh::setLoaded(bool newLoaded)
    {
        loaded = newLoaded;
    }

    bool kMesh::getLoaded()
    {
        return loaded;
    }

    void kMesh::setFileName(std::string newFileName)
    {
        fileName = newFileName;
    }

    std::string kMesh::getFileName()
    {
        return fileName;
    }

    void kMesh::setRefName(std::string newRefName)
    {
        refName = newRefName;
    }

    std::string kMesh::getRefName()
    {
        return refName;
    }

    void kMesh::setPosition(glm::vec3 newPosition)
    {
        kObject::setPosition(newPosition);
        calculateNormalMatrix();
    }

    void kMesh::setRotation(glm::quat newRotation)
    {
        kObject::setRotation(newRotation);
        calculateNormalMatrix();
    }

    void kMesh::setScale(glm::vec3 newScale)
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

    void kMesh::setBoneInfoMap(std::map<std::string, kBoneInfo> newBoneInfoMap)
    {
        boneInfoMap = newBoneInfoMap;
    }

    std::map<std::string, kBoneInfo>& kMesh::getBoneInfoMap()
    {
        return boneInfoMap;
    }

    int& kMesh::getBoneCount()
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

    void kMesh::addVertex(glm::vec3 vertex)
    {
        vertices.push_back(vertex);
    }

    std::vector<glm::vec3> kMesh::getVertices()
    {
        return vertices;
    }

    void kMesh::addUV(glm::vec2 uv)
    {
        uvs.push_back(uv);
    }

    std::vector<glm::vec2> kMesh::getUVs()
    {
        return uvs;
    }

    void kMesh::addVertexColor(glm::vec3 color)
    {
        vertexColors.push_back(color);
    }

    std::vector<glm::vec3> kMesh::getVertexColors()
    {
        return vertexColors;
    }

    void kMesh::addNormal(glm::vec3 normal)
    {
        normals.push_back(normal);
    }

    std::vector<glm::vec3> kMesh::getNormals()
    {
        return normals;
    }

    void kMesh::addTangent(glm::vec3 tangent)
    {
        tangents.push_back(tangent);
    }

    std::vector<glm::vec3> kMesh::getTangents()
    {
        return tangents;
    }

    void kMesh::addBitangent(glm::vec3 bitangent)
    {
        bitangents.push_back(bitangent);
    }

    std::vector<glm::vec3> kMesh::getBitangents()
    {
        return bitangents;
    }

    void kMesh::addBoneID(const glm::ivec4& boneID)
    {
        boneIDs.push_back(boneID);
    }

    void kMesh::setBoneID(size_t vertexIndex, const glm::ivec4& boneID)
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

    glm::ivec4 kMesh::getBoneID(size_t vertexIndex)
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

    std::vector<glm::ivec4> kMesh::getBoneIDs()
    {
        return boneIDs;
    }

    void kMesh::setBoneIDs(std::vector<glm::ivec4> newBoneIDs)
    {
        boneIDs = newBoneIDs;
    }

    void kMesh::addWeight(const glm::vec4& weight)
    {
        weights.push_back(weight);
    }

    void kMesh::setWeight(size_t vertexIndex, const glm::vec4& weight)
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

    glm::vec4 kMesh::getWeight(size_t vertexIndex)
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

    std::vector<glm::vec4> kMesh::getWeights()
    {
        return weights;
    }

    void kMesh::setWeights(std::vector<glm::vec4> newWeights)
    {
        weights = newWeights;
    }

    int kMesh::getVertexCount()
    {
        return vertices.size();
    }

    GLuint kMesh::getVertexArrayObject()
    {
        return vao;
    }

    GLuint kMesh::getVertexBuffer()
    {
        return vertexBuffer;
    }

    GLuint kMesh::getVertexColorBuffer()
    {
        return vertexColorBuffer;
    }

    void kMesh::setNormalMatrix(glm::mat4 newNormalMatrix)
    {
        normalMatrix = newNormalMatrix;
    }

    glm::mat4 kMesh::getNormalMatrix()
    {
        return normalMatrix;
    }

    void kMesh::generateVbo()
    {
        // Make sure required buffers are valid
        assert(vertices.size() == normals.size());
        assert(vertices.size() == uvs.size());
        assert(vertices.size() == boneIDs.size());
        assert(vertices.size() == weights.size());

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // === Index Buffer ===
        if (!indices.empty()) {
            glGenBuffers(1, &indicesEbo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesEbo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        }

        // === Vertex Position (location = 0) ===
        if (!vertices.empty()) {
            glGenBuffers(1, &vertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        }

        // === Vertex Color (location = 1) ===
        if (!vertexColors.empty()) {
            glGenBuffers(1, &vertexColorBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexColorBuffer);
            glBufferData(GL_ARRAY_BUFFER, vertexColors.size() * sizeof(glm::vec3), vertexColors.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        }

        // === UV (location = 2) ===
        if (!uvs.empty()) {
            glGenBuffers(1, &uvBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
        }

        // === Normals (location = 3) ===
        if (!normals.empty()) {
            glGenBuffers(1, &normalBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        }

        // === Tangents (location = 4) ===
        if (!tangents.empty()) {
            glGenBuffers(1, &tangentBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, tangentBuffer);
            glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), tangents.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        }

        // === Bitangents (location = 5) ===
        if (!bitangents.empty()) {
            glGenBuffers(1, &bitangentBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, bitangentBuffer);
            glBufferData(GL_ARRAY_BUFFER, bitangents.size() * sizeof(glm::vec3), bitangents.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        }

        // === Bone IDs (location = 6) ===
        if (!boneIDs.empty()) {
            glGenBuffers(1, &boneIDBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, boneIDBuffer);
            glBufferData(GL_ARRAY_BUFFER, boneIDs.size() * sizeof(glm::ivec4), boneIDs.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(6);
            glVertexAttribIPointer(6, 4, GL_INT, sizeof(glm::ivec4), (void*)0); // Integer pointer!
        }

        // === Weights (location = 7) ===
        if (!weights.empty()) {
            glGenBuffers(1, &weightBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, weightBuffer);
            glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(glm::vec4), weights.data(), GL_STATIC_DRAW);
            glEnableVertexAttribArray(7);
            glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
        }

        // Clean up
        glBindVertexArray(0); // Unbind VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0); // Optional
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Optional

        calculateModelMatrix();
        calculateNormalMatrix();
    }

    void kMesh::calculateNormalMatrix()
    {
        setNormalMatrix(glm::transpose(glm::inverse(getModelMatrixWorld())));
    }

    void kMesh::draw()
    {
        if (vao == 0 || indices.empty()) return;

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0); // Optional
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
            { "type", "mesh" },
            { "uuid", getUuid() },
            { "name", getName() },
            { "active", getActive() },
            { "visible", getVisible() },
            { "static", getStatic() },
            {
                "position",
                {
                    { "x", getPosition().x },
                    { "y", getPosition().y },
                    { "z", getPosition().z }
                }
            },
            {
                "rotation",
                {
                    { "x", getRotationEuler().x },
                    { "y", getRotationEuler().y },
                    { "z", getRotationEuler().z }
                }
            },
            {
                "scale",
                {
                    { "x", getScale().x },
                    { "y", getScale().y },
                    { "z", getScale().z }
                }
            },
            { "children", childrenData },
            { "script", scriptsData },
            { "file_name", getFileName() },
            { "reference", getRefName() },
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

    void kMesh::setAnimator(kAnimator* newAnimator)
    {
        animator = newAnimator;
        setSkinned(true);

        if (getChildren().size() > 0)
        {
            for (size_t i = 0; i < getChildren().size(); ++i)
            {
                if (getChildren().at(i)->getType() == NODE_TYPE_MESH)
                {
                    kMesh* childMesh = (kMesh*) getChildren().at(i);

                    //childMesh->setAnimator(animator);
                    childMesh->setSkinned(true);
                }
            }
        }
    }

    kAnimator* kMesh::getAnimator()
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
