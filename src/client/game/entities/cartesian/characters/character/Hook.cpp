//
// Created by Matq on 14/04/2023.
//

#include "Hook.h"
#include "Character.h"
#include <cmath>

Hook::Hook(Character *parent)
{
	m_Parent = parent;
	pos = Vec2f(0.0f, 0.0f);
	vel = Vec2f(0.0f, 0.0f);
	m_MaxLength = 300.0;
	m_HookTravelSpeed = 35.0;
	m_WallDragForce = 0.55;

	m_HookStrength = 2.0;
	m_HookerInfluenceRatio = 0.0;
	m_HookistInfluenceRatio = 0.0;
	SetInfluenceRatio(0.7);

	m_Deployed = false;
	m_Grabbed = GRABBED_NONE;
	m_GrabbedEntity = nullptr;
}

// The percentage of force that is applied to the hookist
// the remaining percentage will be applied to the hooker.
void Hook::SetInfluenceRatio(double ratio)
{
	m_HookerInfluenceRatio = 1.0 - ratio;
	m_HookistInfluenceRatio = ratio;
}

// Set the hook as retracted
void Hook::Unhook()
{
	m_Deployed = false;
	m_Grabbed = GRABBED_NONE;
	m_GrabbedEntity = nullptr;
}

// Set the hook as grabbed to the wall
void Hook::HookWall()
{
	vel = Vec2f(0.0f, 0.0f);
	m_Grabbed = GRABBED_WALL;
}

void Hook::Tick(double elapsed_seconds)
{
	GameWorld *world = m_Parent->World();
	bool hooking = m_Parent->GetInput().hooking;
	bool last_hooking = m_Parent->GetLastInput().hooking;
	Vec2f looking_direction = m_Parent->GetInput().looking_direction;
	EntityCore& character_core = m_Parent->GetCore();

	if (!m_Deployed && hooking && !last_hooking)
	{
		m_Deployed = true;
		pos = character_core.pos;
		vel = looking_direction * m_HookTravelSpeed;
	}
	else if (m_Deployed && !hooking && last_hooking)
	{  // Instant retraction for now
		Unhook();
	}

	if (!m_Deployed)
		return;

	if (m_Grabbed == GRABBED_NONE)
		pos += vel;

	Vec2f travel_direction = pos - character_core.pos;
	float length = travel_direction.LengthF();
	if (length != 0.0f)
		travel_direction /= length;

	if (m_Grabbed == GRABBED_NONE)
	{
		// Make sure hook isn't longer than it is allowed to be
		if (length > m_MaxLength)
		{
			pos = character_core.pos + travel_direction * m_MaxLength;
			vel -= travel_direction * 2.0f;
		}

		// Hook snaps to player - idk if its good or not cus i havent made it yet
		for (Entity *entity : world->GetEntitiesByType(CHARACTER_ENTITY))
		{
			auto character = (Character *)entity;
			if (character == m_Parent)
				continue;

			if (character->PointCollides(pos))
			{
				m_Grabbed = GRABBED_ENTITY;
				m_GrabbedEntity = character;
				break;
			}
		}
		for (Entity *entity : world->GetEntitiesByType(CRATE_ENTITY))
		{
			auto crate = (Crate *)entity;
			if (crate->PointCollides(pos))
			{
				m_Grabbed = GRABBED_ENTITY;
				m_GrabbedEntity = crate;
				break;
			}
		}

		if (m_Grabbed == GRABBED_NONE)
		{
			// Hook snaps to wall - fix later cus ugly, prob fix when adding tiles and stuff cus doesnt rly matter tbh
			if (pos.x < 0.0f)
			{
				pos.x = 0.0f;
				HookWall();
			}
			else if (pos.y < 0.0f)
			{
				pos.y = 0.0f;
				HookWall();
			}
			else if (pos.x > world->GetWidth())
			{
				pos.x = world->GetWidth();
				HookWall();
			}
			else if (pos.y > world->GetHeight())
			{
				pos.y = world->GetHeight();
				HookWall();
			}
		}
	}
	else if (m_Grabbed == GRABBED_ENTITY)
	{
		EntityCore& GrabbedCore = m_GrabbedEntity->GetCore();
		if (length > m_MaxLength)
		{
			float Slice = (length - m_MaxLength) / 2.0f;
			Vec2f impulse = travel_direction * Slice;
			GrabbedCore.pos -= impulse;
			character_core.pos += impulse;
		}
		pos = GrabbedCore.pos;
//		m_x = GrabbedCore.pos.x;
//		m_y = GrabbedCore.pos.y;
		if (m_GrabbedEntity->GetType() == CHARACTER_ENTITY)
		{
			auto other = (Character *)m_GrabbedEntity;
			float other_acceleration = m_HookStrength * length / m_MaxLength * (1 - m_HookerInfluenceRatio);
			float self_influence = m_HookStrength * length / m_MaxLength * m_HookerInfluenceRatio;
			other->Accelerate(travel_direction * -other_acceleration);
			m_Parent->Accelerate(travel_direction * self_influence);
		}
	}
	else if (m_Grabbed == GRABBED_WALL)
	{
		if (length > m_MaxLength)
		{
			character_core.pos = pos + travel_direction * -m_MaxLength;
//			character_core.pos.x = m_x + -TravelX * m_MaxLength;
//			character_core.pos.y = m_y + -TravelY * m_MaxLength;
		}
		character_core.vel += travel_direction * m_WallDragForce;
//		character_core.vel.x += TravelX * m_WallDragForce;
//		character_core.vel.y += TravelY * m_WallDragForce;
	}
}