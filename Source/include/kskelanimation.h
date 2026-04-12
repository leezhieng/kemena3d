#ifndef KSKELETALANIMATION_H
#define KSKELETALANIMATION_H

#include "kexport.h"

#include <map>
#include <vector>

#include "kdatatype.h"
#include "kbone.h"
#include "kmesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace kemena
{
    class KEMENA3D_API kSkeletalAnimation
    {
    public:
        kSkeletalAnimation();
        virtual ~kSkeletalAnimation();

        void loadAnimation(const string &animationPath, kMesh *model);
        kBone *findBone(string &name);
        float getTicksPerSecond();
        float getDuration();
        const kAssimpNodeData &getRootNode();
        const std::map<string, kBoneInfo> &getBoneIDMap();

    protected:
    private:
        float duration;
        int ticksPerSecond;
        std::vector<kBone> bones;
        kAssimpNodeData rootNode;
        std::map<string, kBoneInfo> boneInfoMap;

        void readMissingBones(const aiAnimation *animation, kMesh &model);
        void readHeirarchyData(kAssimpNodeData &dest, const aiNode *src);
    };
}

#endif // KSKELETALANIMATION_H
