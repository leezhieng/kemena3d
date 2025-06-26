#ifndef KMATERIAL_H
#define KMATERIAL_H

#include "kdatatype.h"
#include "kshader.h"
#include "ktexture2d.h"
#include "ktexturecube.h"

namespace kemena
{
    class kMaterial
    {
        public:
            kMaterial();
            virtual ~kMaterial();

            void setShader(kShader* newShader);
            kShader* getShader();

            void addTexture(kTexture* texture);
            std::vector<kTexture*> getTextures();
            kTexture* getTexture(int index);

            void setTransparent(kTransparentType type);
            kTransparentType getTransparent();

            void setUvTiling(vec2 newTiling);
            vec2 getUvTiling();

            void setAmbientColor(vec3 color);
            void setDiffuseColor(vec3 color);
            void setSpecularColor(vec3 color);
            void setShininess(float value);
            void setMetallic(float value);
            void setRoughness(float value);

            vec3 getAmbientColor();
            vec3 getDiffuseColor();
            vec3 getSpecularColor();
            float getShininess();
            float getMetallic();
            float getRoughness();

            void setSingleSided(bool newSingleSided, bool newCullBack = true);
            bool getSingleSided();
            bool getCullBack();

        protected:

        private:
            kShader* shader = nullptr;
            kTransparentType transparent = TRANSP_TYPE_NONE;
            std::vector<kTexture*> textures;

            vec2 uvTiling = vec2(1, 1);

            vec3 ambientColor;
            vec3 diffuseColor;
            vec3 specularColor;
            float shininess;
            float metallic;
            float roughness;

            bool isSingleSided = true;
            bool isCullBack = true;
    };
}

#endif // KMATERIAL_H
