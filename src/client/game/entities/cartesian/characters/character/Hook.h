//
// Created by Matq on 14/04/2023.
//

#pragma once

#include <client/game/entities/cartesian/Entity.h>

class Character;
struct Hook
{
	enum
	{
		GRABBED_NONE,
		GRABBED_ENTITY,
		GRABBED_WALL
	};

	Character *m_Parent;
	Vec2f pos;
	Vec2f vel;
	float m_MaxLength;
	float m_HookTravelSpeed;
	float m_WallDragForce;
	float m_HookStrength;
	float m_HookerInfluenceRatio;
	float m_HookistInfluenceRatio;
	bool m_Deployed;
	int m_Grabbed;
	Entity *m_GrabbedEntity;

	explicit Hook(Character *parent);

	// Setting
	void SetInfluenceRatio(double ratio);

	// Manipulating
	void Unhook();
	void HookWall();

	// Ticking
	void Tick(double elapsed_seconds);

};
