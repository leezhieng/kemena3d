#include "ktexture.h"

namespace kemena
{
    kTexture::kTexture()
    {
        //ctor
    }

    kTexture::~kTexture()
    {
        //dtor
    }

    kTexture* kTexture::duplicate(std::string newName)
    {
        // New memory, same value
        kTexture* newTexture = new kTexture(*this);
        newTexture->setTextureName(newName);

        return newTexture;
    }

    void kTexture::setTextureID(GLuint newTextureID)
    {
        textureID = newTextureID;
    }

    GLuint kTexture::getTextureID()
    {
        return textureID;
    }

    void kTexture::setTextureName(std::string newName)
    {
        textureName = newName;
    }

    std::string kTexture::getTextureName()
    {
        return textureName;
    }

    void kTexture::setType(kTextureType newType)
    {
        type = newType;
    }

    kTextureType kTexture::getType()
    {
        return type;
    }
}
