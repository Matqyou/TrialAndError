// Copied Entity.h into cartesian folder for cartesian-specific entities
#pragma once

#include <client/game/world/GameWorld.h>
#include <shared/utility/Vec2.h>

struct EntityCore
{
    Vec2f pos, size, vel;
    float base_damping, size_ratio;

	EntityCore()
	{
		base_damping = 0.0f;
		size_ratio = 0.0f;
	}

    // Manipulating
    void Accelerate(const Vec2f& impulse);
};

struct DirectionalEntityCore : public EntityCore
{
    Vec2f direction;

    DirectionalEntityCore() : EntityCore() { }
};

struct HasHealth
{
protected:
    const Entity& m_Parent; // todo: very bad what the heck
    Entity *m_LastDamager;
    unsigned long long m_CombatTick;

public:
    double m_Health;
    double m_MaxHealth;
    double m_LastHealth;

    // Construction & Destruction
    explicit HasHealth(const Entity& parent, double initial_health);

    // Getting
    [[nodiscard]] Entity *GetDamager() const { return m_LastDamager; }

    // Generating
    [[nodiscard]] bool IsAlive() const;
    [[nodiscard]] bool IsFullHealth() const;
    [[nodiscard]] double GetHealthInPercentage() const;
    [[nodiscard]] double GetHealthDifferenceSincePreviousTick() const;
    [[nodiscard]] unsigned long long GetTicksSinceCombat() const;

    // Setting
    void ChangeHealthBy(double delta);
    void SetHealth(double value);
    void SetMaxHealth(double value);
    void HealFully();
    void LimitHealthToMax();
    virtual void UpdateDamager(Entity *damager);

    // Ticking
    void TickUpdateLastHealth();
};

class Entity
{
protected:
    friend class GameWorld;
    friend class Hook;
    HasHealth m_HealthComponent;
    GameWorld *m_World;
    Entity *m_PrevType, *m_NextType;
    Entity *m_Prev, *m_Next;
    EntityCore *m_pUnknownCore, *m_pLastUnknownCore;
    EntityCore& m_Core, & m_LastCore;
    EntityType m_EntityType;
    unsigned long long m_SpawnedTick;
    bool m_Alive;

    const bool m_HasHealthComponent;

    virtual void TickUpdateLastCore();
    virtual void TickVelocity(double seconds_elapsed);
    virtual void TickWalls();

public:
    Entity(GameWorld *world,
           EntityFormFactor form_factor,
           EntityType entity_type,
           const Vec2f& start_pos,
           const Vec2f& start_size,
           const Vec2f& start_vel,
           double base_damping,
           bool has_health_component,
           double max_health = 1.0);
    virtual ~Entity();

    // Getting
    [[nodiscard]] GameWorld *World() const { return m_World; }
    [[nodiscard]] EntityType GetType() const { return m_EntityType; }
    [[nodiscard]] Entity *Next() const { return m_Next; }
    [[nodiscard]] Entity *Prev() const { return m_Prev; }
    [[nodiscard]] Entity *NextType() const { return m_NextType; }
    [[nodiscard]] Entity *PrevType() const { return m_PrevType; }
    [[nodiscard]] EntityCore& GetCore() { return m_Core; }
    [[nodiscard]] EntityCore& GetLastCore() { return m_LastCore; }
    [[nodiscard]] bool IsAlive() const { return m_Alive; }
    [[nodiscard]] bool HasHealthComponent() const { return m_HasHealthComponent; }
    [[nodiscard]] HasHealth& HealthComponent() { return m_HealthComponent; }

    // Generating
    [[nodiscard]] bool PointCollides(const Vec2f& point) const;
    [[nodiscard]] virtual const char *toString() const;

    // Manipulating
    void Accelerate(const Vec2f& direction);

    // Ticking
    virtual void Tick(double elapsed_seconds);
    virtual void Draw();
};

class DirectionalEntity : public Entity
{
protected:
    DirectionalEntityCore& m_DirectionalCore, & m_LastDirectionalCore;

    // Ticking
    void TickUpdateLastCore() override;

public:
    DirectionalEntity(GameWorld *world,
                      EntityType entity_type,
                      const Vec2f& start_pos,
                      const Vec2f& start_size,
                      const Vec2f& start_vel,
                      const Vec2f& start_direction,
                      double base_damping,
                      bool has_health_component,
                      double max_health = 1.0);
    ~DirectionalEntity() override;

    // Getting
    [[nodiscard]] DirectionalEntityCore& GetDirectionalCore() const { return m_DirectionalCore; }
    [[nodiscard]] DirectionalEntityCore& GetLastDirectionalCore() const { return m_LastDirectionalCore; }
};
