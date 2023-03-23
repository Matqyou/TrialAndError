//
// Created by 11dpmbozko on 16.03.2023.
//

#include "Entity.h"

Entity::Entity(GameWorld* world, GameWorld::EntityType entityType,
               double start_x, double start_y, double start_w, double start_h) {
    m_World = world;
    m_EntityType = entityType;
    m_x = start_x;
    m_y = start_y;
    m_w = start_w;
    m_h = start_h;
    m_ExistsSince = m_World->GameWindow()->Timer()->GetTotalTimeElapsed();

    m_World->AddEntity(this);
}

Entity::~Entity() {
    m_World->RemoveEntity(this);
}

void Entity::Tick() {

}

void Entity::Draw() {
    SDL_Renderer* Renderer = m_World->GameWindow()->Renderer();

    SDL_SetRenderDrawColor(Renderer, rand()%255, rand()%255, rand()%255, 255);
    SDL_FRect DrawRect = {float(m_x) - float(m_w/2),
                          float(m_y) - float(m_h/2),
                          float(m_w),
                          float(m_h)};
    SDL_RenderFillRectF(Renderer, &DrawRect);
}
