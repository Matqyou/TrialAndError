//
// Created by Matq on 10/12/2024.
//

#include "Particles.h"

#include <client/core/TextManager.h>
#include <client/game/world/GameWorld.h>
#include "client/game/GameReference.h"
#include "client/core/Application.h"
#include "client/core/TextManager.h"
#include <utility>

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

void Particle::Draw() const
{
	auto drawing = Application.GetDrawing();
	SDL_FRect rect = {
		(float)m_Position.x,
		(float)m_Position.y,
		(float)m_Size.x,
		(float)m_Size.y,
	};

	drawing->RenderTextureRotated(m_Texture->SDLTexture(), nullptr, rect, m_Orientation, nullptr, SDL_FLIP_NONE, GameReference.GetCamera());
}

Particles::Particles(GameWorld *parent_world)
{
	world = parent_world;
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
		if (particle.m_Position.x > world->GetWidth())
		{
			particle.m_Position.x = world->GetWidth();
			particle.m_Vel.x *= -1;
		}
		if (particle.m_Position.y < 0)
		{
			particle.m_Position.y = 0;
			particle.m_Vel.y *= -1;
		}
		if (particle.m_Position.y > world->GetHeight())
		{
			particle.m_Position.y = world->GetHeight();
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

		particle.Draw();
	}
}