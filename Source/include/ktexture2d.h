#ifndef KTEXTURE2D_H
#define KTEXTURE2D_H

#include "kexport.h"

#include "ktexture.h"

namespace kemena
{
    class KEMENA3D_API kTexture2D : public kTexture
    {
    public:
        kTexture2D();
        virtual ~kTexture2D();

        void setWidth(int newWidth);
        int getWidth();
        void setHeight(int newHeight);
        int getHeight();
        void setChannels(int newChannel);
        int getChannels();

        void setData(unsigned char *newData);
        unsigned char *getData();

    protected:
    private:
        int width;
        int height;
        int channels;

        GLuint textureID;
        std::string textureName;

        unsigned char *data;
    };
}

#endif // KTEXTURE2D_H
