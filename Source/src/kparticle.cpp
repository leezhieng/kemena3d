#include "kparticle.h"

#include <algorithm>

namespace kemena
{
    void kParticleManager::update(float dt)
    {
        // Placeholder — particle simulation will be implemented here.
        (void)dt;
    }

    void kParticleManager::addEmitter(kParticle *particle)
    {
        if (particle)
            emitters.push_back(particle);
    }

    void kParticleManager::removeEmitter(const kString &uuid)
    {
        emitters.erase(
            std::remove_if(emitters.begin(), emitters.end(),
                [&uuid](kParticle *p) { return p && p->uuid == uuid; }),
            emitters.end());
    }

} // namespace kemena
