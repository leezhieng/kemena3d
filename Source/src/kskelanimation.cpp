#include "kskelanimation.h"

namespace kemena
{
    kSkeletalAnimation::kSkeletalAnimation()
    {
    }

    kSkeletalAnimation::~kSkeletalAnimation()
    {
    }

    void kSkeletalAnimation::loadAnimation(const std::string& animationPath, kMesh* model)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
        assert(scene && scene->mRootNode);
        auto animation = scene->mAnimations[0];
        duration = animation->mDuration;
        ticksPerSecond = animation->mTicksPerSecond;
        readHeirarchyData(rootNode, scene->mRootNode);
        readMissingBones(animation, *model);
    }

    kBone* kSkeletalAnimation::findBone(std::string& name)
    {
        auto iter = std::find_if(bones.begin(), bones.end(), [&](kBone& bone)
        {
            return bone.getName() == name;
        });
        if (iter == bones.end())
            return nullptr;
        else
            return &(*iter);
    }

    float kSkeletalAnimation::getTicksPerSecond()
    {
        return ticksPerSecond;
    }

    float kSkeletalAnimation::getDuration()
    {
        return duration;
    }

    const kAssimpNodeData& kSkeletalAnimation::getRootNode()
    {
        return rootNode;
    }

    const std::map<std::string, kBoneInfo>& kSkeletalAnimation::getBoneIDMap()
    {
        return boneInfoMap;
    }

    void kSkeletalAnimation::readMissingBones(const aiAnimation* animation, kMesh& model)
    {
        int size = animation->mNumChannels;

        auto& boneInfoMap = model.getBoneInfoMap(); //getting m_BoneInfoMap from Model class
        int& boneCount = model.getBoneCount();      //getting the m_BoneCounter from Model class

        //reading channels(bones engaged in an animation and their keyframes)
        for (int i = 0; i < size; i++)
        {
            auto channel = animation->mChannels[i];
            std::string boneName = channel->mNodeName.data;

            if (boneInfoMap.find(boneName) == boneInfoMap.end())
            {
                boneInfoMap[boneName].id = boneCount;
                boneCount++;
            }
            kBone newBone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel);
            bones.push_back(newBone);
        }

        boneInfoMap = boneInfoMap;
    }

    void kSkeletalAnimation::readHeirarchyData(kAssimpNodeData& dest, const aiNode* src)
    {
        assert(src);

        dest.name = src->mName.data;
        dest.transformation = kAssimpGLMHelpers::convertMatrixToGLMFormat(src->mTransformation);
        dest.childrenCount = src->mNumChildren;

        for (int i = 0; i < src->mNumChildren; i++)
        {
            kAssimpNodeData newData;
            readHeirarchyData(newData, src->mChildren[i]);
            dest.children.push_back(newData);
        }
    }
}
