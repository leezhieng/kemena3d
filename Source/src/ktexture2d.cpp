#include "ktexture2d.h"

namespace kemena
{
    kTexture2D::kTexture2D()
    {
    }

    kTexture2D::~kTexture2D()
    {
    }

    void kTexture2D::setWidth(int newWidth)
    {
        width = newWidth;
    }

    int kTexture2D::getWidth()
    {
        return width;
    }

    void kTexture2D::setHeight(int newHeight)
    {
        height = newHeight;
    }

    int kTexture2D::getHeight()
    {
        return height;
    }

    void kTexture2D::setChannels(int newChannel)
    {
        channels = newChannel;
    }

    int kTexture2D::getChannels()
    {
        return channels;
    }

    void kTexture2D::setData(unsigned char *newData)
    {
        data = newData;
    }

    unsigned char *kTexture2D::getData()
    {
        return data;
    }
}
