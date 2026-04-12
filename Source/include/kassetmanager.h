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

        string getFileExtension(const string &fileName);
        bool fileExists(const string &fileName);
        string getBaseDir(const string &filePath);
        string getBaseFilename(const string &filePath);
        string getExecDir();
        string popDir(const string &filePath);

		unsigned char* loadImageFromResource(const char* resourceName, int& width, int& height, int& channels);

        kTexture2D *loadTexture2D(const string fileName, const string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);
        kTexture2D *loadTexture2DFromMemory(const aiTexture *rawData, const string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);
        kTexture2D *loadTexture2DFromResource(const string resourceName, const string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);
		void saveTexture2D(kTexture2D *texture, const string fileName, string format);

        kTextureCube *loadTextureCube(const string fileNameRight, const string fileNameLeft, const string fileNameTop, const string fileNameBottom, const string fileNameFront, const string fileNameBack, const string textureName);
		kTextureCube *loadTextureCubeFromResource(const string resRight, const string resLeft, const string resTop, const string resBottom, const string resFront, const string resBack, const string textureName);

        kMesh *loadMesh(const string fileName);
		kMesh *loadMeshFromResource(const string resourceName, const string extention);
        kMesh *loadMeshFileAssimp(const string fileName);
		kMesh *loadMeshResourceAssimp(const string resourceName, const string extention);
        kMesh *processNode(aiNode *node, const aiScene *scene, kMesh *parent);
        kMesh *processMesh(aiMesh *mesh, const aiScene *scene);

        void calculateNormal(float N[3], float v0[3], float v1[3], float v2[3]);
        void normalizeVector(vec3 &v);

        void extractBoneWeightForVertices(kMesh *mesh, aiMesh *meshData, const aiScene *scene);
        void setVertexBoneData(kMesh *mesh, size_t vertexID, int boneID, float weight);

        kShader *loadShaderFromFile(string vertexShaderPath, string fragmentShaderPath);
        kShader *loadShaderFromCode(string vertexShaderCode, string fragmentShaderCode);
        kShader *loadShaderFromResource(string vertexShaderName, string fragmentShaderName);
		
        kMaterial *createMaterial(kShader *shader);

        kAnimation *loadAnimation(const string fileName, kMesh *mesh);

        // GUI

    protected:
    private:
        std::vector<kMesh *> meshes;
        std::vector<kShader *> shaders;
        std::vector<kMaterial *> materials;
    };
}

#endif // KASSETMANAGER_H
