#include "klight.h"

namespace kemena
{
    kLight::kLight(kObject *parentNode)
    {
        if (parentNode != nullptr)
            setParent(parentNode);
        setType(kNodeType::NODE_TYPE_LIGHT);
    }

    kVec3 kLight::getDiffuseColor()
    {
        return diffuseColor;
    }

    void kLight::setDiffuseColor(kVec3 newColor)
    {
        diffuseColor = newColor;
    }

    kVec3 kLight::getSpecularColor()
    {
        return specularColor;
    }

    void kLight::setSpecularColor(kVec3 newColor)
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

    kVec3 kLight::getDirection()
    {
        return direction;
    }

    void kLight::setDirection(kVec3 newValue)
    {
        direction = newValue;
    }

    float kLight::getCutOff()
    {
        return cutOff;
    }

    void kLight::setCutOff(float newValue)
    {
        cutOff = newValue;
    }

    float kLight::getOuterCutOff()
    {
        return outerCutOff;
    }

    void kLight::setOuterCutOff(float newValue)
    {
        outerCutOff = newValue;
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
