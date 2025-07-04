#include "kanimator.h"

namespace kemena
{
    kAnimator::kAnimator(kAnimation* newAnimation)
    {
        currentTime = 0.0f;

        if (newAnimation != nullptr)
        {
            currentAnimation = newAnimation;
            animations.push_back(newAnimation);
        }

        finalBoneMatrices.reserve(MAX_BONES);

        for (int i = 0; i < MAX_BONES; i++)
            finalBoneMatrices.push_back(glm::mat4(1.0f));
    }

    void kAnimator::addAnimation(kAnimation* newAnimation)
    {
        animations.push_back(newAnimation);
    }

    void kAnimator::updateAnimation(float newDeltaTime, int frameId)
    {
        //std::cout << "updateAnimation: " << newDeltaTime << std::endl;

        //std::cout << "Current Time: " << currentTime << ", Duration: " << currentAnimation->getDuration() << std::endl;

        deltaTime = newDeltaTime;
        if (currentAnimation != nullptr && currentFrameId != frameId)
        {
            currentTime += currentAnimation->getTicksPerSecond() * newDeltaTime;
            currentTime = fmod(currentTime, currentAnimation->getDuration());

            const kAssimpNodeData& rootNode = currentAnimation->getRootNode();
            calculateBoneTransform(&rootNode, glm::mat4(1.0f));
        }
        // Make sure it's only updated once per frame
        currentFrameId = frameId;
    }

    void kAnimator::playAnimation(kAnimation* animation)
    {
        if (animation != nullptr)
        {
            currentAnimation = animation;
            currentTime = 0.0f;
        }
    }

    kAnimation* kAnimator::getCurrentAnimation()
    {
        return currentAnimation;
    }

    void kAnimator::calculateBoneTransform(const kAssimpNodeData* node, glm::mat4 parentTransform)
    {
        if (node != nullptr)
        {
            string nodeName = node->name;
            glm::mat4 nodeTransform = node->transformation;

            //std::cout << "Found node: " << nodeName << ", transform: " << glm::to_string(nodeTransform) << std::endl;

            kBone* bone = currentAnimation->findBone(nodeName);

            if (bone != nullptr)
            {
                //std::cout << "Before update, nodeTransform: " << glm::to_string(nodeTransform) << std::endl;

                //std::cout << "Found bone: " << nodeName << std::endl;
                //std::cout << "Updating bone: " << nodeName << " at time: " << currentTime << std::endl;

                bone->update(currentTime);

                //////////////////////////////////////
                // Disable this then become firstframe
                //nodeTransform = bone->getLocalTransform() * nodeTransform;
                nodeTransform = bone->getLocalTransform();

                //std::cout << "After update, nodeTransform: " << glm::to_string(nodeTransform) << std::endl;
            }

            // Fixed using inverse()

            /*glm::mat4 invNodeTransform = glm::inverse(node->transformation);
            if (glm::any(glm::isnan(invNodeTransform[0])) || glm::any(glm::isnan(invNodeTransform[1])) ||
                glm::any(glm::isnan(invNodeTransform[2])) || glm::any(glm::isnan(invNodeTransform[3])))
            {
                std::cout << "Invalid inverse detected for node: " << nodeName << std::endl;
                invNodeTransform = glm::mat4(1.0f); // Use identity matrix to prevent NaN propagation
            }

            glm::mat4 globalTransformation = parentTransform * nodeTransform;  // Fallback
            if (glm::determinant(node->transformation) > 0.00001f)  // Prevent singular matrix inversion
            {
                globalTransformation = parentTransform * invNodeTransform * nodeTransform;
            }*/


            //glm::mat4 globalTransformation = parentTransform * glm::inverse(node->transformation) * nodeTransform;
            glm::mat4 globalTransformation = parentTransform * nodeTransform;

            if (currentAnimation->getMeshes().size() > 0)
            {
                for (size_t i = 0; i < currentAnimation->getMeshes().size(); ++i)
                {
                    if (currentAnimation->getMeshes().at(i)->getType() == kNodeType::NODE_TYPE_MESH)
                    {
                        kMesh* childMesh = (kMesh*) currentAnimation->getMeshes().at(i);

                        std::map<string, kBoneInfo>& boneInfoMap = childMesh->getBoneInfoMap();

                        //std::cout << boneInfoMap.size() << std::endl;

                        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
                        {
                            int index = boneInfoMap[nodeName].id;
                            glm::mat4 offset = boneInfoMap[nodeName].offset;

                            finalBoneMatrices[index] = globalTransformation * offset;

                            //std::cout << "Bone: " << nodeName << ", Offset: " << glm::to_string(offset) << std::endl;

                            //std::cout << "offset: " << glm::to_string(offset) << std::endl;
                            //std::cout << "parentTransform: " << glm::to_string(parentTransform) << ", nodeTransform: " << glm::to_string(nodeTransform) << std::endl;
                            //std::cout << "name: " << nodeName << ", index: " << index << ", global trans: " << glm::to_string(globalTransformation) << ", offset: " << glm::to_string(offset) << ", matrix: " << glm::to_string(finalBoneMatrices[index]) << std::endl;

                        }
                    }
                }
            }

            for (int i = 0; i < node->childrenCount; i++)
            {
                calculateBoneTransform(&node->children[i], globalTransformation);

                //std::cout << "i: " << i << ", globalTrans: " << glm::to_string(globalTransformation) << std::endl;
            }
        }
    }

    const std::vector<glm::mat4> kAnimator::getFinalBoneMatrices() const
    {
        //std::cout << glm::to_string(finalBoneMatrices[0]) << std::endl;

        /*for (size_t i = 0; i < finalBoneMatrices.size(); i++)
        {
            if (finalBoneMatrices[i] == glm::mat4(0.0f))
            {
                //std::cout << "WARNING: Final Bone Matrix " << i << " is ZERO!" << std::endl;
            }
        }*/

        return finalBoneMatrices;
    }

    void kAnimator::setCurrentTime(float newTime)
    {
        currentTime = newTime;
    }

    void kAnimator::setSpeed(float newSpeed)
    {
        speed = newSpeed;
    }

    float kAnimator::getSpeed()
    {
        return speed;
    }

}
