//
// Created by Matq on 14/04/2023.
//

#include "Hook.h"
#include "Character.h"
#include <cmath>

Hook::Hook(Character *parent)
{
	m_Parent = parent;
	m_x = 0.0;
	m_y = 0.0;
	m_xvel = 0.0;
	m_yvel = 0.0;
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
	m_xvel = 0.0;
	m_yvel = 0.0;
	m_Grabbed = GRABBED_WALL;
}

void Hook::Tick(double elapsed_seconds)
{
	GameWorld *World = m_Parent->World();
	bool Hooking = m_Parent->GetInput().m_Hooking;
	bool LastHooking = m_Parent->GetLastInput().m_Hooking;
	double LookX = m_Parent->GetInput().m_LookingX;
	double LookY = m_Parent->GetInput().m_LookingY;
	EntityCore& CharCore = m_Parent->GetCore();

	if (!m_Deployed && Hooking && !LastHooking)
	{
		m_Deployed = true;
		m_x = CharCore.Pos.x;
		m_y = CharCore.Pos.y;
		m_xvel = LookX * m_HookTravelSpeed;
		m_yvel = LookY * m_HookTravelSpeed;
	}
	else if (m_Deployed && !Hooking && LastHooking)
	{  // Instant retraction for now
		Unhook();
	}

	if (!m_Deployed)
		return;

	if (m_Grabbed == GRABBED_NONE)
	{
		m_x += m_xvel;
		m_y += m_yvel;
	}

	double TravelX = m_x - CharCore.Pos.x;
	double TravelY = m_y - CharCore.Pos.y;
	double Length = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
	if (Length != 0.0)
	{
		TravelX /= Length;
		TravelY /= Length;
	}

	if (m_Grabbed == GRABBED_NONE)
	{
		// Make sure hook isn't longer than it is allowed to be
		if (Length > m_MaxLength)
		{
			m_x = CharCore.Pos.x + TravelX * m_MaxLength;
			m_y = CharCore.Pos.y + TravelY * m_MaxLength;
			m_xvel -= TravelX * 2.0;
			m_yvel -= TravelY * 2.0;
		}

		// Hook snaps to player - idk if its good or not cus i havent made it yet
		for (Entity* entity : World->GetEntitiesByType(CHARACTER_ENTITY))
		{
			auto character = (Character*)entity;
			if (character == m_Parent)
				continue;

			if (character->PointCollides(Vec2d(m_x, m_y)))
			{
				m_Grabbed = GRABBED_ENTITY;
				m_GrabbedEntity = character;
				break;
			}
		}
		for (Entity* entity : World->GetEntitiesByType(CRATE_ENTITY))
		{
			auto crate = (Crate*)entity;
			if (crate->PointCollides(Vec2d(m_x, m_y)))
			{
				m_Grabbed = GRABBED_ENTITY;
				m_GrabbedEntity = crate;
				break;
			}
		}

		if (m_Grabbed == GRABBED_NONE)
		{
			// Hook snaps to wall - fix later cus ugly, prob fix when adding tiles and stuff cus doesnt rly matter tbh
			if (m_x < 0.0)
			{
				m_x = 0.0;
				HookWall();
			}
			else if (m_y < 0.0)
			{
				m_y = 0.0;
				HookWall();
			}
			else if (m_x > World->GetWidth())
			{
				m_x = World->GetWidth();
				HookWall();
			}
			else if (m_y > World->GetHeight())
			{
				m_y = World->GetHeight();
				HookWall();
			}
		}
	}
	else if (m_Grabbed == GRABBED_ENTITY)
	{
		EntityCore& GrabbedCore = m_GrabbedEntity->GetCore();
		if (Length > m_MaxLength)
		{
			double Slice = (Length - m_MaxLength) / 2;
			GrabbedCore.Pos.x -= TravelX * Slice;
			GrabbedCore.Pos.y -= TravelY * Slice;
			CharCore.Pos.x += TravelX * Slice;
			CharCore.Pos.y += TravelY * Slice;
		}
		m_x = GrabbedCore.Pos.x;
		m_y = GrabbedCore.Pos.y;
		if (m_GrabbedEntity->GetType() == CHARACTER_ENTITY)
		{
			auto Player = (Character *)(m_GrabbedEntity);
			double Acceleration = m_HookStrength * Length / m_MaxLength * (1 - m_HookerInfluenceRatio);
			double Influence = m_HookStrength * Length / m_MaxLength * m_HookerInfluenceRatio;
			Player->Accelerate(Vec2d(TravelX, TravelY) * -Acceleration);
			m_Parent->Accelerate(Vec2d(TravelX, TravelY) * Influence);
		}
	}
	else if (m_Grabbed == GRABBED_WALL)
	{
		if (Length > m_MaxLength)
		{
			CharCore.Pos.x = m_x + -TravelX * m_MaxLength;
			CharCore.Pos.y = m_y + -TravelY * m_MaxLength;
		}
		CharCore.Vel.x += TravelX * m_WallDragForce;
		CharCore.Vel.y += TravelY * m_WallDragForce;
	}
}