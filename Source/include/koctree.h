/**
 * @file koctree.h
 * @brief Frustum class and loose octree for scene-level mesh culling.
 */

#ifndef KOCTREE_H
#define KOCTREE_H

#include "kexport.h"
#include "kdatatype.h"

#include <vector>
#include <memory>
#include <unordered_set>
#include <functional>

namespace kemena
{
    class kMesh;
    class kScene;
    class kObject;

    // -----------------------------------------------------------------------

    enum class kFrustumTestResult { Outside, Intersecting, Inside };

    /**
     * @brief View frustum defined by six planes extracted from a view-projection matrix.
     *
     * Call extractFromMatrix() with (projection * view) each frame before querying.
     */
    class KEMENA3D_API kFrustum
    {
    public:
        /**
         * @brief Extract the six frustum planes from a combined view-projection matrix.
         *
         * Uses the Gribb-Hartmann method (row combination of the VP matrix).
         * The planes are stored in the form  n·x + d = 0  where positive values
         * are inside the frustum.
         *
         * @param viewProjection  projection * view matrix.
         */
        void extractFromMatrix(const kMat4 &viewProjection);

        /**
         * @brief Test an AABB against the frustum.
         * @return Outside      — AABB is fully outside at least one plane.
         *         Intersecting — AABB crosses one or more planes.
         *         Inside       — AABB is fully inside all six planes.
         */
        kFrustumTestResult testAABB(const kAABB &aabb) const;

        /** @brief Quick reject: returns false if the AABB is fully outside. */
        bool intersectsAABB(const kAABB &aabb) const;

    private:
        kVec4 planes[6]; // (nx, ny, nz, d) — positive half-space is inside
    };

    // -----------------------------------------------------------------------

    /**
     * @brief Loose octree that spatially indexes scene meshes for frustum culling.
     *
     * Usage per frame:
     * @code
     *   octree.build(scene);                        // rebuild from scene
     *   kFrustum frustum;
     *   frustum.extractFromMatrix(proj * view);
     *   auto visible = octree.queryVisible(frustum); // cull
     *   for (kMesh *m : visible) renderMesh(m);
     * @endcode
     */
    class KEMENA3D_API kOctree
    {
    public:
        /**
         * @param maxDepth          Maximum recursion depth (default 6).
         * @param maxObjectsPerNode Meshes per leaf before subdivision (default 8).
         */
        kOctree(int maxDepth = 6, int maxObjectsPerNode = 8);
        ~kOctree();

        /**
         * @brief Rebuild the tree from all loaded meshes in the scene.
         *
         * Computes world-space AABBs, derives world bounds, then inserts every
         * mesh into the appropriate octree node.  Call once per frame (or
         * whenever the scene changes).
         */
        void build(kScene *scene);

        /**
         * @brief Return all meshes whose world AABB intersects the frustum.
         *
         * Traverses the tree top-down; entire subtrees whose bounds are outside
         * the frustum are skipped, and subtrees fully inside are collected
         * without further plane tests.
         */
        std::vector<kMesh*> queryVisible(const kFrustum &frustum) const;

        /** @brief Discard all nodes and mesh references. */
        void clear();

        int getNodeCount()  const;
        int getMeshCount()  const { return totalMeshes; }

        /**
         * @brief Visit every node in the tree top-down.
         *
         * The callback receives the node's world-space AABB, its depth (root = 0),
         * and whether it is a leaf node.  Useful for debug visualization.
         *
         * @param visitor  Callable: void(const kAABB &bounds, int depth, bool isLeaf)
         */
        void traverse(const std::function<void(const kAABB &, int, bool)> &visitor) const;

    private:
        struct Node
        {
            kAABB bounds;
            std::vector<kMesh*> meshes;
            std::unique_ptr<Node> children[8];
            bool leaf = true;

            void subdivide();
            void insert(kMesh *mesh, const kAABB &meshBounds, int depth,
                        int maxDepth, int maxObj);
            void collectAll(std::vector<kMesh*> &out) const;
            void query(const kFrustum &frustum, std::vector<kMesh*> &out) const;
            void traverse(const std::function<void(const kAABB &, int, bool)> &visitor,
                          int depth) const;
            int  nodeCount() const;
        };

        std::unique_ptr<Node> root;
        int maxDepth;
        int maxObjectsPerNode;
        int totalMeshes = 0;

        static void collectMeshes(kObject *node, std::vector<kMesh*> &out);
    };
}

#endif // KOCTREE_H
