#include "PlanetaryEntity.h"

PlanetaryEntity::PlanetaryEntity(GameWorld* world, const PlanetaryCoords& start_pos, const Vec2d& start_size, const Vec2d& start_vel)
    : m_World(world), m_Prev(nullptr), m_Next(nullptr), m_Core(), m_LastCore(), m_Alive(true)
{
    m_Core.Pos = start_pos;
    m_Core.Size = start_size;
    m_Core.Vel = start_vel;
}

PlanetaryEntity::~PlanetaryEntity() = default;

void PlanetaryEntity::Tick()
{
    // Default behavior: sync last core then no automatic physics – planetary entities should manage their own movement
    m_LastCore = m_Core;
}

void PlanetaryEntity::Draw()
{
    // Default draw does nothing; subclasses should render based on m_Core.ToCartesian()
}

PlanetaryDirectionalEntity::PlanetaryDirectionalEntity(GameWorld* world, const PlanetaryCoords& start_pos, const Vec2d& start_size, const Vec2d& start_vel, const Vec2d& start_direction)
    : PlanetaryEntity(world, start_pos, start_size, start_vel), m_DirectionalCore(), m_LastDirectionalCore()
{
    m_DirectionalCore.Direction = start_direction;
}

PlanetaryDirectionalEntity::~PlanetaryDirectionalEntity() = default;
