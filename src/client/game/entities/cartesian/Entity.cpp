// Copied Entity.cpp into cartesian folder
#include "Entity.h"
#include <client/core/Application.h>
#include <client/game/GameReference.h>

#include <format>

void EntityCore::Accelerate(const Vec2f& impulse)
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

Entity::Entity(GameWorld *world,
			   EntityFormFactor form_factor,
			   EntityType entity_type,
			   const Vec2f& start_pos,
			   const Vec2f& start_size,
			   const Vec2f& start_vel,
			   double base_damping,
			   bool has_health_component,
			   double max_health)
	: m_pUnknownCore(form_factor == DIRECTIONAL_ENTITY ? new DirectionalEntityCore() : new EntityCore()),
	  m_pLastUnknownCore(form_factor == DIRECTIONAL_ENTITY ? new DirectionalEntityCore() : new EntityCore()),
	  m_Core(*m_pUnknownCore),
	  m_LastCore(*m_pLastUnknownCore),
	  m_HasHealthComponent(has_health_component),
	  m_HealthComponent(*this, max_health)
{
	m_World = world;

	m_PrevType = nullptr;
	m_NextType = nullptr;
	m_Prev = nullptr;
	m_Next = nullptr;
	m_EntityType = entity_type;
	m_Alive = true;
	m_Core = *m_pUnknownCore;
	m_LastCore = *m_pLastUnknownCore;
	m_Core.pos = start_pos;
	m_Core.size = start_size;
	m_Core.vel = start_vel;
	m_Core.base_damping = base_damping;
	m_Core.size_ratio = (m_Core.size.x + m_Core.size.y) / 4.0;
	m_SpawnedTick = m_World->GetTick();
	TickUpdateLastCore();

	m_World->AddEntity(this);
}

Entity::~Entity()
{
	delete m_pUnknownCore;
	delete m_pLastUnknownCore;
	m_World->RemoveEntity(this);
}

void Entity::TickUpdateLastCore()
{
	memcpy(&m_LastCore, &m_Core, sizeof(EntityCore));
}

void Entity::TickVelocity(double seconds_elapsed)
{
	m_Core.vel.x *= m_Core.base_damping;
	m_Core.vel.y *= m_Core.base_damping;

	m_Core.pos.x += m_Core.vel.x;
	m_Core.pos.y += m_Core.vel.y;
}

void Entity::TickWalls()
{
	double w2 = m_Core.size.x / 2.0;
	double h2 = m_Core.size.y / 2.0;

	double XWall = m_World->GetWidth() - w2;
	double YWall = m_World->GetHeight() - h2;

	if (m_Core.pos.x < w2)
	{
		m_Core.pos.x = w2;
		m_Core.vel.x = 0.0;
	}
	if (m_Core.pos.x > XWall)
	{
		m_Core.pos.x = XWall;
		m_Core.vel.x = 0.0;
	}
	if (m_Core.pos.y < h2)
	{
		m_Core.pos.y = h2;
		m_Core.vel.y = 0.0;
	}
	if (m_Core.pos.y > YWall)
	{
		m_Core.pos.y = YWall;
		m_Core.vel.y = 0.0;
	}
}

bool Entity::PointCollides(const Vec2f& point) const
{
	float w2 = m_Core.size.x / 2.0f;
	float h2 = m_Core.size.y / 2.0f;
	return !(point.x < m_Core.pos.x - w2 ||
		point.x > m_Core.pos.x + w2 ||
		point.y < m_Core.pos.y - h2 ||
		point.y > m_Core.pos.y + h2);
}

const char *Entity::toString() const
{
	return ENTITY_NAMES[m_EntityType];
}

void Entity::Accelerate(const Vec2f& direction)
{
	m_Core.vel += direction;
}

void Entity::Tick(double elapsed_seconds)
{

}

void Entity::Draw()
{
	Drawing *Render = Application.GetDrawing();

	Render->SetColor(255, 255, 255, 255);
	SDL_FRect DrawRect = { float(m_Core.pos.x) - float(m_Core.size.x / 2),
						   float(m_Core.pos.y) - float(m_Core.size.y / 2),
						   float(m_Core.size.x),
						   float(m_Core.size.y) };
	Render->DrawRect(DrawRect, true, GameReference.GetCamera());
}

void DirectionalEntity::TickUpdateLastCore()
{
	memcpy(&m_LastDirectionalCore, &m_DirectionalCore, sizeof(DirectionalEntityCore));
}

DirectionalEntity::DirectionalEntity(GameWorld *world, EntityType entity_type,
									 const Vec2f& start_pos,
									 const Vec2f& start_size,
									 const Vec2f& start_vel,
									 const Vec2f& start_direction,
									 double base_damping,
									 bool has_health_component,
									 double max_health)
	: Entity(world,
			 DIRECTIONAL_ENTITY,
			 entity_type,
			 start_pos,
			 start_size,
			 start_vel,
			 base_damping,
			 has_health_component,
			 max_health),
	  m_DirectionalCore(*(DirectionalEntityCore *)(m_pUnknownCore)),
	  m_LastDirectionalCore(*(DirectionalEntityCore *)(m_pLastUnknownCore))
{
	m_DirectionalCore.direction = start_direction;
	TickUpdateLastCore();
}

DirectionalEntity::~DirectionalEntity() = default;
// Implementation is self-contained in this file (do not include ../Entity.cpp here)