#ifndef KBONE_H
#define KBONE_H

#include <string>
#include <stdexcept>
#include <iostream>

#include "assimp/scene.h"

#include "kdatatype.h"

namespace kemena
{
    class kBone
    {
        public:
            kBone(const string &boneName, int boneID, aiNodeAnim* channel);

            void update(float animationTime);
            const mat4 getLocalTransform() const;
            const string getName() const;
            const int getID() const;
            int getPositionIndex(float animationTime);
            int getRotationIndex(float animationTime);
            int getScaleIndex(float animationTime);

        protected:

        private:
            std::vector<kKeyPosition> positions;
            std::vector<kKeyRotation> rotations;
            std::vector<kKeyScale> scales;
            int positionCount;
            int rotationCount;
            int scaleCount;

            mat4 localTransform = mat4(1.0f);
            string name;
            int id;

            float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime, float duration);
            mat4 interpolatePosition(float animationTime);
            mat4 interpolateRotation(float animationTime);
            mat4 interpolateScale(float animationTime);
    };
}

#endif // KBONE_H
