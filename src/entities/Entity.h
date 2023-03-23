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
    Entity* m_PrevEntity, *m_NextEntity;
    double m_x, m_y, m_w, m_h;
    double m_BaseDamping;
    double m_xvel, m_yvel;
    double m_ExistsSince;
    bool m_Destroy;

    GameWorld::EntityType m_EntityType; // .._. .. _.._   ._.. ._ _ . ._.
    void TickVelocity();
    void TickWalls();
    void TickBouncyWalls();

public:
    Entity(GameWorld* world, GameWorld::EntityType entityType,
           double start_x, double start_y, double start_w, double start_h,
           double base_damping);
    ~Entity();

    GameWorld::EntityType EntityType() const { return m_EntityType; }

    virtual void Tick();
    virtual void Draw();
};

#endif //TRIALANDERROR_ENTITY_H
