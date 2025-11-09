// Copied Entity.cpp into cartesian folder
#include "client/game/entities/cartesian/base/Entity.h"
#include "client/core/Application.h"
#include "client/game/GameReference.h"

#include <format>

void EntityCore::Accelerate(const Vec3f& impulse)
{
	vel += impulse;
}

HasHealth::HasHealth(const Entity& parent, double initial_health)
	: m_Parent(parent)
{
	m_LastDamager = nullptr;
	m_CombatTick = 0;

	m_Health = initial_health;
	m_MaxHealth = initial_health;
	m_LastHealth = initial_health;
}

bool HasHealth::IsAlive() const
{
	return m_Health > 0.0;
}

bool HasHealth::IsFullHealth() const
{
	return m_Health == m_MaxHealth;
}

double HasHealth::GetHealthInPercentage() const
{
	return m_Health / m_MaxHealth;
}

double HasHealth::GetHealthDifferenceSincePreviousTick() const
{
	return m_Health - m_LastHealth;
}

unsigned long long HasHealth::GetTicksSinceCombat() const
{
	return m_Parent.World()->GetTick() - m_CombatTick;
}

void HasHealth::ChangeHealthBy(double delta)
{
	m_Health += delta;
}

void HasHealth::SetHealth(double value)
{
	m_Health = value;
}

void HasHealth::SetMaxHealth(double value)
{
	m_MaxHealth = value;
}

void HasHealth::HealFully()
{
	m_Health = m_MaxHealth;
}

void HasHealth::LimitHealthToMax()
{
	if (m_Health > m_MaxHealth)
		m_Health = m_MaxHealth;
}

void HasHealth::UpdateDamager(Entity *damager)
{
	m_CombatTick = m_Parent.World()->GetTick();
	m_LastDamager = damager;
}

void HasHealth::TickUpdateLastHealth()
{
	m_LastHealth = m_Health;
}

Entity::Entity(EntityFormFactor form_factor,
			   EntityType type,
			   const Vec3f& start_pos,
			   const Vec3f& start_size,
			   const Vec3f& start_vel,
			   float base_damping,
			   bool init_has_health_component,
			   double max_health)
	: unknown_core((form_factor == DIRECTIONAL_ENTITY) ? new DirectionalEntityCore() : new EntityCore()),
//	  m_pLastUnknownCore((form_factor == DIRECTIONAL_ENTITY) ? new DirectionalEntityCore() : new EntityCore()),
	  core(*unknown_core),
	  last_core(core),
	  has_health_component(init_has_health_component),
	  health_component(*this, max_health)
{
	world = nullptr;

	entity_type = type;
	pending = false;
	alive = true;
//	core = *m_pUnknownCore;
//	last_core = *m_pLastUnknownCore;
	core.pos = start_pos;
	core.size = start_size;
	core.vel = start_vel;
	core.base_damping = base_damping;
	core.size_ratio = (core.size.x + core.size.y) / 4.0f;
	TickUpdateLastCore();

//	our_world->AddEntity(this);
}

Entity::~Entity()
{
	delete unknown_core;

	if (world)
		world->RemoveEntity(this);
}

void Entity::TickUpdateLastCore()
{
	memcpy(&last_core, &core, sizeof(EntityCore));
}

void Entity::TickVelocity(double seconds_elapsed)
{
	core.vel.x *= core.base_damping;
	core.vel.y *= core.base_damping;

	core.pos.x += core.vel.x;
	core.pos.y += core.vel.y;
}

void Entity::TickWalls()
{
	float w2 = core.size.x / 2.0f;
	float h2 = core.size.y / 2.0f;

	float XWall = world->GetWidth() - w2;
	float YWall = world->GetHeight() - h2;

	if (core.pos.x < w2)
	{
		core.pos.x = w2;
		core.vel.x = 0.0;
	}
	if (core.pos.x > XWall)
	{
		core.pos.x = XWall;
		core.vel.x = 0.0;
	}
	if (core.pos.y < h2)
	{
		core.pos.y = h2;
		core.vel.y = 0.0;
	}
	if (core.pos.y > YWall)
	{
		core.pos.y = YWall;
		core.vel.y = 0.0;
	}
}

bool Entity::PointCollides(const Vec3f& point) const
{
	float w2 = core.size.x / 2.0f;
	float h2 = core.size.y / 2.0f;
	float d2 = core.size.z / 2.0f;
	return !(
		point.x < core.pos.x - w2 ||
			point.x > core.pos.x + w2 ||
			point.y < core.pos.y - h2 ||
			point.y > core.pos.y + h2 ||
			point.z > core.pos.z - d2 ||
			point.z > core.pos.z + d2
	);
}

const char *Entity::toString() const
{
	return ENTITY_NAMES[entity_type];
}

void Entity::Accelerate(const Vec3f& direction)
{
	core.vel += direction;
}

void Entity::Tick(double elapsed_seconds)
{

}

void Entity::Draw()
{
//	Drawing *Render = Application.GetDrawing();
//
//	Render->SetColor(255, 255, 255, 255);
//	SDL_FRect DrawRect = { float(core.pos.x) - float(core.size.x / 2),
//						   float(core.pos.y) - float(core.size.y / 2),
//						   float(core.size.x),
//						   float(core.size.y) };
//	Render->DrawRect(DrawRect, true, GameReference.GetCamera());
}

void DirectionalEntity::TickUpdateLastCore()
{
	memcpy(&last_directional_core, &directional_core, sizeof(DirectionalEntityCore));
}

DirectionalEntity::DirectionalEntity(EntityType init_entity_type,
									 const Vec3f& start_pos,
									 const Vec3f& start_size,
									 const Vec3f& start_vel,
									 const Vec3f& start_direction,
									 float base_damping,
									 bool init_has_health_component,
									 double max_health)
	: Entity(
	DIRECTIONAL_ENTITY,
	init_entity_type,
	start_pos,
	start_size,
	start_vel,
	base_damping,
	init_has_health_component,
	max_health
),
	  directional_core(*(DirectionalEntityCore *)(unknown_core)),
	  last_directional_core(directional_core)
{
	directional_core.direction = start_direction;
	TickUpdateLastCore(); // todo: warning
}

DirectionalEntity::~DirectionalEntity() = default;
// Implementation is self-contained in this file (do not include ../Entity.cpp here)