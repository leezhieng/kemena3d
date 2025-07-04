#include "kfont.h"

namespace kemena
{
    kFont::kFont()
    {
        //ctor
    }

    kFont::~kFont()
    {
        //dtor
    }

    void kFont::setFilePath(string newPath)
    {
        filePath = newPath;
    }

    string kFont::getFilePath()
    {
        return filePath;
    }

    void kFont::setTtfBuffer(unsigned char* newBuffer)
    {
        ttfBuffer = newBuffer;
    }

    unsigned char* kFont::getTtfBuffer()
    {
        return ttfBuffer;
    }

    void kFont::setBitmapWidth(int newWidth)
    {
        bitmapWidth = newWidth;
    }

    int kFont::getBitmapWidth()
    {
        return bitmapWidth;
    }

    void kFont::setBitmapHeight(int newHeight)
    {
        bitmapHeight = newHeight;
    }

    int kFont::getBitmapHeight()
    {
        return bitmapHeight;
    }

    void kFont::setFontGlyph(std::vector<kFontGlyph> newGlyph)
    {
        glyphs = newGlyph;
    }

    std::vector<kFontGlyph> kFont::getFontGlyph()
    {
        return glyphs;
    }

    void kFont::setFontSize(float newSize)
    {
        fontSize = newSize;
    }

    float kFont::getFontSize()
    {
        return fontSize;
    }
}
