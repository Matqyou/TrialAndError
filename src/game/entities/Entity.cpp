//
// Created by 11dpmbozko on 16.03.2023.
//

#include "Entity.h"

void EntityCore::Accelerate(double x, double y) {
    m_xvel += x;
    m_yvel += y;
}

Entity::Entity(GameWorld* world,
               EntityFormFactor form_factor,
               GameWorld::EntityType entityType,
               double start_x, double start_y,
               double start_w, double start_h,
               double start_xvel, double start_yvel,
               double base_damping = 0.90) {
    m_World = world;
    m_PrevType = nullptr;
    m_NextType = nullptr;
    m_Prev = nullptr;
    m_Next = nullptr;
    m_EntityType = entityType;
    m_Core = nullptr;
    m_LastCore = nullptr;
    m_Alive = true;
    if (form_factor == ENTITY_LOOKING) {
        m_Core = new LookingEntityCore();
        m_LastCore = new LookingEntityCore();
        memset(m_Core, 0, sizeof(LookingEntityCore));
        memset(m_LastCore, 0, sizeof(LookingEntityCore));
    } else {
        m_Core = new EntityCore();
        m_LastCore = new EntityCore();
        memset(m_Core, 0, sizeof(EntityCore));
        memset(m_LastCore, 0, sizeof(EntityCore));
    }
    m_Core->m_x = start_x;
    m_Core->m_y = start_y;
    m_Core->m_w = start_w;
    m_Core->m_h = start_h;
    m_Core->m_xvel = start_xvel;
    m_Core->m_yvel = start_yvel;
    m_Core->m_BaseDamping = base_damping;
    m_SpawnedTick = m_World->GetTick();

    m_World->AddEntity(this);
}

Entity::~Entity() {
    delete m_Core;
    delete m_LastCore;
    m_World->RemoveEntity(this);
}

void Entity::TickLastCore() {
    memcpy(m_LastCore, m_Core, sizeof(EntityCore));
}

void Entity::TickVelocity() {
    m_Core->m_xvel *= m_Core->m_BaseDamping;
    m_Core->m_yvel *= m_Core->m_BaseDamping;

    m_Core->m_x += m_Core->m_xvel;
    m_Core->m_y += m_Core->m_yvel;
}

void Entity::TickWalls() {
    double w2 = m_Core->m_w / 2.0;
    double h2 = m_Core->m_h / 2.0;

    double XWall = m_World->GetWidth() - w2;
    double YWall = m_World->GetHeight() - h2;

    if (m_Core->m_x < w2) {
        m_Core->m_x = w2;
        m_Core->m_xvel = 0.0;
    }
    if (m_Core->m_x > XWall) {
        m_Core->m_x = XWall;
        m_Core->m_xvel = 0.0;
    }
    if (m_Core->m_y < h2) {
        m_Core->m_y = h2;
        m_Core->m_yvel = 0.0;
    }
    if (m_Core->m_y > YWall) {
        m_Core->m_y = YWall;
        m_Core->m_yvel = 0.0;
    }
}

bool Entity::PointCollides(double x, double y) const {
    double w2 = m_Core->m_w / 2.0;
    double h2 = m_Core->m_h / 2.0;
    return !(x < m_Core->m_x - w2 || x > m_Core->m_x + w2 || y < m_Core->m_y - h2 || y > m_Core->m_y + h2);
}

// Add some velocity to this character
void Entity::Accelerate(double accelerate_x, double accelerate_y) {
    m_Core->m_xvel += accelerate_x;
    m_Core->m_yvel += accelerate_y;
}

void Entity::Tick() {

}

void Entity::Draw() {
    Drawing* Render = m_World->GameWindow()->Render();

    Render->SetColor(255, 255, 255, 255);
    SDL_FRect DrawRect = {float(m_Core->m_x) - float(m_Core->m_w / 2),
                          float(m_Core->m_y) - float(m_Core->m_h / 2),
                          float(m_Core->m_w),
                          float(m_Core->m_h)};
    Render->FillRectFWorld(DrawRect);
}

void LookingEntity::TickLastCore() {
    memcpy(m_LastLookingCore, m_LookingCore, sizeof(LookingEntityCore));
}

LookingEntity::LookingEntity(GameWorld* world, GameWorld::EntityType entity_type,
                             double start_x, double start_y,
                             double start_w, double start_h,
                             double start_xvel, double start_yvel,
                             double start_xlook, double start_ylook,
                             double base_damping)
 : Entity(world, ENTITY_LOOKING, entity_type, start_x, start_y, start_w, start_h, start_xvel, start_yvel, base_damping) {
    m_LookingCore = (LookingEntityCore*)m_Core;
    m_LastLookingCore = (LookingEntityCore*)m_LastCore;
    m_LookingCore->m_xlook = start_xlook;
    m_LookingCore->m_ylook = start_ylook;
}

LookingEntity::~LookingEntity() = default;