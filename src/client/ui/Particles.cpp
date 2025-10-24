//
// Created by Matq on 10/12/2024.
//

#include "Particles.h"

#include <utility>
#include <client/core/TextManager.h>
#include <client/game/world/GameWorld.h>

void Particle::Tick()
{
	m_Vel *= m_VelDamping;
	m_OrientationVel *= m_OrientationVelDamping;
	m_Position += m_Vel;
	m_Orientation += m_OrientationVel;

	if (m_Lifetime == 0 || m_Vel.Length() <= 0.001)
		m_Active = false;

	m_Lifetime--;
}

void Particle::Draw(Drawing *drawing) const
{
	SDL_FRect asd = {
		(float)m_Position.x,
		(float)m_Position.y,
		(float)m_Size.x,
		(float)m_Size.y,
	};

	drawing->RenderTextureExFCamera(m_Texture->SDLTexture(), nullptr, asd, m_Orientation, nullptr, SDL_FLIP_NONE);
}

Particles::Particles(GameWorld *game_world)
{
	m_World = game_world;
	m_Drawing = game_world->GameWindow()->Render();
	m_CreateParticleIndex = 0;

	for (auto& particle : m_Particles)
		particle = Particle();
}

bool Particles::PlayParticle(Particle particle)
{
	bool success = false;
	for (int i = 0; i < 1024; i++)
	{
		// Occupy space if available
		if (!m_Particles[m_CreateParticleIndex].m_Active)
		{
			m_Particles[m_CreateParticleIndex] = std::move(particle);
			success = true;
		}

		// Go to the next index
		m_CreateParticleIndex++;
		if (m_CreateParticleIndex >= 1024)
			m_CreateParticleIndex = 0;

		// Leave if found a space successfully
		if (success) return true;
	}

	// There was no space for the particle
	return false;
}

void Particles::Tick()
{
	for (auto& particle : m_Particles)
	{
		if (!particle.m_Active)
			continue;

		if (particle.m_Position.x < 0)
		{
			particle.m_Position.x = 0;
			particle.m_Vel.x *= -1;
		}
		if (particle.m_Position.x > m_World->GetWidth())
		{
			particle.m_Position.x = m_World->GetWidth();
			particle.m_Vel.x *= -1;
		}
		if (particle.m_Position.y < 0)
		{
			particle.m_Position.y = 0;
			particle.m_Vel.y *= -1;
		}
		if (particle.m_Position.y > m_World->GetHeight())
		{
			particle.m_Position.y = m_World->GetHeight();
			particle.m_Vel.y *= -1;
		}

		particle.Tick();
	}
}

void Particles::Draw()
{
	for (auto& particle : m_Particles)
	{
		if (!particle.m_Active)
			continue;

		particle.Draw(m_Drawing);
	}
}