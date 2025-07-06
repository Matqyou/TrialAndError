//
// Created by Matq on 15/05/2023.
//

#include "Hands.h"
#include "Character.h"
#include "../CharacterNPC.h"
#include <cmath>

LinkTexture sFistTexture("entity.character.fist");
LinkSound sPunchSound("entity.character.punch");

Hands::Hands(Character *parent, double fist_animation_duration)
	: m_FistingAnimationDuration(fist_animation_duration)
{
	m_Parent = parent;

	m_LastFisted = 0;
	m_LastFistedL = 0;
	m_LastFistedR = 0;

	auto& ParentCore = m_Parent->GetDirectionalCore();
	m_BaseFistingRadius = ParentCore.sizeRatio / 2.5;
	m_FistingRadius = ParentCore.sizeRatio / 2.5;
	m_LeftHand = Vec2d(ParentCore.Size.x / 2.0, -ParentCore.Size.y / 3.0);
	m_RightHand = Vec2d(ParentCore.Size.x / 2.0, ParentCore.Size.y / 3.0);
	m_Size = ParentCore.sizeRatio * 0.36 * 2;
	m_Size2 = m_Size / 2.0;
}

Hands::~Hands()
{

}

void Hands::SetColor(SDL_Color& color)
{
	m_Color = color;
}

void Hands::Tick()
{
	if (!m_Parent->GetErrorStatuses().RangedFists.IsActive())
		m_FistingRadius = m_BaseFistingRadius;
	else if (m_Parent->GetErrorStatuses().RangedFists.IsActive() && m_FistingRadius != m_BaseFistingRadius + 100)
		m_FistingRadius = m_BaseFistingRadius + 100;

	if (m_Parent->GetCurrentWeapon())
		return;

	GameWorld *World = m_Parent->World();
	auto CurrentTick = World->GetTick();
	if (CurrentTick - m_LastFisted < 5)
		return;

	if (m_Parent->GetInput().m_Shooting && !m_Parent->GetLastInput().m_Shooting)
	{
		m_LastFisted = CurrentTick;

		auto& ParentCore = m_Parent->GetDirectionalCore();
		double Radians = m_Parent->GetDirectionalCore().Direction.Atan2();

		double XHands, YHands;
		if (m_LastFistedR < m_LastFistedL)
		{
			m_LastFistedR = CurrentTick;
			XHands = ParentCore.Pos.x + std::cos(Radians) * m_RightHand.x + m_Parent->GetInput().m_LookingX * m_FistingRadius;
			YHands = ParentCore.Pos.y + std::sin(Radians) * m_RightHand.y + m_Parent->GetInput().m_LookingY * m_FistingRadius;
		}
		else
		{
			m_LastFistedL = CurrentTick;
			XHands = ParentCore.Pos.x + std::cos(Radians) * m_LeftHand.x + m_Parent->GetInput().m_LookingX * m_FistingRadius;
			YHands = ParentCore.Pos.y + std::sin(Radians) * m_LeftHand.y + m_Parent->GetInput().m_LookingY * m_FistingRadius;
		}

		auto Ent = World->FirstEntity();
		for (; Ent; Ent = Ent->Next())
		{
			if (Ent == m_Parent) continue;
			if (Ent->GetType() == CHARACTER_ENTITY
				&& m_Parent->IsNPC() == ((Character *)Ent)->IsNPC())
				continue;

			EntityCore& EntCore = Ent->GetCore();
			double XClosest = std::max(EntCore.Pos.x - EntCore.Size.x / 2.0, std::min(EntCore.Pos.x + EntCore.Size.x / 2.0, XHands));
			double YClosest = std::max(EntCore.Pos.y - EntCore.Size.y / 2.0, std::min(EntCore.Pos.y + EntCore.Size.y / 2.0, YHands));
			double Distance = std::sqrt(std::pow(XClosest - XHands, 2) + std::pow(YClosest - YHands, 2));
			if (Distance > m_FistingRadius)
				continue;

			if (!m_Parent->IsNPC() && Ent->GetType() == CRATE_ENTITY)
			{
				((Crate *)Ent)->Damage(5, nullptr);
				continue;
			}

			if (Ent->GetType() != CHARACTER_ENTITY)
				continue;

			auto BoostDirection = Vec2d(m_Parent->GetInput().m_LookingX, m_Parent->GetInput().m_LookingY);
			Ent->Accelerate(BoostDirection * 5.0);

			double Damage = m_Parent->GetBaseDamage() * m_Parent->GetDamageAmp();
			if (!m_Parent->GetPlayer())
			{
				((Character *)Ent)->Damage(Damage, m_Parent);
				continue;
			}
			if (((CharacterNPC *)Ent)->IsBoss())
			{
				Damage = Damage * m_Parent->GetPlayer()->GetBossDamageAmp();
			}
			((Character *)Ent)->Damage(Damage, m_Parent);

		}
	}
}

void Hands::Draw()
{
	GameWorld *World = m_Parent->World();
	auto& ParentCore = m_Parent->GetDirectionalCore();
	auto drawing = Application.GetDrawing();

	auto CurrentTick = World->GetTick();
	auto direction = m_Parent->GetDirectionalCore().Direction;
	double Radians = direction.Atan2();
	double Angle = Radians / M_PI * 180.0;

	double fisting_coefficient_left = double(CurrentTick - m_LastFistedL) / double(m_FistingAnimationDuration);
	double fisting_coefficient_right = double(CurrentTick - m_LastFistedR) / double(m_FistingAnimationDuration);
	if (fisting_coefficient_left > 1.0) fisting_coefficient_left = 1.0;
	if (fisting_coefficient_right > 1.0) fisting_coefficient_right = 1.0;

	fisting_coefficient_left = (1.0 - fisting_coefficient_left) * m_FistingRadius;
	fisting_coefficient_right = (1.0 - fisting_coefficient_right) * m_FistingRadius;

	Vec2d LeftHand, RightHand;
	auto CurrentWeapon = m_Parent->GetCurrentWeapon();
	if (CurrentWeapon != nullptr)
	{
		LeftHand = CurrentWeapon->GetHoldPosition() + CurrentWeapon->GetLeftHand();
		RightHand = CurrentWeapon->GetHoldPosition() + CurrentWeapon->GetRightHand();
	}
	else
	{
		LeftHand = m_LeftHand;
		RightHand = m_RightHand;
	}
	LeftHand.Rotate(Radians);
	RightHand.Rotate(Radians);

	// Punching related stuff
	auto offset_left = LeftHand + direction * fisting_coefficient_left;
	auto offset_right = RightHand + direction * fisting_coefficient_right;

	SDL_FRect HandRectLeft = { float(ParentCore.Pos.x - m_Size2 + offset_left.x),
							   float(ParentCore.Pos.y - m_Size2 + offset_left.y),
							   float(m_Size), float(m_Size) };
	SDL_FRect HandRectRight = { float(ParentCore.Pos.x - m_Size2 + offset_right.x),
								float(ParentCore.Pos.y - m_Size2 + offset_right.y),
								float(m_Size), float(m_Size) };

	sFistTexture.GetTexture()->SetColorMod(m_Color.r, m_Color.g, m_Color.b);
	drawing->RenderTextureRotated(sFistTexture.GetTexture()->SDLTexture(), nullptr, HandRectLeft, Angle, nullptr, SDL_FLIP_NONE, GameReference.GetCamera());
	drawing->RenderTextureRotated(sFistTexture.GetTexture()->SDLTexture(), nullptr, HandRectRight, Angle, nullptr, SDL_FLIP_NONE, GameReference.GetCamera());

//	Render->SetColor(255, 0, 0, 255);
//	Render->RenderDebugPointCamera(ParentCore.Pos.x, ParentCore.Pos.y);
}