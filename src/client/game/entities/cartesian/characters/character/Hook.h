//
// Created by Matq on 14/04/2023.
//

#pragma once

#include "client/game/entities/cartesian/base/Entity.h"

class Character;
struct Hook
{
	enum
	{
		GRABBED_NONE,
		GRABBED_ENTITY,
		GRABBED_WALL
	};

	Character *parent;
	Vec2f pos;
	Vec2f vel;
	float max_length;
	float hook_travel_speed;
	float wall_drag_force;
	float hook_strength;
	float hooker_influence_ratio;
	float hookist_influence_ratio;
	bool deployed;
	int grabbed;
	Entity *grabbed_entity;

	explicit Hook(Character *parent_character);

	// Setting
	void SetInfluenceRatio(double ratio);

	// Manipulating
	void Unhook();
	void HookWall();

	// Ticking
	void Tick(double elapsed_seconds);

};
