#include "koctree.h"
#include "kmesh.h"
#include "kscene.h"

#include <algorithm>
#include <cmath>

namespace kemena
{
    // -----------------------------------------------------------------------
    // kFrustum
    // -----------------------------------------------------------------------

    static kVec4 rowOf(const kMat4 &m, int i)
    {
        return kVec4(m[0][i], m[1][i], m[2][i], m[3][i]);
    }

    void kFrustum::extractFromMatrix(const kMat4 &vp)
    {
        kVec4 r0 = rowOf(vp, 0);
        kVec4 r1 = rowOf(vp, 1);
        kVec4 r2 = rowOf(vp, 2);
        kVec4 r3 = rowOf(vp, 3);

        // Gribb-Hartmann: planes are combinations of VP rows
        planes[0] = r3 + r0; // left
        planes[1] = r3 - r0; // right
        planes[2] = r3 + r1; // bottom
        planes[3] = r3 - r1; // top
        planes[4] = r3 + r2; // near
        planes[5] = r3 - r2; // far

        // Normalize each plane so that the distance component is meaningful
        for (auto &p : planes)
        {
            float len = glm::length(kVec3(p));
            if (len > 0.0f) p /= len;
        }
    }

    kFrustumTestResult kFrustum::testAABB(const kAABB &aabb) const
    {
        int fullyInside = 0;

        for (const auto &plane : planes)
        {
            kVec3 n(plane);
            float d = plane.w;

            // Positive vertex — corner of aabb furthest along the plane normal
            kVec3 pos = aabb.min;
            if (n.x >= 0.0f) pos.x = aabb.max.x;
            if (n.y >= 0.0f) pos.y = aabb.max.y;
            if (n.z >= 0.0f) pos.z = aabb.max.z;

            // Negative vertex — corner closest to the plane
            kVec3 neg = aabb.max;
            if (n.x >= 0.0f) neg.x = aabb.min.x;
            if (n.y >= 0.0f) neg.y = aabb.min.y;
            if (n.z >= 0.0f) neg.z = aabb.min.z;

            if (glm::dot(n, pos) + d < 0.0f)
                return kFrustumTestResult::Outside;      // fully outside this plane

            if (glm::dot(n, neg) + d >= 0.0f)
                fullyInside++;                            // fully inside this plane
        }

        return (fullyInside == 6) ? kFrustumTestResult::Inside
                                  : kFrustumTestResult::Intersecting;
    }

    bool kFrustum::intersectsAABB(const kAABB &aabb) const
    {
        return testAABB(aabb) != kFrustumTestResult::Outside;
    }

    // -----------------------------------------------------------------------
    // kOctree::Node
    // -----------------------------------------------------------------------

    void kOctree::Node::subdivide()
    {
        if (!leaf) return;
        leaf = false;

        kVec3 center = bounds.center();
        kVec3 he     = bounds.halfExtents();

        // Create 8 child nodes — one per octant
        const kVec3 offsets[8] = {
            {-1, -1, -1}, { 1, -1, -1},
            {-1,  1, -1}, { 1,  1, -1},
            {-1, -1,  1}, { 1, -1,  1},
            {-1,  1,  1}, { 1,  1,  1}
        };
        for (int i = 0; i < 8; ++i)
        {
            kVec3 childCenter = center + offsets[i] * (he * 0.5f);
            kVec3 childHe     = he * 0.5f;
            children[i] = std::make_unique<Node>();
            children[i]->bounds = kAABB(childCenter - childHe,
                                        childCenter + childHe);
        }
    }

    void kOctree::Node::insert(kMesh *mesh, const kAABB &meshBounds,
                               int depth, int maxDepth, int maxObj)
    {
        // If this is a leaf and not yet full, store here
        if (leaf)
        {
            meshes.push_back(mesh);

            // Subdivide if overfull and not at max depth
            if ((int)meshes.size() > maxObj && depth < maxDepth)
            {
                subdivide();
                // Re-distribute existing meshes into children
                std::vector<kMesh*> kept;
                for (kMesh *m : meshes)
                {
                    kAABB mb = m->getWorldAABB();
                    bool placed = false;
                    for (auto &child : children)
                    {
                        if (child && child->bounds.contains(mb))
                        {
                            child->insert(m, mb, depth + 1, maxDepth, maxObj);
                            placed = true;
                            break;
                        }
                    }
                    if (!placed) kept.push_back(m); // straddles boundary → stays here
                }
                meshes = std::move(kept);
            }
            return;
        }

        // Internal node — try to push into a child that fully contains the mesh
        for (auto &child : children)
        {
            if (child && child->bounds.contains(meshBounds))
            {
                child->insert(mesh, meshBounds, depth + 1, maxDepth, maxObj);
                return;
            }
        }

        // No child fully contains it — store at this level (straddles boundary)
        meshes.push_back(mesh);
    }

    void kOctree::Node::collectAll(std::vector<kMesh*> &out) const
    {
        out.insert(out.end(), meshes.begin(), meshes.end());
        if (!leaf)
            for (const auto &child : children)
                if (child) child->collectAll(out);
    }

    void kOctree::Node::query(const kFrustum &frustum, std::vector<kMesh*> &out) const
    {
        kFrustumTestResult result = frustum.testAABB(bounds);

        if (result == kFrustumTestResult::Outside)
            return; // entire subtree culled

        if (result == kFrustumTestResult::Inside)
        {
            // All children are inside — collect without further plane tests
            collectAll(out);
            return;
        }

        // Intersecting — add meshes at this level, recurse into children
        out.insert(out.end(), meshes.begin(), meshes.end());
        if (!leaf)
            for (const auto &child : children)
                if (child) child->query(frustum, out);
    }

    int kOctree::Node::nodeCount() const
    {
        int n = 1;
        if (!leaf)
            for (const auto &child : children)
                if (child) n += child->nodeCount();
        return n;
    }

    void kOctree::Node::traverse(const std::function<void(const kAABB &, int, bool)> &visitor,
                                 int depth) const
    {
        visitor(bounds, depth, leaf);
        if (!leaf)
            for (const auto &child : children)
                if (child) child->traverse(visitor, depth + 1);
    }

    // -----------------------------------------------------------------------
    // kOctree
    // -----------------------------------------------------------------------

    kOctree::kOctree(int maxDepth, int maxObjectsPerNode)
        : maxDepth(maxDepth), maxObjectsPerNode(maxObjectsPerNode)
    {}

    kOctree::~kOctree() = default;

    void kOctree::clear()
    {
        root.reset();
        totalMeshes = 0;
    }

    void kOctree::collectMeshes(kObject *node, std::vector<kMesh*> &out)
    {
        if (node == nullptr || !node->getActive()) return;

        if (node->getType() == kNodeType::NODE_TYPE_MESH)
        {
            kMesh *mesh = static_cast<kMesh*>(node);
            if (mesh->getLoaded() && mesh->getVisible() && mesh->getMaterial() != nullptr &&
                mesh->getStatic())
                out.push_back(mesh);
        }

        for (kObject *child : node->getChildren())
            collectMeshes(child, out);
    }

    void kOctree::build(kScene *scene)
    {
        clear();

        // Collect all renderable meshes from the scene graph
        std::vector<kMesh*> meshes;
        collectMeshes(scene->getRootNode(), meshes);

        if (meshes.empty()) return;

        // Compute world AABB for each mesh and the overall scene bounds
        std::vector<kAABB> worldBounds;
        worldBounds.reserve(meshes.size());
        kAABB sceneBounds;

        for (kMesh *m : meshes)
        {
            m->calculateModelMatrix(); // ensure world transform is current
            kAABB wb = m->getWorldAABB();
            worldBounds.push_back(wb);
            sceneBounds.merge(wb);
        }

        // Pad the scene bounds slightly so meshes on the boundary don't straddle root
        float maxExtent = glm::compMax(sceneBounds.max - sceneBounds.min);
        kAABB rootBounds = sceneBounds.expanded(maxExtent * 0.01f + 0.1f);

        // Build root node and insert all meshes
        root = std::make_unique<Node>();
        root->bounds = rootBounds;

        for (size_t i = 0; i < meshes.size(); ++i)
            root->insert(meshes[i], worldBounds[i], 0, maxDepth, maxObjectsPerNode);

        totalMeshes = (int)meshes.size();
    }

    std::vector<kMesh*> kOctree::queryVisible(const kFrustum &frustum) const
    {
        std::vector<kMesh*> result;
        if (root)
        {
            result.reserve(totalMeshes);
            root->query(frustum, result);
        }
        return result;
    }

    int kOctree::getNodeCount() const
    {
        return root ? root->nodeCount() : 0;
    }

    void kOctree::traverse(const std::function<void(const kAABB &, int, bool)> &visitor) const
    {
        if (root) root->traverse(visitor, 0);
    }
}
