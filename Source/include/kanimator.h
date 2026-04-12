/**
 * @file kanimator.h
 * @brief Drives skeletal animation playback and computes final bone matrices.
 */

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

    /**
     * @brief Controls playback of skeletal animations and produces bone transform matrices.
     *
     * Attach a kAnimator to a kMesh via kMesh::setAnimator().  Each frame,
     * call updateAnimation() with the elapsed delta time and the current frame
     * ID to advance playback.  The resulting bone matrices are fetched with
     * getFinalBoneMatrices() and uploaded to the shader as a uniform array.
     *
     * Multiple animations can be registered with addAnimation() and switched
     * at runtime with playAnimation().
     */
    class kAnimator
    {
    public:
        /**
         * @brief Constructs an animator and sets the initial animation.
         * @param newAnimation Animation to start playing immediately.
         */
        kAnimator(kAnimation *newAnimation);

        /**
         * @brief Registers an additional animation clip.
         * @param newAnimation Clip to add to the internal animation list.
         */
        void addAnimation(kAnimation *newAnimation);

        /**
         * @brief Advances the current animation by one step.
         * @param newDeltaTime Elapsed time since the last update in seconds.
         * @param frameId      Current frame identifier (used to skip duplicate updates).
         */
        void updateAnimation(float newDeltaTime, int frameId);

        /**
         * @brief Switches to a different animation clip immediately.
         * @param animation Clip to start playing; resets the playback time.
         */
        void playAnimation(kAnimation *animation);

        /**
         * @brief Returns the currently active animation clip.
         * @return Pointer to the current kAnimation.
         */
        kAnimation *getCurrentAnimation();

        /**
         * @brief Recursively computes bone transforms for the entire skeleton.
         * @param node            Current hierarchy node being processed.
         * @param parentTransform Accumulated world transform of the parent bone.
         */
        void calculateBoneTransform(const kAssimpNodeData *node, mat4 parentTransform);

        /**
         * @brief Returns the final bone transform matrices ready for shader upload.
         * @return Const reference to the vector of per-bone world transforms.
         */
        const std::vector<mat4> getFinalBoneMatrices() const;

        /**
         * @brief Seeks to a specific time in the current animation.
         * @param newTime Playback time in animation ticks.
         */
        void setCurrentTime(float newTime);

        /**
         * @brief Sets the global playback speed multiplier.
         * @param newSpeed Speed factor (1.0 = normal speed).
         */
        void setSpeed(float newSpeed);

        /**
         * @brief Returns the global playback speed multiplier.
         * @return Current speed factor.
         */
        float getSpeed();

    protected:
    private:
        std::vector<mat4> finalBoneMatrices;         ///< Per-bone final transform matrices.
        kAnimation       *currentAnimation = nullptr; ///< Currently playing animation.
        float             currentTime;               ///< Current playback time in ticks.
        float             deltaTime;                 ///< Last delta time passed to updateAnimation.
        float             speed = 1.0f;              ///< Global speed multiplier.

        std::vector<kAnimation *> animations; ///< Registered animation clips.

        int currentFrameId = -1; ///< Tracks the last processed frame to avoid double-updates.
    };
}

#endif // KANIMATOR_H
