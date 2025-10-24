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
	double m_x, m_y;
	double m_xvel, m_yvel;
	double m_MaxLength;
	double m_HookTravelSpeed;
	double m_WallDragForce;
	double m_HookStrength;
	double m_HookerInfluenceRatio;
	double m_HookistInfluenceRatio;
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
	void Tick();
};
