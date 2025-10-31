//
// Created by Matq on 10/12/2024.
//

#pragma once

#include <client/core/Assets.h>
#include <shared/utility/Vec2.h>

struct Particle
{
	Texture *m_Texture;
	Vec2f m_Position;
	Vec2f m_Size;
	Vec2f m_Vel;
	float m_VelDamping;
	float m_Orientation;
	float m_OrientationVel;
	float m_OrientationVelDamping;
	unsigned long long m_Lifetime;
	bool m_Active;

	Particle()
	{
		m_Texture = nullptr;
		m_Position = { };
		m_Size = { };
		m_Vel = { };
		m_Orientation = { };
		m_OrientationVel = { };
		m_Active = false;
	}
	Particle(Texture *texture,
			 const Vec2f& pos,
			 const Vec2f& size,
			 const Vec2f& vel,
			 float vel_damping,
			 float orientation,
			 float orientation_vel,
			 float orientation_vel_damping,
			 unsigned long long lifetime)
	{
		m_Texture = texture;
		m_Position = pos;
		m_Size = size;
		m_Vel = vel;
		m_VelDamping = vel_damping;
		m_Orientation = orientation;
		m_OrientationVel = orientation_vel;
		m_OrientationVelDamping = orientation_vel_damping;
		m_Lifetime = lifetime;

		m_Active = true;
	}

	void Tick();
	void Draw() const;

};

class GameWorld;
class Particles
{
private:
	GameWorld *world;
	Particle m_Particles[1024];
	size_t m_CreateParticleIndex;

public:
	Particles(GameWorld *parent_world);

	// Manipulating
	bool PlayParticle(Particle particle);

	// Ticking
	void Tick();
	void Draw();

};

