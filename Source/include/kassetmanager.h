/**
 * @file kassetmanager.h
 * @brief Loads and manages textures, meshes, shaders, materials, and animations.
 */

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
    /**
     * @brief Central asset-loading and cache manager.
     *
     * Provides helpers for common filesystem operations and factory methods
     * for textures (2D and cube-map), meshes (via Assimp), shader programs,
     * materials, and skeletal animations.  All loaded assets are tracked
     * internally; call the appropriate delete methods when assets are no
     * longer needed.
     */
    class kAssetManager
    {
    public:
        kAssetManager();

        /**
         * @brief Extracts the file extension from a path string.
         * @param fileName File path or name.
         * @return Lower-case extension without the leading dot.
         */
        string getFileExtension(const string &fileName);

        /**
         * @brief Checks whether a file exists on disk.
         * @param fileName Absolute or relative file path.
         * @return true if the file is accessible.
         */
        bool fileExists(const string &fileName);

        /**
         * @brief Returns the directory portion of a file path.
         * @param filePath Full file path.
         * @return Directory path with trailing separator.
         */
        string getBaseDir(const string &filePath);

        /**
         * @brief Returns the filename without directory or extension.
         * @param filePath Full file path.
         * @return Bare filename stem.
         */
        string getBaseFilename(const string &filePath);

        /**
         * @brief Returns the directory containing the running executable.
         * @return Executable directory with trailing separator.
         */
        string getExecDir();

        /**
         * @brief Removes the last directory component from a path.
         * @param filePath Input path.
         * @return Path with the last component removed.
         */
        string popDir(const string &filePath);

        /**
         * @brief Loads an image from a Windows embedded resource into CPU memory.
         * @param resourceName Resource identifier string.
         * @param width        Receives the image width in pixels.
         * @param height       Receives the image height in pixels.
         * @param channels     Receives the number of colour channels.
         * @return Heap-allocated pixel data; caller must free with stbi_image_free.
         */
        unsigned char *loadImageFromResource(const char *resourceName, int &width, int &height, int &channels);

        /**
         * @brief Loads a 2D texture from a file on disk and uploads it to the GPU.
         * @param fileName      Path to the image file.
         * @param textureName   GLSL sampler uniform name.
         * @param format        Desired GPU format (default SRGBA).
         * @param flipVertical  Flip the image vertically before upload.
         * @param keepData      Retain CPU-side pixel data after upload.
         * @return Heap-allocated kTexture2D; caller takes ownership.
         */
        kTexture2D *loadTexture2D(const string fileName, const string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);

        /**
         * @brief Loads a 2D texture from an in-memory Assimp texture blob.
         * @param rawData      Assimp embedded texture descriptor.
         * @param textureName  GLSL sampler uniform name.
         * @param format       Desired GPU format.
         * @param flipVertical Flip vertically before upload.
         * @param keepData     Retain CPU-side pixel data.
         * @return Heap-allocated kTexture2D; caller takes ownership.
         */
        kTexture2D *loadTexture2DFromMemory(const aiTexture *rawData, const string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);

        /**
         * @brief Loads a 2D texture from a Windows embedded resource.
         * @param resourceName Resource identifier string.
         * @param textureName  GLSL sampler uniform name.
         * @param format       Desired GPU format.
         * @param flipVertical Flip vertically before upload.
         * @param keepData     Retain CPU-side pixel data.
         * @return Heap-allocated kTexture2D; caller takes ownership.
         */
        kTexture2D *loadTexture2DFromResource(const string resourceName, const string textureName, const kTextureFormat format = kTextureFormat::TEX_FORMAT_SRGBA, const bool flipVertical = false, const bool keepData = false);

        /**
         * @brief Saves a kTexture2D to disk using the specified image format.
         * @param texture  Source texture with CPU-side pixel data retained.
         * @param fileName Output file path.
         * @param format   Output format string (e.g. "png", "jpg").
         */
        void saveTexture2D(kTexture2D *texture, const string fileName, string format);

        /**
         * @brief Loads a cube-map texture from six individual face files.
         * @param fileNameRight  +X face image path.
         * @param fileNameLeft   -X face image path.
         * @param fileNameTop    +Y face image path.
         * @param fileNameBottom -Y face image path.
         * @param fileNameFront  +Z face image path.
         * @param fileNameBack   -Z face image path.
         * @param textureName    GLSL samplerCube uniform name.
         * @return Heap-allocated kTextureCube; caller takes ownership.
         */
        kTextureCube *loadTextureCube(const string fileNameRight, const string fileNameLeft, const string fileNameTop, const string fileNameBottom, const string fileNameFront, const string fileNameBack, const string textureName);

        /**
         * @brief Loads a cube-map texture from six Windows embedded resources.
         * @param resRight   +X face resource name.
         * @param resLeft    -X face resource name.
         * @param resTop     +Y face resource name.
         * @param resBottom  -Y face resource name.
         * @param resFront   +Z face resource name.
         * @param resBack    -Z face resource name.
         * @param textureName GLSL samplerCube uniform name.
         * @return Heap-allocated kTextureCube; caller takes ownership.
         */
        kTextureCube *loadTextureCubeFromResource(const string resRight, const string resLeft, const string resTop, const string resBottom, const string resFront, const string resBack, const string textureName);

        /**
         * @brief Loads a mesh hierarchy from a file, using Assimp for 3D formats.
         * @param fileName Path to the mesh file.
         * @return Root kMesh of the loaded hierarchy; caller takes ownership.
         */
        kMesh *loadMesh(const string fileName);

        /**
         * @brief Loads a mesh from a Windows embedded resource.
         * @param resourceName Resource identifier string.
         * @param extention    File extension hint for Assimp (e.g. ".fbx").
         * @return Root kMesh; caller takes ownership.
         */
        kMesh *loadMeshFromResource(const string resourceName, const string extention);

        /**
         * @brief Loads a mesh file using the Assimp importer.
         * @param fileName Path to the asset file.
         * @return Root kMesh; caller takes ownership.
         */
        kMesh *loadMeshFileAssimp(const string fileName);

        /**
         * @brief Loads a mesh from an embedded resource using the Assimp importer.
         * @param resourceName Resource identifier string.
         * @param extention    File extension hint.
         * @return Root kMesh; caller takes ownership.
         */
        kMesh *loadMeshResourceAssimp(const string resourceName, const string extention);

        /**
         * @brief Recursively converts an Assimp node into a kMesh hierarchy.
         * @param node   Assimp node to process.
         * @param scene  Parent Assimp scene.
         * @param parent Parent kMesh node (nullptr for root).
         * @return kMesh node corresponding to @p node.
         */
        kMesh *processNode(aiNode *node, const aiScene *scene, kMesh *parent);

        /**
         * @brief Converts a single Assimp mesh into a kMesh.
         * @param mesh  Assimp mesh descriptor.
         * @param scene Parent Assimp scene (for texture lookups).
         * @return Heap-allocated kMesh; caller takes ownership.
         */
        kMesh *processMesh(aiMesh *mesh, const aiScene *scene);

        /**
         * @brief Computes the face normal for a triangle.
         * @param N  Output: normalised face normal.
         * @param v0 First vertex position.
         * @param v1 Second vertex position.
         * @param v2 Third vertex position.
         */
        void calculateNormal(float N[3], float v0[3], float v1[3], float v2[3]);

        /**
         * @brief Normalises a vec3 in-place.
         * @param v Vector to normalise.
         */
        void normalizeVector(vec3 &v);

        /**
         * @brief Reads bone-weight data from an Assimp mesh and populates a kMesh.
         * @param mesh     Destination kMesh.
         * @param meshData Source Assimp mesh.
         * @param scene    Parent Assimp scene.
         */
        void extractBoneWeightForVertices(kMesh *mesh, aiMesh *meshData, const aiScene *scene);

        /**
         * @brief Assigns one bone influence slot to a vertex (helper used during loading).
         * @param mesh     Target kMesh.
         * @param vertexID Zero-based vertex index.
         * @param boneID   Bone palette index.
         * @param weight   Blend weight.
         */
        void setVertexBoneData(kMesh *mesh, size_t vertexID, int boneID, float weight);

        /**
         * @brief Compiles a shader program from GLSL source files on disk.
         * @param vertexShaderPath   Path to the vertex shader source.
         * @param fragmentShaderPath Path to the fragment shader source.
         * @return Heap-allocated kShader; caller takes ownership.
         */
        kShader *loadShaderFromFile(string vertexShaderPath, string fragmentShaderPath);

        /**
         * @brief Compiles a shader program from inline GLSL source strings.
         * @param vertexShaderCode   Vertex shader GLSL source.
         * @param fragmentShaderCode Fragment shader GLSL source.
         * @return Heap-allocated kShader; caller takes ownership.
         */
        kShader *loadShaderFromCode(string vertexShaderCode, string fragmentShaderCode);

        /**
         * @brief Compiles a shader program from Windows embedded resources.
         * @param vertexShaderName   Resource name for the vertex shader.
         * @param fragmentShaderName Resource name for the fragment shader.
         * @return Heap-allocated kShader; caller takes ownership.
         */
        kShader *loadShaderFromResource(string vertexShaderName, string fragmentShaderName);

        /**
         * @brief Creates a new material with a pre-assigned shader.
         * @param shader Compiled shader to attach to the material.
         * @return Heap-allocated kMaterial; caller takes ownership.
         */
        kMaterial *createMaterial(kShader *shader);

        /**
         * @brief Loads a skeletal animation from a file for a given mesh.
         * @param fileName Path to the animation file.
         * @param mesh     Mesh whose bone map is used to bind the animation.
         * @return Heap-allocated kAnimation; caller takes ownership.
         */
        kAnimation *loadAnimation(const string fileName, kMesh *mesh);

    protected:
    private:
        std::vector<kMesh *>     meshes;    ///< Tracked loaded meshes.
        std::vector<kShader *>   shaders;   ///< Tracked compiled shaders.
        std::vector<kMaterial *> materials; ///< Tracked created materials.
    };
}

#endif // KASSETMANAGER_H
