#ifndef KANIMATION_H
#define KANIMATION_H

#include "kdatatype.h"
#include "kmesh.h"
#include "kbone.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace kemena
{
    class kMesh;

    class kAnimation
    {
    public:
        kAnimation(const std::string &animationPath, kMesh *setMesh);

        kBone *findBone(const std::string &name);
        float getTicksPerSecond() const;
        float getDuration() const;
        const kAssimpNodeData &getRootNode() const;

        void setMesh(const aiAnimation *animation, kMesh *newMesh);
        std::vector<kMesh *> getMeshes();

        void setSpeed(float newSpeed);
        float getSpeed();

    protected:
    private:
        float duration;
        int ticksPerSecond;
        std::vector<kBone> bones;
        kAssimpNodeData rootNode;

        std::vector<kMesh *> meshes;

        void readMissingBones(const aiAnimation *animation, kMesh *setMesh);
        void readHierarchyData(kAssimpNodeData &dest, const aiNode *src);

        std::map<std::string, kBoneInfo> boneInfoMap;

        float speed = 1.0f;
    };
}

#endif // KANIMATION_H
