#include "klight.h"

namespace kemena
{
    kLight::kLight(kObject *parentNode)
    {
        if (parentNode != nullptr)
            setParent(parentNode);
        setType(kNodeType::NODE_TYPE_LIGHT);
    }

    glm::vec3 kLight::getAmbientColor()
    {
        return ambientColor;
    }

    void kLight::setAmbientColor(glm::vec3 newColor)
    {
        ambientColor = newColor;
    }

    glm::vec3 kLight::getDiffuseColor()
    {
        return diffuseColor;
    }

    void kLight::setDiffuseColor(glm::vec3 newColor)
    {
        diffuseColor = newColor;
    }

    glm::vec3 kLight::getSpecularColor()
    {
        return specularColor;
    }

    void kLight::setSpecularColor(glm::vec3 newColor)
    {
        specularColor = newColor;
    }

    float kLight::getPower()
    {
        return power;
    }

    void kLight::setPower(float newValue)
    {
        power = newValue;
    }

    float kLight::getConstant()
    {
        return constant;
    }

    float kLight::getLinear()
    {
        return linear;
    }

    float kLight::getQuadratic()
    {
        return quadratic;
    }

    void kLight::setConstant(float newValue)
    {
        constant = newValue;
    }

    void kLight::setLinear(float newValue)
    {
        linear = newValue;
    }

    void kLight::setQuadratic(float newValue)
    {
        quadratic = newValue;
    }

    glm::vec3 kLight::getDirection()
    {
        return direction;
    }

    void kLight::setDirection(glm::vec3 newValue)
    {
        direction = newValue;
    }

    float kLight::getCutOff()
    {
        return cutOff;
    }

    void kLight::setCutOff(float newValue)
    {
        cutOff = glm::cos(glm::radians(newValue));
    }

    float kLight::getOuterCutOff()
    {
        return outerCutOff;
    }

    void kLight::setOuterCutOff(float newValue)
    {
        outerCutOff = glm::cos(glm::radians(newValue));
    }

    kLightType kLight::getLightType()
    {
        return lightType;
    }

    void kLight::setLightType(kLightType newType)
    {
        lightType = newType;
    }
}
