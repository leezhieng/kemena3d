/**
 * @file knavobstacle.h
 * @brief Dynamic navigation obstacle (cylinder) backed by dtTileCache.
 */

#ifndef KNAVOBSTACLE_H
#define KNAVOBSTACLE_H

#include "kexport.h"
#include "kdatatype.h"

namespace kemena
{
    // -------------------------------------------------------------------------
    // kNavObstacle
    // -------------------------------------------------------------------------

    /**
     * @brief A dynamic cylinder obstacle carved into the navmesh at runtime.
     *
     * Obstacles require a tiled navmesh (kNavBuildConfig::tileSize > 0).
     * Do not create directly — obtain via kNavManager::addObstacle().
     *
     * @code
     *   kNavObstacle* obs = navManager->addObstacle(pos, 0.8f, 2.0f);
     *   obs->setPosition(newPos);      // moves the obstacle
     *   navManager->removeObstacle(obs); // carves back in
     * @endcode
     */
    class KEMENA3D_API kNavObstacle
    {
    public:
        kNavObstacle();
        ~kNavObstacle();

        // --- Accessors -------------------------------------------------------

        kVec3 getPosition() const;
        float getRadius() const;
        float getHeight() const;

        /**
         * @brief Moves the obstacle to a new world-space position.
         *
         * Internally removes the old dtObstacleRef and adds a new one.
         * Affected tiles are flagged for re-baking on the next
         * kNavManager::update() call.
         */
        void setPosition(const kVec3 &pos);

        /** @brief Returns true if the obstacle is registered in the tile cache. */
        bool isValid() const;

        // --- Internal (used by kNavManager) ----------------------------------

        bool init(void *tileCache, const kVec3 &position,
                  float radius, float height);
        void uninit();

        /** @brief Opaque dtObstacleRef handle. */
        unsigned int getObstacleRef() const;

    protected:
    private:
        struct Impl;
        Impl *m_impl;
    };

} // namespace kemena

#endif // KNAVOBSTACLE_H
