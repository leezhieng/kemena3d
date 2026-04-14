#include "knavobstacle.h"

#ifdef _MSC_VER
#  pragma warning(push, 0)
#endif

#include <DetourTileCache.h>

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

namespace kemena
{

struct kNavObstacle::Impl
{
    dtTileCache    *tileCache = nullptr;
    dtObstacleRef   ref       = 0;
    kVec3           position  = {};
    float           radius    = 0.0f;
    float           height    = 0.0f;
    bool            valid     = false;
};

kNavObstacle::kNavObstacle()  : m_impl(new Impl) {}
kNavObstacle::~kNavObstacle() { uninit(); delete m_impl; }

bool kNavObstacle::init(void *tileCache, const kVec3 &position, float radius, float height)
{
    m_impl->tileCache = static_cast<dtTileCache*>(tileCache);
    m_impl->position  = position;
    m_impl->radius    = radius;
    m_impl->height    = height;

    float pos[3] = { position.x, position.y, position.z };
    dtStatus st = m_impl->tileCache->addObstacle(pos, radius, height, &m_impl->ref);
    m_impl->valid = dtStatusSucceed(st);
    return m_impl->valid;
}

void kNavObstacle::uninit()
{
    if (m_impl->tileCache && m_impl->valid)
        m_impl->tileCache->removeObstacle(m_impl->ref);
    m_impl->valid = false;
    m_impl->ref   = 0;
}

kVec3 kNavObstacle::getPosition() const { return m_impl->position; }
float kNavObstacle::getRadius()   const { return m_impl->radius; }
float kNavObstacle::getHeight()   const { return m_impl->height; }
bool  kNavObstacle::isValid()     const { return m_impl->valid; }
unsigned int kNavObstacle::getObstacleRef() const { return static_cast<unsigned int>(m_impl->ref); }

void kNavObstacle::setPosition(const kVec3 &pos)
{
    if (!m_impl->tileCache || !m_impl->valid) return;

    // Remove old obstacle and add a new one at the new position
    m_impl->tileCache->removeObstacle(m_impl->ref);
    m_impl->position = pos;
    float p[3] = { pos.x, pos.y, pos.z };
    m_impl->tileCache->addObstacle(p, m_impl->radius, m_impl->height, &m_impl->ref);
}

} // namespace kemena
