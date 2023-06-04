//
// Created by 11dpmbozko on 16.03.2023.
//

#ifndef TRIALANDERROR_ENTITY_H
#define TRIALANDERROR_ENTITY_H

#include "../../GameWorld.h"

struct EntityCore {
    Vec2d Pos, Size, Vel;
    double BaseDamping{};

    // Manipulating
    void Accelerate(double x, double y);
};

struct DirectionalEntityCore : public EntityCore {
    Vec2d Direction;

    DirectionalEntityCore() : EntityCore() {}
};

class Entity {
protected:
    friend class GameWorld;
    friend class Hook;
    GameWorld* m_World;
    Entity* m_PrevType, * m_NextType;
    Entity* m_Prev, * m_Next;
    unsigned long long m_SpawnedTick;
    EntityCore* m_pUnknownCore, * m_pLastUnknownCore;
    EntityCore& m_Core, & m_LastCore;
    EntityType m_EntityType;
    bool m_Alive;

    virtual void TickLastCore();
    void TickVelocity();
    void TickWalls();

public:
    Entity(GameWorld* world,
           EntityFormFactor form_factor,
           EntityType entity_type,
           double start_x, double start_y,
           double start_w, double start_h,
           double start_xvel, double start_yvel,
           double base_damping);
    virtual ~Entity();

    // Getting
    [[nodiscard]] GameWorld* World() const { return m_World; }
    [[nodiscard]] EntityType GetEntityType() const { return m_EntityType; }
    [[nodiscard]] Entity* Next() const { return m_Next; }
    [[nodiscard]] Entity* Prev() const { return m_Prev; }
    [[nodiscard]] Entity* NextType() const { return m_NextType; }
    [[nodiscard]] Entity* PrevType() const { return m_PrevType; }
    [[nodiscard]] EntityCore& GetCore() { return m_Core; }
    [[nodiscard]] EntityCore& GetLastCore() { return m_LastCore; }
    [[nodiscard]] bool IsAlive() const { return m_Alive; }

    // Generating
    [[nodiscard]] bool PointCollides(double x, double y) const;

    // Manipulating
    void Accelerate(const Vec2d& direction);

    // Ticking
    virtual void Tick();
    virtual void Draw();
};

class DirectionalEntity : public Entity {
protected:
    DirectionalEntityCore& m_DirectionalCore, & m_LastDirectionalCore;

    // Ticking
    void TickLastCore() override;

public:
    DirectionalEntity(GameWorld* world,
                      EntityType entity_type,
                      double start_x, double start_y,
                      double start_w, double start_h,
                      double start_xvel, double start_yvel,
                      double start_xlook, double start_ylook,
                      double base_damping);
    virtual ~DirectionalEntity();

    // Getting
    [[nodiscard]] DirectionalEntityCore& GetDirectionalCore() const { return m_DirectionalCore; }
    [[nodiscard]] DirectionalEntityCore& GetLastDirectionalCore() const { return m_LastDirectionalCore; }
};

#endif //TRIALANDERROR_ENTITY_H
