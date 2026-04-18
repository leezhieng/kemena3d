#ifndef KMESHGENERATOR_H
#define KMESHGENERATOR_H

#include "kexport.h"
#include "kmesh.h"

namespace kemena
{
    /**
     * @brief Procedural mesh generators — no file I/O, no asset manager needed.
     *
     * All methods return a heap-allocated kMesh with vertex data already uploaded
     * to the GPU (generateVbo() has been called). The caller is responsible for
     * adding the mesh to a scene and assigning a material.
     */
    class KEMENA3D_API kMeshGenerator
    {
    public:
        /** @brief 2×2 quad in the XZ plane, facing +Y. */
        static kMesh *generatePlane(float size = 2.0f);

        /** @brief Axis-aligned cube centred at the origin. */
        static kMesh *generateCube(float size = 2.0f);

        /** @brief UV sphere with the given radius, stacks and slices. */
        static kMesh *generateSphere(float radius = 1.0f, int stacks = 18, int slices = 36);

        /** @brief Upright cylinder with flat caps. */
        static kMesh *generateCylinder(float radius = 1.0f, float height = 2.0f, int slices = 36);

        /** @brief Cylinder with hemispherical caps (total height = height + 2*radius). */
        static kMesh *generateCapsule(float radius = 1.0f, float height = 2.0f,
                                      int slices = 36, int hemiStacks = 8);
    };
}

#endif // KMESHGENERATOR_H
