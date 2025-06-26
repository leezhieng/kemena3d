#include "kAnimation.h"

namespace kemena
{
    kAnimation::kAnimation(const std::string& animationPath, kMesh* newMesh)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate | aiProcess_LimitBoneWeights);
        assert(scene && scene->mRootNode);

        if (scene->mNumAnimations == 0)
        {
            throw std::runtime_error("No animations found in the file: " + animationPath);
        }

        auto animation = scene->mAnimations[0];
        duration = animation->mDuration;
        ticksPerSecond = animation->mTicksPerSecond;
        readHierarchyData(rootNode, scene->mRootNode);

        setMesh(animation, newMesh);

        //readMissingBones(animation, newMesh);

        //std::cout << "name: " << scene->mAnimations[0]->mName.data << ", duration: " << duration << ", ticksPerSecond: " << ticksPerSecond << std::endl;
    }

    kBone* kAnimation::findBone(const std::string& name)
    {
        auto iter = std::find_if(bones.begin(), bones.end(),
            [&](const kBone& bone)
            {
                return bone.getName() == name;
            }
        );
        if (iter == bones.end()) return nullptr;
        else return &(*iter);
    }

    float kAnimation::getTicksPerSecond() const
    {
        return ticksPerSecond;
    }

    float kAnimation::getDuration() const
    {
        return duration;
    }

    const kAssimpNodeData& kAnimation::getRootNode() const
    {
        return rootNode;
    }

    void kAnimation::setMesh(const aiAnimation* animation, kMesh* newMesh)
    {
        meshes.push_back(newMesh);
        readMissingBones(animation, newMesh);

        if (newMesh->getChildren().size() > 0)
        {
            for (size_t i = 0; i < newMesh->getChildren().size(); ++i)
            {
                if (newMesh->getChildren().at(i)->getType() == kNodeType::NODE_TYPE_MESH)
                {
                    kMesh* childMesh = (kMesh*) newMesh->getChildren().at(i);
                    setMesh(animation, childMesh);
                }
            }
        }
    }

    std::vector<kMesh*> kAnimation::getMeshes()
    {
        return meshes;
    }

    void kAnimation::setSpeed(float newSpeed)
    {
        speed = newSpeed;
    }

    float kAnimation::getSpeed()
    {
        return speed;
    }

    void kAnimation::readMissingBones(const aiAnimation* animation, kMesh* setMesh)
    {
        // Only check actual meshes with vertices
        if (setMesh->getVertices().size() > 0)
        {
            size_t size = animation->mNumChannels;

            std::map<string, kBoneInfo>& meshBoneInfoMap = setMesh->getBoneInfoMap();
            int boneCount = setMesh->getBoneCount();

            // Reading channels(bones engaged in an animation and their keyframes)
            for (size_t i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                if (!channel || (channel->mNumPositionKeys == 0 && channel->mNumRotationKeys == 0 && channel->mNumScalingKeys == 0))
                {
                    std::cerr << "Invalid animation channel at index: " << i << std::endl;
                    continue;
                }

                string boneName = channel->mNodeName.data;

                if (meshBoneInfoMap.find(boneName) == meshBoneInfoMap.end())
                {
                    meshBoneInfoMap[boneName].id = boneCount;
                    boneCount++;
                }
                bones.push_back(kBone(channel->mNodeName.data, meshBoneInfoMap[channel->mNodeName.data].id, channel));

                //std::cout << "boneName: " << boneName << ", channel: " << channel << std::endl;
                //std::cout << "id: " << meshBoneInfoMap[boneName].id << std::endl;
                //std::cout << boneCount << std::endl;
            }
            //std::cout << bones.size() << std::endl;
        }
    }

    void kAnimation::readHierarchyData(kAssimpNodeData& dest, const aiNode* src)
    {
        if (!src)
        {
            throw std::runtime_error("Invalid node in hierarchy data.");
        }

        dest.name = src->mName.data;

        glm::mat4 nodeTransform = kAssimpGLMHelpers::convertMatrixToGLMFormat(src->mTransformation);
        dest.transformation = nodeTransform;

        dest.childrenCount = src->mNumChildren;
        for (unsigned int i = 0; i < src->mNumChildren; i++)
        {
            kAssimpNodeData newData;
            readHierarchyData(newData, src->mChildren[i]);  // Pass updated parent transform
            dest.children.push_back(newData);
        }
    }
}
