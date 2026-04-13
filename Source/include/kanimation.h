/**
 * @file kanimation.h
 * @brief Skeletal animation clip loaded from an asset file.
 */

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

    /**
     * @brief Holds a single skeletal animation clip and its bone channel data.
     *
     * Loaded via kAssetManager::loadAnimation().  The clip stores a list of
     * kBone objects (one per animated joint) and the root of the Assimp node
     * hierarchy used by kAnimator to propagate bone transforms.
     *
     * Playback speed can be scaled with setSpeed(); the kAnimator queries
     * getDuration() and getTicksPerSecond() to advance time correctly.
     */
    class kAnimation
    {
    public:
        /**
         * @brief Loads an animation clip from a file and binds it to a mesh.
         * @param animationPath Path to the animation asset file.
         * @param setMesh       Mesh whose bone map is used to resolve bone indices.
         */
        kAnimation(const kString &animationPath, kMesh *setMesh);

        /**
         * @brief Finds the kBone with the given name in this clip.
         * @param name Bone name as it appears in the asset.
         * @return Pointer to the matching kBone, or nullptr if not found.
         */
        kBone *findBone(const kString &name);

        /**
         * @brief Returns the tick rate of this animation.
         * @return Ticks per second (as specified in the asset).
         */
        float getTicksPerSecond() const;

        /**
         * @brief Returns the total duration of this animation in ticks.
         * @return Duration in animation ticks.
         */
        float getDuration() const;

        /**
         * @brief Returns the root node of the bone hierarchy.
         * @return Const reference to the root kAssimpNodeData.
         */
        const kAssimpNodeData &getRootNode() const;

        /**
         * @brief Reads bone channel data from an aiAnimation and binds it to a mesh.
         * @param animation Assimp animation descriptor.
         * @param newMesh   Mesh to bind bone indices from.
         */
        void setMesh(const aiAnimation *animation, kMesh *newMesh);

        /**
         * @brief Returns the list of meshes associated with this animation.
         * @return Copy of the internal mesh pointer vector.
         */
        std::vector<kMesh *> getMeshes();

        /**
         * @brief Sets the playback speed multiplier.
         * @param newSpeed Speed factor (1.0 = normal, 2.0 = double speed).
         */
        void setSpeed(float newSpeed);

        /**
         * @brief Returns the playback speed multiplier.
         * @return Current speed factor.
         */
        float getSpeed();

    protected:
    private:
        float duration;           ///< Total animation length in ticks.
        int ticksPerSecond;       ///< Tick rate from the asset.
        std::vector<kBone> bones; ///< Per-bone channel data.
        kAssimpNodeData rootNode; ///< Root of the node hierarchy.

        std::vector<kMesh *> meshes; ///< Bound mesh references.

        void readMissingBones(const aiAnimation *animation, kMesh *setMesh);
        void readHierarchyData(kAssimpNodeData &dest, const aiNode *src);

        std::map<kString, kBoneInfo> boneInfoMap; ///< Bone name → info lookup.

        float speed = 1.0f; ///< Playback speed multiplier.
    };
}

#endif // KANIMATION_H
