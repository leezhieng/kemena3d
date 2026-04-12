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

    kTexture* kTexture::duplicate(string newName)
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

    void kTexture::setTextureName(string newName)
    {
        textureName = newName;
    }

    string kTexture::getTextureName()
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
