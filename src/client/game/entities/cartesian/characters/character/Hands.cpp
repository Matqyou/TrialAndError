//
// Created by Matq on 15/05/2023.
//

#include "Hands.h"
#include "Character.h"
#include <client/game/entities/cartesian/characters/CharacterNPC.h>
#include <cmath>

LinkTexture sFistTexture("entity.character.fist");
LinkSound sPunchSound("entity.character.punch");

Hands::Hands(Character *parent, float fist_animation_duration)
	: m_FistingAnimationDuration(fist_animation_duration)
{
	m_Parent = parent;

	m_LastFisted = 0;
	m_LastFistedL = 0;
	m_LastFistedR = 0;

	auto& ParentCore = m_Parent->GetDirectionalCore();
	m_BaseFistingRadius = ParentCore.size_ratio / 2.5f;
	m_FistingRadius = ParentCore.size_ratio / 2.5f;
	m_LeftHand = Vec2f(ParentCore.size.x / 2.0f, -ParentCore.size.y / 3.0f);
	m_RightHand = Vec2f(ParentCore.size.x / 2.0f, ParentCore.size.y / 3.0f);
	m_Size = ParentCore.size_ratio * 0.36 * 2;
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

	if (m_Parent->GetInput().shooting && !m_Parent->GetLastInput().shooting)
	{
		m_LastFisted = CurrentTick;

		auto& ParentCore = m_Parent->GetDirectionalCore();
		float radians = m_Parent->GetDirectionalCore().direction.Atan2F();

//		double hands_x, hands_y;
		Vec2f hands_pos;
		if (m_LastFistedR < m_LastFistedL)
		{
			m_LastFistedR = CurrentTick;
			hands_pos = ParentCore.pos + FromAngleVec2f(radians) * m_RightHand + m_Parent->GetInput().looking_direction * m_FistingRadius;
//			hands_x = ParentCore.Pos.x + std::cos(Radians) * m_RightHand.x + m_Parent->GetInput().m_LookingX * m_FistingRadius;
//			hands_y = ParentCore.Pos.y + std::sin(Radians) * m_RightHand.y + m_Parent->GetInput().m_LookingY * m_FistingRadius;
		}
		else
		{
			m_LastFistedL = CurrentTick;
			hands_pos = ParentCore.pos + FromAngleVec2f(radians) * m_LeftHand + m_Parent->GetInput().looking_direction * m_FistingRadius;
//			hands_x = ParentCore.pos.x + std::cos(radians) * m_LeftHand.x + m_Parent->GetInput().m_LookingX * m_FistingRadius;
//			hands_y = ParentCore.pos.y + std::sin(radians) * m_LeftHand.y + m_Parent->GetInput().m_LookingY * m_FistingRadius;
		}

		for (Entity *entity : World->GetEntities())
		{
			if (entity == m_Parent) continue;
			if (entity->GetType() == CHARACTER_ENTITY &&
				m_Parent->IsNPC() == ((Character *)entity)->IsNPC())
				continue;

			EntityCore& EntCore = entity->GetCore();
			float XClosest = std::max(EntCore.pos.x - EntCore.size.x / 2.0f, std::min(EntCore.pos.x + EntCore.size.x / 2.0f, hands_pos.x));
			float YClosest = std::max(EntCore.pos.y - EntCore.size.y / 2.0f, std::min(EntCore.pos.y + EntCore.size.y / 2.0f, hands_pos.y));
			float Distance =  Vec2f(XClosest - hands_pos.x, YClosest - hands_pos.y).LengthF();
			if (Distance > m_FistingRadius)
				continue;

			if (!m_Parent->IsNPC() && entity->GetType() == CRATE_ENTITY)
			{
				((Crate *)entity)->Damage(5, nullptr);
				continue;
			}

			if (entity->GetType() != CHARACTER_ENTITY)
				continue;

			Vec2f boost_direction = m_Parent->GetInput().looking_direction;
			entity->Accelerate(boost_direction * 5.0f);

			double Damage = m_Parent->GetBaseDamage() * m_Parent->GetDamageAmp();
			if (!m_Parent->GetPlayer())
			{
				((Character *)entity)->Damage(Damage, m_Parent);
				continue;
			}

			if (((CharacterNPC *)entity)->IsBoss())
				Damage = Damage * m_Parent->GetPlayer()->GetBossDamageAmp();
			((Character *)entity)->Damage(Damage, m_Parent);

		}
	}
}

void Hands::Draw()
{
	GameWorld *World = m_Parent->World();
	auto& ParentCore = m_Parent->GetDirectionalCore();
	auto drawing = Application.GetDrawing();

	auto CurrentTick = World->GetTick();
	auto direction = m_Parent->GetDirectionalCore().direction;
	float radians = direction.Atan2F();
	float Angle = radians / static_cast<float>(M_PI) * 180.0f;

	float fisting_coefficient_left = float(CurrentTick - m_LastFistedL) / m_FistingAnimationDuration;
	float fisting_coefficient_right = float(CurrentTick - m_LastFistedR) / m_FistingAnimationDuration;
	if (fisting_coefficient_left > 1.0) fisting_coefficient_left = 1.0;
	if (fisting_coefficient_right > 1.0) fisting_coefficient_right = 1.0;

	fisting_coefficient_left = (1.0 - fisting_coefficient_left) * m_FistingRadius;
	fisting_coefficient_right = (1.0 - fisting_coefficient_right) * m_FistingRadius;

	Vec2f LeftHand, RightHand;
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
	LeftHand = LeftHand.RotateF(radians);
	RightHand = RightHand.RotateF(radians);

	// Punching related stuff
	auto offset_left = LeftHand + direction * fisting_coefficient_left;
	auto offset_right = RightHand + direction * fisting_coefficient_right;

	SDL_FRect HandRectLeft = { float(ParentCore.pos.x - m_Size2 + offset_left.x),
							   float(ParentCore.pos.y - m_Size2 + offset_left.y),
							   float(m_Size), float(m_Size) };
	SDL_FRect HandRectRight = { float(ParentCore.pos.x - m_Size2 + offset_right.x),
								float(ParentCore.pos.y - m_Size2 + offset_right.y),
								float(m_Size), float(m_Size) };

	sFistTexture.GetTexture()->SetColorMod(m_Color.r, m_Color.g, m_Color.b);
	drawing->RenderTextureRotated(sFistTexture.GetTexture()->SDLTexture(), nullptr, HandRectLeft, Angle, nullptr, SDL_FLIP_NONE, GameReference.GetCamera());
	drawing->RenderTextureRotated(sFistTexture.GetTexture()->SDLTexture(), nullptr, HandRectRight, Angle, nullptr, SDL_FLIP_NONE, GameReference.GetCamera());

//	Render->SetColor(255, 0, 0, 255);
//	Render->RenderDebugPointCamera(ParentCore.Pos.x, ParentCore.Pos.y);
}