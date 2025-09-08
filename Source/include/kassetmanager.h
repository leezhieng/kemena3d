#ifndef KASSETMANAGER_H
#define KASSETMANAGER_H

#include <SDL3/SDL_filesystem.h>

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "kdatatype.h"
#include "kobject.h"
#include "kmesh.h"
#include "kshader.h"
#include "kmaterial.h"
#include "ktexture2d.h"
#include "ktexturecube.h"
#include "kbone.h"
#include "kanimation.h"
#include "kanimator.h"

// Windows only
#include <windows.h>

namespace kemena
{
    class kAssetManager
    {
    public:
        kAssetManager();

        std::string getFileExtension(const std::string &fileName);
        bool fileExists(const std::string &fileName);
        std::string getBaseDir(const std::string &filePath);
        std::string getBaseFilename(const std::string &filePath);
        std::string getExecDir();
        std::string popDir(const std::string &filePath);

		unsigned char* loadImageFromResource(const char* resourceName, int& width, int& height, int& channels);

        kTexture2D *loadTexture2D(const std::string fileName, const std::string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);
        kTexture2D *loadTexture2DFromMemory(const aiTexture *rawData, const std::string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);
        kTexture2D *loadTexture2DFromResource(const std::string resourceName, const std::string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);
		void saveTexture2D(kTexture2D *texture, const std::string fileName, std::string format);

        kTextureCube *loadTextureCube(const std::string fileNameRight, const std::string fileNameLeft, const std::string fileNameTop, const std::string fileNameBottom, const std::string fileNameFront, const std::string fileNameBack, const std::string textureName);
		kTextureCube *loadTextureCubeFromResource(const std::string resRight, const std::string resLeft, const std::string resTop, const std::string resBottom, const std::string resFront, const std::string resBack, const std::string textureName);

        kMesh *loadMesh(const std::string fileName);
		kMesh *loadMeshFromResource(const std::string resourceName, const std::string extention);
        kMesh *loadMeshFileAssimp(const std::string fileName);
		kMesh *loadMeshResourceAssimp(const string resourceName, const std::string extention);
        kMesh *processNode(aiNode *node, const aiScene *scene, kMesh *parent);
        kMesh *processMesh(aiMesh *mesh, const aiScene *scene);

        void calculateNormal(float N[3], float v0[3], float v1[3], float v2[3]);
        void normalizeVector(glm::vec3 &v);

        void extractBoneWeightForVertices(kMesh *mesh, aiMesh *meshData, const aiScene *scene);
        void setVertexBoneData(kMesh *mesh, size_t vertexID, int boneID, float weight);

        kShader *loadShaderFromFile(std::string vertexShaderPath, std::string fragmentShaderPath);
        kShader *loadShaderFromCode(std::string vertexShaderCode, std::string fragmentShaderCode);
        kShader *loadShaderFromResource(std::string vertexShaderName, std::string fragmentShaderName);
		
        kMaterial *createMaterial(kShader *shader);

        kAnimation *loadAnimation(const std::string fileName, kMesh *mesh);

        // GUI

    protected:
    private:
        std::vector<kMesh *> meshes;
        std::vector<kShader *> shaders;
        std::vector<kMaterial *> materials;
    };
}

#endif // KASSETMANAGER_H
