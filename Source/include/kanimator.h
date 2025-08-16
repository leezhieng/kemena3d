#ifndef KANIMATOR_H
#define KANIMATOR_H

#include "kdatatype.h"
#include "kanimation.h"
#include "kbone.h"
#include "kmesh.h"

#include <glm/gtx/string_cast.hpp>

namespace kemena
{
    class kAnimation;
    class kMesh;

    class kAnimator
    {
    public:
        kAnimator(kAnimation *newAnimation);

        void addAnimation(kAnimation *newAnimation);

        void updateAnimation(float newDeltaTime, int frameId);

        void playAnimation(kAnimation *animation);
        kAnimation *getCurrentAnimation();

        void calculateBoneTransform(const kAssimpNodeData *node, glm::mat4 parentTransform);
        const std::vector<glm::mat4> getFinalBoneMatrices() const;

        void setCurrentTime(float newTime);

        void setSpeed(float newSpeed);
        float getSpeed();

    protected:
    private:
        std::vector<glm::mat4> finalBoneMatrices;
        kAnimation *currentAnimation = nullptr;
        float currentTime;
        float deltaTime;
        float speed = 1.0f;

        std::vector<kAnimation *> animations;

        int currentFrameId = -1;
    };

}

#endif // KANIMATOR_H
