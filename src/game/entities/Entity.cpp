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
               EntityType entityType,
               double start_x, double start_y,
               double start_w, double start_h,
               double start_xvel, double start_yvel,
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
    m_EntityType = entityType;
    m_Alive = true;
    m_Core = *m_pUnknownCore;
    m_LastCore = *m_pLastUnknownCore;
    m_Core.Pos.x = start_x;
    m_Core.Pos.y = start_y;
    m_Core.Size.x = start_w;
    m_Core.Size.y = start_h;
    m_Core.Vel.x = start_xvel;
    m_Core.Vel.y = start_yvel;
    m_Core.BaseDamping = base_damping;
    m_SpawnedTick = m_World->GetTick();

    m_World->AddEntity(this);
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
                                     double start_x, double start_y,
                                     double start_w, double start_h,
                                     double start_xvel, double start_yvel,
                                     double start_xlook, double start_ylook,
                                     double base_damping)
    : Entity(world,
             ENTFORM_DIRECTIONAL,
             entity_type,
             start_x, start_y,
             start_w, start_h,
             start_xvel, start_yvel,
             base_damping),
      m_DirectionalCore(*(DirectionalEntityCore*) (m_pUnknownCore)),
      m_LastDirectionalCore(*(DirectionalEntityCore*) (m_pLastUnknownCore)) {
    m_DirectionalCore.Direction.x = start_xlook;
    m_DirectionalCore.Direction.y = start_ylook;
}

DirectionalEntity::~DirectionalEntity() = default;