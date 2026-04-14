#include "knavagent.h"

#ifdef _MSC_VER
#  pragma warning(push, 0)
#endif

#include <DetourCrowd.h>
#include <DetourNavMeshQuery.h>
#include <DetourCommon.h>

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

namespace kemena
{

struct kNavAgent::Impl
{
    dtCrowd      *crowd    = nullptr;
    dtNavMeshQuery *query  = nullptr;
    int            idx     = -1;
    bool           valid   = false;
};

kNavAgent::kNavAgent()  : m_impl(new Impl) {}
kNavAgent::~kNavAgent() { uninit(); delete m_impl; }

bool kNavAgent::init(void *crowd, void *navMeshQuery,
                     const kVec3 &position, const kNavAgentConfig &config)
{
    m_impl->crowd = static_cast<dtCrowd*>(crowd);
    m_impl->query = static_cast<dtNavMeshQuery*>(navMeshQuery);

    dtCrowdAgentParams ap{};
    ap.radius            = config.radius;
    ap.height            = config.height;
    ap.maxAcceleration   = config.maxAcceleration;
    ap.maxSpeed          = config.maxSpeed;
    ap.collisionQueryRange   = ap.radius * 12.0f;
    ap.pathOptimizationRange = config.pathOptRange;
    ap.separationWeight  = config.separationWeight;
    ap.updateFlags       = 0;
    if (config.anticipateTurns)   ap.updateFlags |= DT_CROWD_ANTICIPATE_TURNS;
    if (config.optimizeVis)       ap.updateFlags |= DT_CROWD_OPTIMIZE_VIS;
    if (config.optimizeTopo)      ap.updateFlags |= DT_CROWD_OPTIMIZE_TOPO;
    if (config.obstacleAvoidance) ap.updateFlags |= DT_CROWD_OBSTACLE_AVOIDANCE;
    if (config.separation)        ap.updateFlags |= DT_CROWD_SEPARATION;
    ap.obstacleAvoidanceType = 3; // quality level 0-3

    float pos[3] = { position.x, position.y, position.z };
    m_impl->idx = m_impl->crowd->addAgent(pos, &ap);
    m_impl->valid = (m_impl->idx >= 0);
    return m_impl->valid;
}

void kNavAgent::uninit()
{
    if (m_impl->crowd && m_impl->valid)
        m_impl->crowd->removeAgent(m_impl->idx);
    m_impl->valid = false;
    m_impl->idx   = -1;
}

bool kNavAgent::setTarget(const kVec3 &target)
{
    if (!m_impl->valid) return false;

    float tgt[3]    = { target.x, target.y, target.z };
    float extents[3] = { 2.0f, 4.0f, 2.0f };
    dtQueryFilter filter;
    dtPolyRef ref = 0;
    float nearest[3];

    dtStatus st = m_impl->query->findNearestPoly(tgt, extents, &filter, &ref, nearest);
    if (dtStatusFailed(st) || !ref) return false;

    return m_impl->crowd->requestMoveTarget(m_impl->idx, ref, nearest);
}

void kNavAgent::stop()
{
    if (!m_impl->valid) return;
    m_impl->crowd->resetMoveTarget(m_impl->idx);
    // zero requested velocity to stop immediately
    float zero[3] = { 0, 0, 0 };
    m_impl->crowd->requestMoveVelocity(m_impl->idx, zero);
}

kVec3 kNavAgent::getPosition() const
{
    if (!m_impl->valid) return {};
    const dtCrowdAgent *ag = m_impl->crowd->getAgent(m_impl->idx);
    if (!ag) return {};
    return { ag->npos[0], ag->npos[1], ag->npos[2] };
}

kVec3 kNavAgent::getVelocity() const
{
    if (!m_impl->valid) return {};
    const dtCrowdAgent *ag = m_impl->crowd->getAgent(m_impl->idx);
    if (!ag) return {};
    return { ag->vel[0], ag->vel[1], ag->vel[2] };
}

bool kNavAgent::isMoving() const
{
    if (!m_impl->valid) return false;
    const dtCrowdAgent *ag = m_impl->crowd->getAgent(m_impl->idx);
    if (!ag) return false;
    return ag->active && ag->targetState == DT_CROWDAGENT_TARGET_VALID;
}

bool kNavAgent::isValid() const { return m_impl->valid; }

void kNavAgent::setMaxSpeed(float speed)
{
    if (!m_impl->valid) return;
    const dtCrowdAgent *ag = m_impl->crowd->getAgent(m_impl->idx);
    if (!ag) return;
    dtCrowdAgentParams ap = ag->params;
    ap.maxSpeed = speed;
    m_impl->crowd->updateAgentParameters(m_impl->idx, &ap);
}

void kNavAgent::setMaxAcceleration(float accel)
{
    if (!m_impl->valid) return;
    const dtCrowdAgent *ag = m_impl->crowd->getAgent(m_impl->idx);
    if (!ag) return;
    dtCrowdAgentParams ap = ag->params;
    ap.maxAcceleration = accel;
    m_impl->crowd->updateAgentParameters(m_impl->idx, &ap);
}

float kNavAgent::getMaxSpeed() const
{
    if (!m_impl->valid) return 0.0f;
    const dtCrowdAgent *ag = m_impl->crowd->getAgent(m_impl->idx);
    return ag ? ag->params.maxSpeed : 0.0f;
}

float kNavAgent::getMaxAcceleration() const
{
    if (!m_impl->valid) return 0.0f;
    const dtCrowdAgent *ag = m_impl->crowd->getAgent(m_impl->idx);
    return ag ? ag->params.maxAcceleration : 0.0f;
}

int kNavAgent::getAgentIndex() const { return m_impl->idx; }

} // namespace kemena
