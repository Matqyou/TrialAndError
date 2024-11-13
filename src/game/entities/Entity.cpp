//
// Created by 11dpmbozko on 16.03.2023.
//

#include "Entity.h"

void EntityCore::Accelerate(double x, double y) {
    Vel.x += x;
    Vel.y += y;
}

Entity::Entity(GameWorld* world,
               EntityFormFactor form_factor,
               EntityType entity_type,
               const Vec2d& start_pos,
               const Vec2d& start_size,
               const Vec2d& start_vel,
               double base_damping = 0.90)
    : m_pUnknownCore(form_factor == ENTFORM_DIRECTIONAL ? new DirectionalEntityCore() : new EntityCore()),
      m_pLastUnknownCore(form_factor == ENTFORM_DIRECTIONAL ? new DirectionalEntityCore() : new EntityCore()),
      m_Core(*m_pUnknownCore),
      m_LastCore(*m_pLastUnknownCore) {
    m_World = world;
    m_PrevType = nullptr;
    m_NextType = nullptr;
    m_Prev = nullptr;
    m_Next = nullptr;
    m_EntityType = entity_type;
    m_Alive = true;
    m_Core = *m_pUnknownCore;
    m_LastCore = *m_pLastUnknownCore;
    m_Core.Pos = start_pos;
    m_Core.Size = start_size;
    m_Core.Vel = start_vel;
    m_Core.BaseDamping = base_damping;
    m_SpawnedTick = m_World->GetTick();

    m_World->AddEntity(this);
    m_Core.sizeRatio = (m_Core.Size.x + m_Core.Size.y) / 4.0;

}

Entity::~Entity() {
    delete m_pUnknownCore;
    delete m_pLastUnknownCore;
    m_World->RemoveEntity(this);
}

void Entity::TickLastCore() {
    memcpy(&m_LastCore, &m_Core, sizeof(EntityCore));
}

void Entity::TickVelocity() {
    m_Core.Vel.x *= m_Core.BaseDamping;
    m_Core.Vel.y *= m_Core.BaseDamping;

    m_Core.Pos.x += m_Core.Vel.x;
    m_Core.Pos.y += m_Core.Vel.y;
}

void Entity::TickWalls() {
    double w2 = m_Core.Size.x / 2.0;
    double h2 = m_Core.Size.y / 2.0;

    double XWall = m_World->GetWidth() - w2;
    double YWall = m_World->GetHeight() - h2;

    if (m_Core.Pos.x < w2) {
        m_Core.Pos.x = w2;
        m_Core.Vel.x = 0.0;
    }
    if (m_Core.Pos.x > XWall) {
        m_Core.Pos.x = XWall;
        m_Core.Vel.x = 0.0;
    }
    if (m_Core.Pos.y < h2) {
        m_Core.Pos.y = h2;
        m_Core.Vel.y = 0.0;
    }
    if (m_Core.Pos.y > YWall) {
        m_Core.Pos.y = YWall;
        m_Core.Vel.y = 0.0;
    }
}

bool Entity::PointCollides(double x, double y) const {
    double w2 = m_Core.Size.x / 2.0;
    double h2 = m_Core.Size.y / 2.0;
    return !(x < m_Core.Pos.x - w2 ||
        x > m_Core.Pos.x + w2 ||
        y < m_Core.Pos.y - h2 ||
        y > m_Core.Pos.y + h2);
}

// Add some velocity to this character
void Entity::Accelerate(const Vec2d& direction) {
    m_Core.Vel += direction;
}

void Entity::Tick() {

}

void Entity::Draw() {
    Drawing* Render = m_World->GameWindow()->Render();

    Render->SetColor(255, 255, 255, 255);
    SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2),
                           float(m_Core.Pos.y) - float(m_Core.Size.y / 2),
                           float(m_Core.Size.x),
                           float(m_Core.Size.y) };
    Render->FillRectFCamera(DrawRect);
}

void DirectionalEntity::TickLastCore() {
    memcpy(&m_LastDirectionalCore, &m_DirectionalCore, sizeof(DirectionalEntityCore));
}

DirectionalEntity::DirectionalEntity(GameWorld* world, EntityType entity_type,
                                     const Vec2d& start_pos,
                                     const Vec2d& start_size,
                                     const Vec2d& start_vel,
                                     const Vec2d& start_direction,
                                     double base_damping)
    : Entity(world,
             ENTFORM_DIRECTIONAL,
             entity_type,
             start_pos,
             start_size,
             start_vel,
             base_damping),
      m_DirectionalCore(*(DirectionalEntityCore*)(m_pUnknownCore)),
      m_LastDirectionalCore(*(DirectionalEntityCore*)(m_pLastUnknownCore)) {
    m_DirectionalCore.Direction = start_direction;
}

DirectionalEntity::~DirectionalEntity() = default;