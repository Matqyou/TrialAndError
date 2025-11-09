//
// Created by Matq on 14/04/2023.
//

#include "Hook.h"
#include "Character.h"
#include <cmath>

Hook::Hook(Character *parent_character)
{
	parent = parent_character;
	pos = Vec3f(0, 0, 0);
	vel = Vec3f(0, 0, 0);
	max_length = 300.0;
	hook_travel_speed = 35.0;
	wall_drag_force = 0.55;

	hook_strength = 2.0;
	hooker_influence_ratio = 0.0;
	hookist_influence_ratio = 0.0;
	SetInfluenceRatio(0.7);

	deployed = false;
	grabbed = GRABBED_NONE;
	grabbed_entity = nullptr;
}

// The percentage of force that is applied to the hookist
// the remaining percentage will be applied to the hooker.
void Hook::SetInfluenceRatio(double ratio)
{
	hooker_influence_ratio = 1.0 - ratio;
	hookist_influence_ratio = ratio;
}

// Set the hook as retracted
void Hook::Unhook()
{
	deployed = false;
	grabbed = GRABBED_NONE;
	grabbed_entity = nullptr;
}

// Set the hook as grabbed to the wall
void Hook::HookWall()
{
	vel = Vec3f(0, 0, 0);
	grabbed = GRABBED_WALL;
}

void Hook::Tick(double elapsed_seconds)
{
	GameWorld *world = parent->World();
	bool hooking = parent->GetInput().hooking;
	bool last_hooking = parent->GetLastInput().hooking;
	EntityCore& character_core = parent->GetCore();

	if (!deployed && hooking && !last_hooking)
	{
		deployed = true;
		pos = character_core.pos;
		vel = parent->GetDirectionalCore().orientation.GetLook() * hook_travel_speed;
	}
	else if (deployed && !hooking && last_hooking)
	{  // Instant retraction for now
		Unhook();
	}

	if (!deployed)
		return;

	if (grabbed == GRABBED_NONE)
		pos += vel;

	Vec3f travel_direction = pos - character_core.pos;
	float length = travel_direction.LengthF();
	if (length != 0.0f)
		travel_direction /= length;

	if (grabbed == GRABBED_NONE)
	{
		// Make sure hook isn't longer than it is allowed to be
		if (length > max_length)
		{
			pos = character_core.pos + travel_direction * max_length;
			vel -= travel_direction * 2.0f;
		}

		// Hook snaps to player - idk if its good or not cus i havent made it yet
		for (Entity *entity : world->GetEntitiesByType(ENTITY_CHARACTER))
		{
			auto character = (Character *)entity;
			if (character == parent)
				continue;

			if (character->PointCollides(pos))
			{
				grabbed = GRABBED_ENTITY;
				grabbed_entity = character;
				break;
			}
		}
		for (Entity *entity : world->GetEntitiesByType(ENTITY_CRATE))
		{
			auto crate = (Crate *)entity;
			if (crate->PointCollides(pos))
			{
				grabbed = GRABBED_ENTITY;
				grabbed_entity = crate;
				break;
			}
		}

		if (grabbed == GRABBED_NONE)
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
	else if (grabbed == GRABBED_ENTITY && grabbed_entity)
	{
		EntityCore& GrabbedCore = grabbed_entity->GetCore();
		if (length > max_length)
		{
			float Slice = (length - max_length) / 2.0f;
			Vec3f impulse = travel_direction * Slice;
			GrabbedCore.pos -= impulse;
			character_core.pos += impulse;
		}
		pos = GrabbedCore.pos;
//		m_x = GrabbedCore.pos.x;
//		m_y = GrabbedCore.pos.y;
		if (grabbed_entity->GetType() == ENTITY_CHARACTER)
		{
			auto grabbed_character = (Character *)grabbed_entity;
			float grabbed_acceleration = hook_strength * length / max_length * (1 - hooker_influence_ratio);
			float self_influence = hook_strength * length / max_length * hookist_influence_ratio;
			grabbed_character->Accelerate(travel_direction * -grabbed_acceleration);
			parent->Accelerate(travel_direction * self_influence);
		}
	}
	else if (grabbed == GRABBED_WALL)
	{
		if (length > max_length)
		{
			character_core.pos = pos + travel_direction * -max_length;
//			character_core.pos.x = m_x + -TravelX * m_MaxLength;
//			character_core.pos.y = m_y + -TravelY * m_MaxLength;
		}
		character_core.vel += travel_direction * wall_drag_force;
//		character_core.vel.x += TravelX * m_WallDragForce;
//		character_core.vel.y += TravelY * m_WallDragForce;
	}
}