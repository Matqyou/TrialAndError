//
// Created by 11dpmbozko on 16.03.2023.
//

#include "Entity.h"

Entity::Entity(GameWorld* world, GameWorld::EntityType entityType,
               double start_x, double start_y, double start_w, double start_h,
               double base_damping = 0.90) {
    m_World = world;
    m_PrevType = nullptr;
    m_NextType = nullptr;
    m_Prev = nullptr;
    m_Next = nullptr;
    m_EntityType = entityType;
    m_x = start_x;
    m_y = start_y;
    m_w = start_w;
    m_h = start_h;
    m_BaseDamping = base_damping;
    m_ExistsSince = m_World->GameWindow()->Timer()->GetTotalTimeElapsed();

    m_World->AddEntity(this);
}

Entity::~Entity() {
    m_World->RemoveEntity(this);
}

void Entity::TickVelocity() {
    m_xvel *= m_BaseDamping;
    m_yvel *= m_BaseDamping;

    m_x += m_xvel;
    m_y += m_yvel;
}

void Entity::TickWalls() {
    double w2 = m_w / 2;
    double h2 = m_h / 2;

    double XWall = m_World->Width() - w2;
    double YWall = m_World->Height() - h2;

    if (m_x < w2) {
        m_x = w2;
        m_xvel = 0.0;
    }
    if (m_x > XWall) {
        m_x = XWall;
        m_xvel = 0.0;
    }
    if (m_y < h2) {
        m_y = h2;
        m_yvel = 0.0;
    }
    if (m_y > YWall) {
        m_y = YWall;
        m_yvel = 0.0;
    }
}
void Entity::TickBouncyWalls() {
    double w2 = m_w / 2;
    double h2 = m_h / 2;

    double XWall = m_World->Width() - w2;
    double YWall = m_World->Height() - h2;

    if (m_x < w2) {
        m_x = w2;
        m_xvel *= -1;
    }
    if (m_x > XWall) {
        m_x = XWall;
        m_xvel *= -1;
    }
    if (m_y < h2) {
        m_y = h2;
        m_yvel *= -1;
    }
    if (m_y > YWall) {
        m_y = YWall;
        m_yvel *= -1;
    }
}

bool Entity::PointCollides(double x, double y) const {
    double w2 = m_w / 2.0;
    double h2 = m_h / 2.0;
    return !(x < m_x - w2 || x > m_x + w2 || y < m_y - h2 || y > m_y + h2);
}

void Entity::Tick() {

}

void Entity::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    Render->SetColor(rand()%255, rand()%255, rand()%255, 255);
    SDL_FRect DrawRect = {float(m_x) - float(m_w/2),
                          float(m_y) - float(m_h/2),
                          float(m_w),
                          float(m_h)};
    Render->FillRectFWorld(DrawRect);
}
