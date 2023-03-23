//
// Created by 11dpmbozko on 16.03.2023.
//

#ifndef TRIALANDERROR_ENTITY_H
#define TRIALANDERROR_ENTITY_H

#include "../GameWorld.h"

class Entity {
protected:
    friend class GameWorld;
    GameWorld* m_World;
    double m_x, m_y, m_w, m_h;
    double m_ExistsSince;

    GameWorld::EntityType m_EntityType; // .._. .. _.._   ._.. ._ _ . ._.

public:
    Entity(GameWorld* world, GameWorld::EntityType entityType,
           double start_x, double start_y, double start_w, double start_h);
    ~Entity();

    GameWorld::EntityType EntityType() const { return m_EntityType; }

    virtual void Tick();
    virtual void Draw();
};

#endif //TRIALANDERROR_ENTITY_H
