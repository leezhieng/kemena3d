#include "kassetmanager.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

namespace kemena
{
    kAssetManager::kAssetManager()
    {
    }

    std::string kAssetManager::getFileExtension(const std::string &fileName)
    {
        std::string::size_type idx;
        idx = fileName.rfind('.');

        if (idx != std::string::npos)
        {
            std::string extension = fileName.substr(idx + 1);
            return extension;
        }

        // No extension found
        return "";
    }

    bool kAssetManager::fileExists(const std::string &fileName)
    {
        bool ret;
        FILE *fp = fopen(fileName.c_str(), "rb");
        if (fp)
        {
            ret = true;
            fclose(fp);
        }
        else
        {
            ret = false;
        }
        return ret;
    }

    std::string kAssetManager::getBaseDir(const std::string &filePath)
    {
        if (filePath.find_last_of("/\\") != std::string::npos)
            return filePath.substr(0, filePath.find_last_of("/\\"));
        return "";
    }

    std::string kAssetManager::getBaseFilename(const std::string &filePath)
    {
        if (filePath.find_last_of("/\\") != std::string::npos)
            return filePath.substr(filePath.find_last_of("/\\") + 1, filePath.size());
        return "";
    }

    std::string kAssetManager::getExecDir()
    {
        return std::string(SDL_GetBasePath());
    }

    std::string kAssetManager::popDir(const std::string &filePath)
    {
        if (filePath.find_last_of("/\\") != std::string::npos)
            return filePath.substr(0, filePath.find_last_of("/\\"));
        return "";
    }

    kTexture2D *kAssetManager::loadTexture2D(const std::string fileName, const std::string textureName, const kTextureFormat format, const bool flipVertical, const bool keepData)
    {
        // std::cout << textureName << std::endl;

        int width;
        int height;
        int channels;

        if (flipVertical)
            stbi_set_flip_vertically_on_load(true);

        unsigned char *data = stbi_load(fileName.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (data)
        {
            if (format == kTextureFormat::TEX_FORMAT_RGB)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else if (format == kTextureFormat::TEX_FORMAT_RGBA)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else if (format == kTextureFormat::TEX_FORMAT_SRGB)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else if (format == kTextureFormat::TEX_FORMAT_SRGBA)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture:" << fileName << std::endl;
        }

        kTexture2D *texture = new kTexture2D();

        texture->setType(TEX_TYPE_2D);
        texture->setTextureID(textureID);
        texture->setWidth(width);
        texture->setHeight(height);
        texture->setChannels(channels);
        texture->setTextureName(textureName);

        // Normally no need keep the data unless you need it to modify or export the texture again later
        if (keepData)
            texture->setData(data);
        else
            stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }

    kTexture2D *kAssetManager::loadTexture2DFromMemory(const aiTexture *rawData, const std::string textureName, const kTextureFormat format, const bool flipVertical, const bool keepData)
    {
        unsigned char *data = nullptr;

        int width;
        int height;
        int channels;

        if (flipVertical)
            stbi_set_flip_vertically_on_load(true);

        if (rawData->mHeight == 0)
        {
            data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(rawData->pcData), rawData->mWidth, &width, &height, &channels, 0);
        }
        else
        {
            data = stbi_load_from_memory(reinterpret_cast<unsigned char *>(rawData->pcData), rawData->mWidth * rawData->mHeight, &width, &height, &channels, 0);
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (data)
        {
            if (format == kTextureFormat::TEX_FORMAT_RGB)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else if (format == kTextureFormat::TEX_FORMAT_RGBA)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            else if (format == kTextureFormat::TEX_FORMAT_SRGB)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else if (format == kTextureFormat::TEX_FORMAT_SRGBA)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture from memory" << std::endl;
        }

        kTexture2D *texture = new kTexture2D();
        texture->setTextureID(textureID);

        texture->setWidth(width);
        texture->setHeight(height);
        texture->setChannels(channels);
        texture->setTextureName(textureName);

        // Normally no need keep the data unless you need it to modify or export the texture again later
        if (keepData)
            texture->setData(data);
        else
            stbi_image_free(data);

        glBindTexture(GL_TEXTURE_2D, 0);

        return texture;
    }

    void kAssetManager::saveTexture2D(kTexture2D *texture, const std::string fileName, std::string format)
    {
        if (format == "png")
        {
            stbi_write_png(fileName.c_str(), texture->getWidth(), texture->getHeight(), texture->getChannels(), texture->getData(), texture->getWidth() * texture->getChannels());
        }
        else if (format == "jpg")
        {
            stbi_write_jpg(fileName.c_str(), texture->getWidth(), texture->getHeight(), texture->getChannels(), texture->getData(), 12);
        }
        else if (format == "tga")
        {
            stbi_write_tga(fileName.c_str(), texture->getWidth(), texture->getHeight(), texture->getChannels(), texture->getData());
        }
        else if (format == "bmp")
        {
            stbi_write_bmp(fileName.c_str(), texture->getWidth(), texture->getHeight(), texture->getChannels(), texture->getData());
        }
        else
        {
            std::cout << "Failed to save texture, invalid format" << std::endl;
        }
    }

    kTextureCube *kAssetManager::loadTextureCube(const std::string fileNameRight, const std::string fileNameLeft, const std::string fileNameTop, const std::string fileNameBottom, const std::string fileNameFront, const std::string fileNameBack, const std::string textureName)
    {
        std::vector<std::string> faces;
        faces.push_back(fileNameRight);
        faces.push_back(fileNameLeft);
        faces.push_back(fileNameTop);
        faces.push_back(fileNameBottom);
        faces.push_back(fileNameFront);
        faces.push_back(fileNameBack);

        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, nrChannels;
        for (unsigned int i = 0; i < faces.size(); i++)
        {
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                // glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        kTextureCube *newTexture = new kTextureCube();
        newTexture->setType(kTextureType::TEX_TYPE_CUBE);
        newTexture->setTextureID(textureID);
        newTexture->setTextureName(textureName);

        return newTexture;
    }

    kMesh *kAssetManager::loadMesh(const std::string fileName)
    {
        std::cout << "Load mesh: " << fileName << std::endl;

        std::string ext = getFileExtension(fileName);

        kMesh *mesh;

        if (ext == "obj" || ext == "fbx" || ext == "gltf" || ext == "glb")
        {
            mesh = loadMeshFileAssimp(fileName);
            mesh->setLoaded(true);
        }
        else
        {
            std::cout << "Failed to load mesh, invalid extension" << std::endl;
            mesh = nullptr;
        }

        mesh->setFileName(fileName);

        return mesh;
    }

    kMesh *kAssetManager::loadMeshFileAssimp(const std::string fileName)
    {
        // kMesh* rootMesh = new kMesh();
        kMesh *rootMesh;

        unsigned int assimpReadFlag = aiProcess_Triangulate |
                                      aiProcess_FlipUVs |
                                      aiProcess_GenSmoothNormals |
                                      aiProcess_CalcTangentSpace |
                                      aiProcess_JoinIdenticalVertices |
                                      aiProcess_LimitBoneWeights |
                                      aiProcess_ImproveCacheLocality |
                                      aiProcess_RemoveRedundantMaterials |
                                      aiProcess_FixInfacingNormals |
                                      aiProcess_TransformUVCoords |
                                      aiProcess_SortByPType;

        Assimp::Importer import;
        import.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

        const aiScene *scene = import.ReadFile(fileName, assimpReadFlag);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
            return nullptr;
        }
        std::string directory;
        directory = fileName.substr(0, fileName.find_last_of('/'));

        // kMesh* model = processNode(scene->mRootNode, scene, rootMesh);
        rootMesh = processNode(scene->mRootNode, scene, nullptr);

        /*std::cout << "Meshes: " << scene->mNumMeshes << std::endl;
        std::cout << "Materials: " << scene->mNumMaterials << std::endl;
        std::cout << "Textures: " << scene->mNumTextures << std::endl;
        std::cout << "Skeletons: " << scene->mNumSkeletons << std::endl;
        std::cout << "Animations:" << scene->mNumAnimations << std::endl;
        std::cout << "Cameras: " << scene->mNumCameras << std::endl;
        std::cout << "Lights: " << scene->mNumLights << std::endl;

        if (scene->mNumMeshes > 0)
        {
            for (int i = 0; i < scene->mNumMeshes; ++i)
            {
                std::cout << "Mesh: " << scene->mMeshes[i]->mName.C_Str() << std::endl;
                std::cout << "-- Anim Meshes: " << scene->mMeshes[i]->mNumAnimMeshes << std::endl;
                std::cout << "-- Bones: " << scene->mMeshes[i]->mNumBones << std::endl;
                std::cout << "-- Faces: " << scene->mMeshes[i]->mNumFaces << std::endl;
                std::cout << "-- UV Components: " << scene->mMeshes[i]->mNumUVComponents << std::endl;
                std::cout << "-- Vertices: " << scene->mMeshes[i]->mNumVertices << std::endl;
            }
        }

        if (scene->mNumMaterials > 0)
        {
            for (int i = 0; i < scene->mNumMaterials; ++i)
            {
                std::cout << "Material: " << scene->mMaterials[i]->GetName().C_Str() << std::endl;

                if (scene->mMaterials[i]->mNumProperties > 0)
                {
                    for (int j = 0; j < scene->mMaterials[i]->mNumProperties; ++j)
                    {
                        std::string typeName = "";
                        aiPropertyTypeInfo type = scene->mMaterials[i]->mProperties[j]->mType;

                        if (type == aiPropertyTypeInfo::aiPTI_Buffer)
                            typeName = "buffer";
                        else if (type == aiPropertyTypeInfo::aiPTI_Double)
                            typeName = "double";
                        else if (type == aiPropertyTypeInfo::aiPTI_Float)
                            typeName = "float";
                        else if (type == aiPropertyTypeInfo::aiPTI_Integer)
                            typeName = "integer";
                        else if (type == aiPropertyTypeInfo::aiPTI_String)
                            typeName = "string";
                        else if (type == aiPropertyTypeInfo::_aiPTI_Force32Bit)
                            typeName = "force 32 bit";

                        std::cout << "-- Property: " << scene->mMaterials[i]->mProperties[j]->mKey.C_Str() << " : " << scene->mMaterials[i]->mProperties[j]->mData << " (" << typeName << ")" << std::endl;
                    }
                }
            }
        }*/

        /*if (scene->mNumTextures > 0)
        {
            for (int i = 0; i < scene->mNumTextures; ++i)
            {
                std::cout << "Texture: " << getBaseFilename(scene->mTextures[i]->mFilename.C_Str()) << std::endl;

                kTexture2D* texture = loadTextureFromMemory(scene->mTextures[i], getBaseFilename(scene->mTextures[i]->mFilename.C_Str()));

                //saveTexture(texture, getBaseFilename(scene->mTextures[i]->mFilename.C_Str()), getFileExtension(getBaseFilename(scene->mTextures[i]->mFilename.C_Str())));
            }
        }*/

        // return model;
        return rootMesh;
    }

    kMesh *kAssetManager::processNode(aiNode *node, const aiScene *scene, kMesh *parent)
    {
        kMesh *newMesh;

        // std::cout << "Mesh count:" << node->mNumMeshes << std::endl;

        if (node->mNumMeshes > 0)
        {
            // process all the node's meshes (if any)
            for (size_t i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

                newMesh = processMesh(mesh, scene);
                if (newMesh != nullptr && parent != nullptr)
                {
                    newMesh->setParent(parent);
                }
            }
        }
        else
        {
            newMesh = new kMesh(parent);
        }

        if (node->mNumChildren > 0)
        {
            // then do the same for each of its children
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                if (node->mChildren[i] != nullptr && scene != nullptr && newMesh != nullptr)
                {
                    processNode(node->mChildren[i], scene, newMesh);
                }
            }
        }

        return newMesh;
    }

    kMesh *kAssetManager::processMesh(aiMesh *mesh, const aiScene *scene)
    {
        kMesh *newMesh = new kMesh();
        newMesh->setName(std::string(mesh->mName.C_Str()));

        if (mesh->mNumVertices > 0)
        {
            // Reserve space for bone data (bone IDs and weights)
            newMesh->reserveBoneData(mesh->mNumVertices);

            // Reserve space for vertices, normals, UVs, bone IDs, and weights
            newMesh->reserveSpace(mesh->mNumVertices);

            for (int i = 0; i < (int)mesh->mNumVertices; ++i)
            {
                // Vertex
                glm::vec3 position = kAssimpGLMHelpers::getGLMVec3(mesh->mVertices[i]);
                newMesh->addVertex(position);

                // Normal
                glm::vec3 normal = kAssimpGLMHelpers::getGLMVec3(mesh->mNormals[i]);
                newMesh->addNormal(normal);

                // UV
                if (mesh->HasTextureCoords(0))
                {
                    glm::vec2 texCoord = kAssimpGLMHelpers::getGLMVec2(mesh->mTextureCoords[0][i]);
                    newMesh->addUV(texCoord);
                }
                else
                {
                    newMesh->addUV(glm::vec2(0.0f, 0.0f));
                }

                // Tangent and Bitangent
                glm::vec3 tangent = glm::vec3(0.0f);
                glm::vec3 bitangent = glm::vec3(0.0f);
                if (mesh->HasTangentsAndBitangents())
                {
                    tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
                    bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
                }
                newMesh->addTangent(tangent);
                newMesh->addBitangent(bitangent);

                // Initialize bone IDs and weights
                newMesh->addBoneID(glm::ivec4(-1, -1, -1, -1));
                newMesh->addWeight(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
            }

            // Debug: Print bone data after initialization
            // std::cout << "Bone data after initialization:" << std::endl;
            /*
            for (int i = 0; i < newMesh->getVertexCount(); ++i)
            {
                auto boneID = newMesh->getBoneID(i);
                auto weight = newMesh->getWeight(i);
                //std::cout << "Vertex " << i << ": BoneID = " << glm::to_string(boneID) << ", Weight = " << glm::to_string(weight) << std::endl;
            }
            */

            // Extract bone weights for vertices
            extractBoneWeightForVertices(newMesh, mesh, scene);

            // Debug: Print bone data after extraction
            // std::cout << "Bone data after extraction:" << std::endl;
            /*
            for (int i = 0; i < newMesh->getVertexCount(); ++i)
            {
                auto boneID = newMesh->getBoneID(i);
                auto weight = newMesh->getWeight(i);
                //std::cout << "Vertex " << i << ": BoneID = " << glm::to_string(boneID) << ", Weight = " << glm::to_string(weight) << std::endl;
            }
            */
        }

        if (mesh->mNumFaces > 0)
        {
            for (int i = 0; i < (int)mesh->mNumFaces; ++i)
            {
                // Indices
                if (mesh->mFaces[i].mNumIndices > 0)
                {
                    for (int j = 0; j < (int)mesh->mFaces[i].mNumIndices; ++j)
                    {
                        newMesh->addIndex((uint32_t)mesh->mFaces[i].mIndices[j]);
                    }
                }
            }
        }

        newMesh->generateVbo();
        newMesh->setLoaded(true);

        return newMesh;
    }

    void kAssetManager::calculateNormal(float N[3], float v0[3], float v1[3], float v2[3])
    {
        float v10[3];
        v10[0] = v1[0] - v0[0];
        v10[1] = v1[1] - v0[1];
        v10[2] = v1[2] - v0[2];

        float v20[3];
        v20[0] = v2[0] - v0[0];
        v20[1] = v2[1] - v0[1];
        v20[2] = v2[2] - v0[2];

        N[0] = v10[1] * v20[2] - v10[2] * v20[1];
        N[1] = v10[2] * v20[0] - v10[0] * v20[2];
        N[2] = v10[0] * v20[1] - v10[1] * v20[0];

        float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
        if (len2 > 0.0f)
        {
            float len = sqrtf(len2);

            N[0] /= len;
            N[1] /= len;
            N[2] /= len;
        }
    }

    void kAssetManager::normalizeVector(glm::vec3 &v)
    {
        float len2 = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
        if (len2 > 0.0f)
        {
            float len = sqrtf(len2);

            v[0] /= len;
            v[1] /= len;
            v[2] /= len;
        }
    }

    void kAssetManager::extractBoneWeightForVertices(kMesh *mesh, aiMesh *meshData, const aiScene *scene)
    {
        if (meshData->mNumBones > 0)
        {
            for (unsigned int boneIndex = 0; boneIndex < meshData->mNumBones; ++boneIndex)
            {
                int boneID = -1;
                std::string boneName = meshData->mBones[boneIndex]->mName.C_Str();

                // Debug: Print bone name and offset matrix
                // std::cout << "Processing bone: " << boneName << std::endl;
                glm::mat4 offset = kAssimpGLMHelpers::convertMatrixToGLMFormat(meshData->mBones[boneIndex]->mOffsetMatrix);
                // std::cout << "Offset matrix: " << glm::to_string(offset) << std::endl;

                std::map<std::string, kBoneInfo> &boneInfoMap = mesh->getBoneInfoMap();

                // Check if the bone already exists in the boneInfoMap
                if (boneInfoMap.find(boneName) == boneInfoMap.end())
                {
                    // std::cout << "not found" << std::endl;

                    // Add new bone to the boneInfoMap
                    kBoneInfo newBoneInfo;
                    newBoneInfo.id = mesh->getBoneCount();
                    newBoneInfo.offset = offset;

                    boneInfoMap.insert(std::make_pair(boneName, newBoneInfo));

                    boneID = mesh->getBoneCount();
                    mesh->setBoneCount(mesh->getBoneCount() + 1);

                    // std::cout << mesh->getBoneCount() << std::endl;

                    // Debug: Print new bone info
                    // std::cout << "New bone added 1: " << boneName << ", ID: " << newBoneInfo.id << ", offset: " << glm::to_string(newBoneInfo.offset) << std::endl;
                    // std::cout << "New bone added 2: " << boneName << ", ID: " << mesh->getBoneInfoMap()[boneName].id << ", offset: " << glm::to_string(mesh->getBoneInfoMap()[boneName].offset) << std::endl;
                }
                else
                {
                    // std::cout << "found" << std::endl;

                    kBoneInfo &boneInfo = boneInfoMap[boneName];

                    // Use existing bone ID
                    boneID = boneInfo.id;

                    // Replace
                    kBoneInfo newBoneInfo;
                    newBoneInfo.id = boneID;
                    newBoneInfo.offset = offset;

                    // std::cout << "Replace: " << glm::to_string(*newBoneInfo.offset) << std::endl;

                    boneInfoMap[boneName] = newBoneInfo;
                }

                assert(boneID != -1);

                // Assign bone weights to vertices
                auto weights = meshData->mBones[boneIndex]->mWeights;
                int numWeights = meshData->mBones[boneIndex]->mNumWeights;

                for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
                {
                    size_t vertexID = weights[weightIndex].mVertexId;
                    float weight = weights[weightIndex].mWeight;

                    // Debug: Print vertex bone data
                    // std::cout << "Vertex " << vertexID << ": BoneID = " << boneID << ", Weight = " << weight << std::endl;

                    // Assign bone ID and weight to the vertex
                    mesh->setVertexBoneData(vertexID, boneID, weight);
                }
            }

            // After loading bones (in extractBoneWeightForVertices)
            /*
            std::cout << "Bone Info Map after loading:" << std::endl;
            for (const auto& pair : mesh->getBoneInfoMap())
            {
                std::cout << "Bone " << pair.first << " Offset: " << glm::to_string(pair.second.offset) << std::endl;
            }
            */
        }
        else
        {
            // std::cout << "No bones found in mesh: " << mesh->getName() << std::endl;
        }
    }

    kShader *kAssetManager::createShaderByFile(std::string vertexShaderPath, std::string fragmentShaderPath)
    {
        kShader *shader = new kShader();
        shader->loadShadersFile(vertexShaderPath, fragmentShaderPath);

        shaders.push_back(shader);

        return shader;
    }

    kShader *kAssetManager::createShaderByCode(std::string vertexShaderCode, std::string fragmentShaderCode)
    {
        kShader *shader = new kShader();
        shader->loadShadersCode(vertexShaderCode.c_str(), fragmentShaderCode.c_str());

        shaders.push_back(shader);

        return shader;
    }

    kMaterial *kAssetManager::createMaterial(kShader *shader)
    {
        kMaterial *material = new kMaterial();
        material->setShader(shader);

        materials.push_back(material);

        return material;
    }

    kAnimation *kAssetManager::loadAnimation(const std::string fileName, kMesh *mesh)
    {
        kAnimation *animation = new kAnimation(fileName, mesh);

        return animation;
    }
}
