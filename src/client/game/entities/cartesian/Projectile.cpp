// Copied Projectile.cpp into cartesian
#include "Projectile.h"
#include <cmath>
#include <iostream>
#include "Crate.h"
#include <client/game/entities/cartesian/characters/character/Character.h>

LinkTexture Projectile::sTextureSpark("particle.spark");
LinkSound Projectile::sMetalImpactSounds[2] = {
	LinkSound("entity.projectile.impact.metal.1"),
	LinkSound("entity.projectile.impact.metal.2"),
};

Projectile::Projectile(GameWorld *world,
					   Entity *shooter,
					   WeaponType weapon_type,
					   Texture *projectile_texture,
					   double damage,
					   const Vec2f& start_pos,
					   const Vec2f& start_vel)
	: Entity(world,
			 NORMAL_ENTITY,
			 PROJECTILE_ENTITY,
			 start_pos,
			 Vec2f(6, 10),
			 start_vel,
			 1.0,
			 false)
{
	m_Texture = projectile_texture;
	m_Shooter = shooter;
	m_Type = weapon_type;
	m_Damage = damage;
	m_StillCollidesShooter = true;
}

void Projectile::TickCollision()
{
	Vec2f CurrentPosition = m_Core.pos;
	Vec2f LastPosition = m_LastCore.pos;
	float distance_traveled = DistanceVec2f(CurrentPosition, LastPosition);
	if (distance_traveled <= 0.0)
		return;

	Vec2f direction = (CurrentPosition - LastPosition) / distance_traveled;
	int units_traveled = (int)distance_traveled;

	bool ShooterIsCharacter = m_Shooter->GetType() == CHARACTER_ENTITY;
	auto ShooterCharacter = (Character *)m_Shooter;

	for (int i = 0; i < units_traveled; i++)
	{
		if (!m_Alive)
			break;

		Vec2f current_position = LastPosition + direction * static_cast<float>(i);

		for (Entity *entity : m_World->GetEntities())
		{
			bool IsShooter = (m_Shooter == entity);
			if (!entity->IsAlive()) continue;
			if (!entity->HasHealthComponent()) continue;
			if (!entity->HealthComponent().IsAlive()) continue;

			if (entity->GetType() == CHARACTER_ENTITY)
			{
				auto shootable_character = (Character *)entity;
				if (ShooterIsCharacter && ShooterCharacter->IsNPC() == shootable_character->IsNPC())
					continue;
			}

			bool Collides = entity->PointCollides(current_position);
			if (IsShooter && !Collides)
			{ m_StillCollidesShooter = false; }
			else if (Collides && (!IsShooter || !m_StillCollidesShooter))
			{
				double victim_health;
				if (entity->GetType() == CHARACTER_ENTITY)
				{
					auto shootable_character = (Character *)entity;
					victim_health = shootable_character->HealthComponent().m_Health;
					shootable_character->Damage(m_Damage, m_Shooter);
					shootable_character->Accelerate(direction * 0.5 * m_Damage);
				}
				else if (entity->GetType() == CRATE_ENTITY)
				{
					auto ShootableCrate = (Crate *)entity;
					victim_health = ShootableCrate->HealthComponent().m_Health;
					ShootableCrate->Damage(m_Damage, m_Shooter);
				}
				else
				{
					throw std::runtime_error("Unhandled projectile impact with entity that has health");
				}

				m_Damage -= victim_health;
				if (m_Damage <= 0.0)
				{
					m_Alive = false;
					break;
				}
			}
		}
	}
}

void Projectile::TickWallCollision()
{
	if (m_Core.pos.x < 0 || m_Core.pos.x > m_World->GetWidth() ||
		m_Core.pos.y < 0 || m_Core.pos.y > m_World->GetHeight())
	{
		sMetalImpactSounds[rand() % 2].GetSound()->PlaySound();

		if (m_Core.pos.x < 0)
		{
			m_Core.pos.x = 0;
			m_Core.vel.x *= -1;
		}
		if (m_Core.pos.x > m_World->GetWidth())
		{
			m_Core.pos.x = m_World->GetWidth();
			m_Core.vel.x *= -1;
		}
		if (m_Core.pos.y < 0)
		{
			m_Core.pos.y = 0;
			m_Core.vel.y *= -1;
		}
		if (m_Core.pos.y > m_World->GetHeight())
		{
			m_Core.pos.y = m_World->GetHeight();
			m_Core.vel.y *= -1;
		}
		for (int i = 0; i < 6; i++)
		{
			Vec2f vel = Vec2f(m_Core.vel.x * (0.1 + 0.17 * (double)(rand()) / RAND_MAX) + 0.05 * (double)(rand()) / RAND_MAX,
							  m_Core.vel.y * (0.1 + 0.17 * (double)(rand()) / RAND_MAX) + 0.05 * (double)(rand()) / RAND_MAX);
			float orientation = vel.Atan2F() / static_cast<float>(M_PI) * 180.0f;
			auto lifetime = (unsigned long long)(6 + rand() % 25);
			Vec2f size = Vec2f(m_Core.vel.LengthF() * 0.25f + ((float)(rand() % 100) / 100.0f), 3.0f);
			m_World->GetParticles()->PlayParticle(Particle(sTextureSpark.GetTexture(), m_Core.pos, size, vel, 0.98, orientation, 0.0, 1.0, lifetime));
		}

		m_Alive = false;
	}
}

void Projectile::Tick(double seconds_elapsed)
{
	TickVelocity(seconds_elapsed);
	TickCollision();
	TickWallCollision();
	TickUpdateLastCore();
}

void Projectile::Draw()
{
	Drawing *drawing = Application.GetDrawing();
	double angle = std::atan2(m_Core.vel.y, m_Core.vel.x) / M_PI * 180.0 - 90.0;
	SDL_FRect bullet_rect = { float(m_Core.pos.x - m_Core.size.x / 2.0),
							  float(m_Core.pos.y - m_Core.size.y / 2.0),
							  float(m_Core.size.x),
							  float(m_Core.size.y) };
	drawing->RenderTextureRotated(m_Texture->SDLTexture(),
								  nullptr,
								  bullet_rect,
								  angle,
								  nullptr,
								  SDL_FLIP_NONE,
								  GameReference.GetCamera());
}
