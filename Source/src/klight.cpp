#include "klight.h"

namespace kemena
{
    kLight::kLight(kObject *parentNode)
    {
        if (parentNode != nullptr)
            setParent(parentNode);
        setType(kNodeType::NODE_TYPE_LIGHT);
    }

    vec3 kLight::getAmbientColor()
    {
        return ambientColor;
    }

    void kLight::setAmbientColor(vec3 newColor)
    {
        ambientColor = newColor;
    }

    vec3 kLight::getDiffuseColor()
    {
        return diffuseColor;
    }

    void kLight::setDiffuseColor(vec3 newColor)
    {
        diffuseColor = newColor;
    }

    vec3 kLight::getSpecularColor()
    {
        return specularColor;
    }

    void kLight::setSpecularColor(vec3 newColor)
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

    vec3 kLight::getDirection()
    {
        return direction;
    }

    void kLight::setDirection(vec3 newValue)
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
