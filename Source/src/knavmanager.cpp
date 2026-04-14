#include "knavmanager.h"

#ifdef _MSC_VER
#  pragma warning(push, 0)
#endif

#include <DetourCrowd.h>
#include <DetourNavMesh.h>
#include <DetourTileCache.h>

#ifdef _MSC_VER
#  pragma warning(pop)
#endif

#include <algorithm>

namespace kemena
{

struct kNavManager::Impl
{
    kNavMesh   *mesh       = nullptr;
    dtCrowd    *crowd      = nullptr;
    int         maxAgents  = 128;
    bool        initialized = false;

    std::vector<kNavAgent*>    agents;
    std::vector<kNavObstacle*> obstacles;
};

kNavManager::kNavManager(kNavMesh *mesh, int maxAgents)
    : m_impl(new Impl)
{
    m_impl->mesh      = mesh;
    m_impl->maxAgents = maxAgents;
}

kNavManager::~kNavManager()
{
    shutdown();
    delete m_impl;
}

bool kNavManager::init()
{
    if (!m_impl->mesh || !m_impl->mesh->isBaked()) return false;

    m_impl->crowd = dtAllocCrowd();
    if (!m_impl->crowd) return false;

    dtNavMesh *nm = static_cast<dtNavMesh*>(m_impl->mesh->getNavMesh());
    if (!m_impl->crowd->init(m_impl->maxAgents, 0.6f, nm))
    {
        dtFreeCrowd(m_impl->crowd);
        m_impl->crowd = nullptr;
        return false;
    }

    // Default avoidance params (quality level 3 = best)
    dtObstacleAvoidanceParams avoidParams{};
    avoidParams.velBias          = 0.4f;
    avoidParams.weightDesVel     = 2.0f;
    avoidParams.weightCurVel     = 0.75f;
    avoidParams.weightSide       = 0.75f;
    avoidParams.weightToi        = 2.5f;
    avoidParams.horizTime        = 2.5f;
    avoidParams.gridSize         = 33;
    avoidParams.adaptiveDivs     = 7;
    avoidParams.adaptiveRings    = 2;
    avoidParams.adaptiveDepth    = 5;
    m_impl->crowd->setObstacleAvoidanceParams(3, &avoidParams);

    m_impl->initialized = true;
    return true;
}

void kNavManager::shutdown()
{
    for (kNavAgent *a : m_impl->agents)
    {
        a->uninit();
        delete a;
    }
    m_impl->agents.clear();

    for (kNavObstacle *o : m_impl->obstacles)
    {
        o->uninit();
        delete o;
    }
    m_impl->obstacles.clear();

    if (m_impl->crowd)
    {
        dtFreeCrowd(m_impl->crowd);
        m_impl->crowd = nullptr;
    }
    m_impl->initialized = false;
}

void kNavManager::update(float deltaTime)
{
    if (!m_impl->initialized) return;

    // Step the tile cache (processes obstacle add/remove, rebuilds dirty tiles)
    dtTileCache *tc = static_cast<dtTileCache*>(m_impl->mesh->getTileCache());
    if (tc)
    {
        dtNavMesh *nm = static_cast<dtNavMesh*>(m_impl->mesh->getNavMesh());
        tc->update(deltaTime, nm);
    }

    // Step the crowd
    m_impl->crowd->update(deltaTime, nullptr);
}

kNavAgent* kNavManager::addAgent(const kVec3 &position, const kNavAgentConfig &config)
{
    if (!m_impl->initialized) return nullptr;

    kNavAgent *agent = new kNavAgent();
    bool ok = agent->init(m_impl->crowd,
                          m_impl->mesh->getNavMeshQuery(),
                          position, config);
    if (!ok) { delete agent; return nullptr; }

    m_impl->agents.push_back(agent);
    return agent;
}

void kNavManager::removeAgent(kNavAgent *agent)
{
    auto it = std::find(m_impl->agents.begin(), m_impl->agents.end(), agent);
    if (it == m_impl->agents.end()) return;
    m_impl->agents.erase(it);
    agent->uninit();
    delete agent;
}

const std::vector<kNavAgent*> &kNavManager::getAgents() const
{
    return m_impl->agents;
}

kNavObstacle* kNavManager::addObstacle(const kVec3 &position, float radius, float height)
{
    if (!m_impl->initialized) return nullptr;

    dtTileCache *tc = static_cast<dtTileCache*>(m_impl->mesh->getTileCache());
    if (!tc) return nullptr; // tiled build required

    kNavObstacle *obs = new kNavObstacle();
    bool ok = obs->init(tc, position, radius, height);
    if (!ok) { delete obs; return nullptr; }

    m_impl->obstacles.push_back(obs);
    return obs;
}

void kNavManager::removeObstacle(kNavObstacle *obstacle)
{
    auto it = std::find(m_impl->obstacles.begin(), m_impl->obstacles.end(), obstacle);
    if (it == m_impl->obstacles.end()) return;
    m_impl->obstacles.erase(it);
    obstacle->uninit();
    delete obstacle;
}

const std::vector<kNavObstacle*> &kNavManager::getObstacles() const
{
    return m_impl->obstacles;
}

kNavMesh *kNavManager::getNavMesh() const { return m_impl->mesh; }

} // namespace kemena
