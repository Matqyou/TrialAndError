//
// Created by 11dpmbozko on 16.03.2023.
//

#ifndef TRIALANDERROR_ENTITY_H
#define TRIALANDERROR_ENTITY_H

#include "../../GameWorld.h"

struct EntityCore {
    double m_x, m_y, m_w, m_h;
    double m_xvel, m_yvel;
    double m_BaseDamping;

    void Accelerate(double x, double y);
};

struct LookingEntityCore : public EntityCore {
    double m_xlook, m_ylook;
};

enum EntityFormFactor {
    ENTITY_NORMAL,
    ENTITY_LOOKING
};

class Entity {
protected:
    friend class GameWorld;
    friend class Hook;
    GameWorld* m_World;
    Entity* m_PrevType, *m_NextType;
    Entity* m_Prev, *m_Next;
    unsigned long long m_ExistsSince;
    EntityCore* m_Core, *m_LastCore;
    GameWorld::EntityType m_EntityType;
    bool m_Alive;

    virtual void TickLastCore();
    void TickVelocity();
    void TickWalls();

public:
    Entity(GameWorld* world,
           EntityFormFactor form_factor,
           GameWorld::EntityType entity_type,
           double start_x, double start_y,
           double start_w, double start_h,
           double start_xvel, double start_yvel,
           double base_damping);
    virtual ~Entity();

    GameWorld* World() const { return m_World; }
    GameWorld::EntityType EntityType() const { return m_EntityType; }
    Entity* Next() const { return m_Next; }
    Entity* Prev() const { return m_Prev; }
    Entity* NextType() const { return m_NextType; }
    Entity* PrevType() const { return m_PrevType; }
    bool PointCollides(double x, double y) const;
    EntityCore* GetCore() const { return m_Core; }
    EntityCore* GetLastCore() const { return m_LastCore; }
    bool IsAlive() const { return m_Alive; }

    void Accelerate(double accelerate_x, double accelerate_y);

    virtual void Tick();
    virtual void Draw();
};

class LookingEntity : public Entity {
protected:
    LookingEntityCore* m_LookingCore, *m_LastLookingCore;
    void TickLastCore() override;

public:
    LookingEntity(GameWorld* world,
                  GameWorld::EntityType entity_type,
                  double start_x, double start_y,
                  double start_w, double start_h,
                  double start_xvel, double start_yvel,
                  double start_xlook, double start_ylook,
                  double base_damping);
    virtual ~LookingEntity();
};

#endif //TRIALANDERROR_ENTITY_H
