#include "kmaterial.h"

namespace kemena
{
    kMaterial::kMaterial()
    {
    }

    kMaterial::~kMaterial()
    {
    }

    void kMaterial::setShader(kShader* newShader)
    {
        shader = newShader;
    }

    kShader* kMaterial::getShader()
    {
        return shader;
    }

    void kMaterial::addTexture(kTexture* texture)
    {
        textures.push_back(texture);
    }

    std::vector<kTexture*> kMaterial::getTextures()
    {
        return textures;
    }

    kTexture* kMaterial::getTexture(int index)
    {
        return textures.at(index);
    }

    void kMaterial::setTransparent(kTransparentType type)
    {
        transparent = type;
    }

    kTransparentType kMaterial::getTransparent()
    {
        return transparent;
    }

    void kMaterial::setUvTiling(vec2 newTiling)
    {
        uvTiling = newTiling;
    }

    vec2 kMaterial::getUvTiling()
    {
        return uvTiling;
    }

    void kMaterial::setAmbientColor(glm::vec3 color)
    {
        ambientColor = color;
    }

    void kMaterial::setDiffuseColor(glm::vec3 color)
    {
        diffuseColor = color;
    }

    void kMaterial::setSpecularColor(glm::vec3 color)
    {
        specularColor = color;
    }

    void kMaterial::setShininess(float value)
    {
        shininess = value;
    }

    void kMaterial::setMetallic(float value)
    {
        metallic = value;
    }

    void kMaterial::setRoughness(float value)
    {
        roughness = value;
    }

    glm::vec3 kMaterial::getAmbientColor()
    {
        return ambientColor;
    }

    glm::vec3 kMaterial::getDiffuseColor()
    {
        return diffuseColor;
    }

    glm::vec3 kMaterial::getSpecularColor()
    {
        return specularColor;
    }

    float kMaterial::getShininess()
    {
        return shininess;
    }

    float kMaterial::getMetallic()
    {
        return metallic;
    }

    float kMaterial::getRoughness()
    {
        return roughness;
    }

    void kMaterial::setSingleSided(bool newSingleSided, bool newCullBack)
    {
        isSingleSided = newSingleSided;
        isCullBack = newCullBack;
    }

    bool kMaterial::getSingleSided()
    {
        return isSingleSided;
    }

    bool kMaterial::getCullBack()
    {
        return isCullBack;
    }
}
