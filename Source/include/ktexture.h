#ifndef KTEXTURE_H
#define KTEXTURE_H

#include "kexport.h"

#include <iostream>
#include <string>
#include <vector>

#include "glm/glm.hpp"
#include <GL/glew.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "kdatatype.h"

namespace kemena
{
    class KEMENA3D_API kTexture
    {
    public:
        kTexture();
        virtual ~kTexture();

        kTexture *duplicate(string newName);

        void setTextureID(GLuint newTextureID);
        GLuint getTextureID();

        void setTextureName(string newName);
        string getTextureName();

        void setType(kTextureType newType);
        kTextureType getType();

    protected:
    private:
        GLuint textureID;
        string textureName;
        kTextureType type = kTextureType::TEX_TYPE_2D;
    };
}

#endif // KTEXTURE_H
