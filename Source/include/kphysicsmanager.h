/**
 * @file kphysicsmanager.h
 * @brief Physics subsystem manager — owns the simulation world and acts as a factory for kPhysicsObject.
 */

#ifndef KPHYSICSMANAGER_H
#define KPHYSICSMANAGER_H

#include "kexport.h"
#include "kdatatype.h"
#include "kphysicsobject.h"

namespace kemena
{
    /**
     * @brief Owns the Jolt PhysicsSystem and manages the lifecycle of all physics bodies.
     *
     * Create one kPhysicsManager per scene.  Call init() once, then step the simulation
     * each frame with update().  After each update, sync moving objects back to their
     * scene nodes via kObject::syncFromPhysics().
     *
     * @code
     *   kPhysicsManager* physics = kemena::createPhysicsManager();
     *
     *   // Create a dynamic box
     *   kPhysicsObjectDesc desc;
     *   desc.shape.type        = kPhysicsShapeType::Box;
     *   desc.shape.halfExtents = kVec3(1.f, 1.f, 1.f);
     *   desc.position          = kVec3(0.f, 10.f, 0.f);
     *   kPhysicsObject* box = physics->createObject(desc);
     *
     *   // Attach to a scene node
     *   myObject->attachPhysics(box);
     *
     *   // Game loop:
     *   physics->update(deltaTime);
     *   myObject->syncFromPhysics();
     * @endcode
     */
    class KEMENA3D_API kPhysicsManager
    {
    public:
        kPhysicsManager();
        ~kPhysicsManager();

        // --- Lifecycle -------------------------------------------------------

        /**
         * @brief Initialises the Jolt physics engine and internal systems.
         * @return true on success.
         */
        bool init();

        /**
         * @brief Destroys all physics bodies and tears down the engine.
         * Called automatically by the destructor.
         */
        void shutdown();

        // --- Simulation ------------------------------------------------------

        /**
         * @brief Advances the simulation by @p deltaTime seconds.
         * @param deltaTime Time since the last frame in seconds.
         * Call this once per game-loop iteration before reading body transforms.
         */
        void update(float deltaTime);

        // --- World settings --------------------------------------------------

        /**
         * @brief Sets the global gravity vector (m/s²).
         * @param gravity Acceleration vector — default is kVec3(0, -9.81, 0).
         */
        void setGravity(const kVec3 &gravity);

        /** @brief Returns the current global gravity vector (m/s²). */
        kVec3 getGravity() const;

        // --- Object factory --------------------------------------------------

        /**
         * @brief Creates a physics body and returns a new kPhysicsObject.
         *
         * The returned pointer is owned by this manager.  Release it with
         * destroyObject() rather than deleting it directly.
         *
         * @param desc Full shape and motion parameters.
         * @return Pointer to the new kPhysicsObject, or nullptr on failure.
         */
        kPhysicsObject *createObject(const kPhysicsObjectDesc &desc);

        /**
         * @brief Removes a physics body from the simulation and destroys the object.
         * @param object Pointer returned by createObject().
         */
        void destroyObject(kPhysicsObject *object);

    protected:
    private:
        struct Impl;
        Impl *m_impl;
    };

} // namespace kemena

#endif // KPHYSICSMANAGER_H
