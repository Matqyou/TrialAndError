//
// Created by Matq on 15/05/2023.
//

#pragma once

#include <shared/utility/Colors.h>
#include <client/core/Assets.h>
#include <shared/utility/Vec2.h>
#include <client/core/Assets.h>

class Character;
class Hands
{
private:
	Character *m_Parent;
	SDL_Color m_Color;
	unsigned long long m_LastFisted;
	unsigned long long m_LastFistedL, m_LastFistedR;
	const double m_FistingAnimationDuration;
	double m_FistingRadius, m_BaseFistingRadius;

	Vec2d m_LeftHand;
	Vec2d m_RightHand;
	double m_Size, m_Size2;

public:
	Hands(Character *parent, double fist_animation_duration);
	~Hands();

	// Getting
	[[nodiscard]] Character *Parent() const { return m_Parent; }
	[[nodiscard]] unsigned long long LastFisted() const { return m_LastFisted; }

	// Setting
	void SetColor(SDL_Color& color);

	// Ticking
	void Tick();
	void Draw();
};
