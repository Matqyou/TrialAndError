// Copied Entity.h into cartesian folder for cartesian-specific entities
#pragma once

#include "client/game/world/GameWorld.h"
#include "shared/math/Vec2.h"

struct EntityCore
{
	Vec3f pos, size, vel;
	float base_damping, size_ratio;

	EntityCore()
	{
		base_damping = 0.0f;
		size_ratio = 0.0f;
	}

	// Manipulating
	void Accelerate(const Vec3f& impulse);
};

struct DirectionalEntityCore : public EntityCore
{
	Vec3f direction;

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
	HasHealth health_component;
	GameWorld *world;
	EntityCore *unknown_core;
	EntityCore& core;
	EntityCore last_core;
	EntityType entity_type;
	bool pending; // is not yet added to the world(next tick)
	bool alive;

	const bool has_health_component;

	// added into world
	Streamer<GameWorld*> entity_added_event;

	virtual void TickUpdateLastCore();
	virtual void TickVelocity(double seconds_elapsed);
	virtual void TickWalls();

public:
	Entity(EntityFormFactor form_factor,
		   EntityType type,
		   const Vec3f& start_pos,
		   const Vec3f& start_size,
		   const Vec3f& start_vel,
		   float base_damping,
		   bool has_health_component,
		   double max_health = 1.0);
	virtual ~Entity();

	// Getting
	[[nodiscard]] GameWorld *World() const { return world; }
	[[nodiscard]] EntityType GetType() const { return entity_type; }
	[[nodiscard]] EntityCore& GetCore() { return core; }
	[[nodiscard]] const EntityCore& GetLastCore() const { return last_core; }
	[[nodiscard]] bool IsAlive() const { return alive; }
	[[nodiscard]] bool HasHealthComponent() const { return has_health_component; }
	[[nodiscard]] HasHealth& HealthComponent() { return health_component; }

	// Generating
	[[nodiscard]] bool PointCollides(const Vec3f& point) const;
	[[nodiscard]] virtual const char *toString() const;

	// Manipulating
	void Accelerate(const Vec3f& direction);

	// Ticking
	virtual void Tick(double elapsed_seconds);
	virtual void Draw();
};

class DirectionalEntity : public Entity
{
protected:
	DirectionalEntityCore& directional_core;
	DirectionalEntityCore last_directional_core;

	// Ticking
	void TickUpdateLastCore() override;

public:
	DirectionalEntity(EntityType entity_type,
					  const Vec3f& start_pos,
					  const Vec3f& start_size,
					  const Vec3f& start_vel,
					  const Vec3f& start_direction,
					  float base_damping,
					  bool has_health_component,
					  double max_health = 1.0);
	~DirectionalEntity() override;

	// Getting
	[[nodiscard]] DirectionalEntityCore& GetDirectionalCore() { return directional_core; }
	[[nodiscard]] const DirectionalEntityCore& GetLastDirectionalCore() const { return last_directional_core; }
};
