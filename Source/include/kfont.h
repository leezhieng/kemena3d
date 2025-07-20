#ifndef KFONT_H
#define KFONT_H

#include "kexport.h"

#include "kdatatype.h"

namespace kemena
{
    class KEMENA3D_API kFont
    {
        public:
            kFont();
            virtual ~kFont();

            void setFilePath(string newPath);
            string getFilePath();

            void setTtfBuffer(unsigned char* newBuffer);
            unsigned char* getTtfBuffer();

            void setBitmapWidth(int newWidth);
            int getBitmapWidth();

            void setBitmapHeight(int newHeight);
            int getBitmapHeight();

            void setFontGlyph(std::vector<kFontGlyph> newGlyph);
            std::vector<kFontGlyph> getFontGlyph();

            void setFontSize(float newSize);
            float getFontSize();

        protected:

        private:
            string filePath;
            unsigned char* ttfBuffer;
            int bitmapWidth, bitmapHeight;
            std::vector<kFontGlyph> glyphs;
            float fontSize;
    };
}

#endif // KFONT_H
