#ifndef KLIGHT_H
#define KLIGHT_H

#include "kexport.h"

#include "kobject.h"

namespace kemena
{
    class KEMENA3D_API kLight : public kObject
    {
        public:
            kLight(kObject* parentNode = nullptr);

            glm::vec3 getAmbientColor();
            void setAmbientColor(glm::vec3 newColor);
            glm::vec3 getDiffuseColor();
            void setDiffuseColor(glm::vec3 newColor);
            glm::vec3 getSpecularColor();
            void setSpecularColor(glm::vec3 newColor);

            float getPower();
            void setPower(float newValue);

            float getConstant();
            float getLinear();
            float getQuadratic();

            void setConstant(float newValue);
            void setLinear(float newValue);
            void setQuadratic(float newValue);

            glm::vec3 getDirection();
            void setDirection(glm::vec3 newValue);

            float getCutOff();
            void setCutOff(float newValue);
            float getOuterCutOff();
            void setOuterCutOff(float newValue);

            kLightType getLightType();
            void setLightType(kLightType newType);

        protected:

        private:
            glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
            glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);

            float power = 10.0f;

            // Point light
            float constant = 1.0f;
            float linear = 0.7f;
            float quadratic = 1.8f;

            // Sun light
            glm::vec3 direction = glm::vec3(1.0f, 1.0f, 1.0f);

            // Spotlight
            float cutOff = glm::cos(glm::radians(15.0f));
            float outerCutOff = glm::cos(glm::radians(20.0f));

            kLightType lightType = kLightType::LIGHT_TYPE_POINT;
    };
}

#endif // KLIGHT_H
