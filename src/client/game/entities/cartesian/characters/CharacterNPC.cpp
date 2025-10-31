//
// Created by 11dpmbozko on 18.05.2023.
//

#include "CharacterNPC.h"
#include <cmath>

CharacterNPC::CharacterNPC(GameWorld *world,
						   double max_health,
						   const Vec2f& start_pos,
						   const Vec2f& start_vel,
						   NPCType npc_type,
						   bool is_boss)
	: Character(world,
				nullptr,
				max_health,
				start_pos,
				start_vel,
				true)
{
	m_LastAttacker = nullptr;
	m_AIType = npc_type;
	m_IsBoss = is_boss;
	m_ColorHue = is_boss ? 250.0 + (rand() % 90) - 45 : 120.0 + (rand() % 90) - 45;
}

CharacterNPC::~CharacterNPC() = default;

void CharacterNPC::EventDeath()
{
	Character::EventDeath();

	m_World->AddScore(m_IsBoss ? 20 * 5 : 20);

	Character *killer = (Character *)m_HealthComponent.GetDamager();
	Player *KillerPlayer = killer->GetPlayer();
	if (KillerPlayer)
		m_World->EnemyKilled(KillerPlayer, this);

	if (rand() % 100 <= 20)
		new Crate(m_World, m_Core.pos, DropType(rand() % 2));

	int num_npcs_alive = 0;
	for (Entity *entity : m_World->GetEntitiesByType(CHARACTER_ENTITY))
	{
		auto character = (Character *)entity;
		if (character->IsNPC() && character->IsAlive())
			num_npcs_alive++;
	}

	if (num_npcs_alive == 0)
	{
		m_World->EnemiesKilled();
		for (Entity *entity : m_World->GetEntitiesByType(CHARACTER_ENTITY))
			((Character *)entity)->RemoveCombat();
	}
}

void CharacterNPC::UpdateAttacker(Player *attacker)
{
	m_LastAttacker = attacker;
}

void CharacterNPC::TickControls()
{
	if (m_AIType == NPC_DUMMY)
		return;

	auto CurrentTick = m_World->GetTick();

	Character *ClosestChar = nullptr;
	double Closest = -1;
	for (Entity *entity : m_World->GetEntitiesByType(CHARACTER_ENTITY))
	{
		auto character = (Character *)entity;
		if (character == this || character->IsNPC())
			continue;

		auto& core = character->GetDirectionalCore();
		float distance = DistanceVec2f(m_Core.pos, core.pos);
		if (distance < 1000.0 && (!ClosestChar || distance < Closest))
		{
			Closest = distance;
			ClosestChar = character;
		}
	}

	input.shooting = false;
	input.next_item = false;

	if (!ClosestChar)
	{
//		input.m_GoingLength = 0.0;
//		input.m_LookingLength = 1.0;
		const float one_degree = static_cast<float>(rand() % 10 - 5) / 180.0f * static_cast<float>(M_PI);
		float angle = input.looking_direction.Atan2F() + one_degree;
		input.looking_direction = FromAngleVec2f(angle);
//		double Angle = std::atan2(input.m_LookingY, input.m_LookingX) + OneDegree;
//		input.m_LookingX = cos(Angle);
//		input.m_LookingY = sin(Angle);
		input.shooting = false;
	}
	else
	{
		EntityCore& ClosestCore = ClosestChar->GetCore();
		Vec2f travel_direction = (ClosestCore.pos - m_Core.pos).Normalize();
		input.going_direction = travel_direction * (m_CurrentWeapon ? 1.0f : 0.5f);
		input.looking_direction = travel_direction;

		input.reloading = false;

		if (!m_CurrentWeapon)
		{
			input.next_item = true;
			if (CurrentTick - m_Hands.LastFisted() > 20)
				input.shooting = true;
		}
		else
		{
			if (Closest <= 300.0)
				input.going_direction = Vec2f(0.0f, 0.0f);

			if (!m_CurrentWeapon->GetMagAmmo())
			{
				if (m_CurrentWeapon->GetTrueAmmo())
				{
					input.reloading = true;
				}
				else
				{
					input.next_item = true;
				}
			}
			else
			{
				if (m_CurrentWeapon->IsAutomatic() || CurrentTick - m_CurrentWeapon->LastShot() > (unsigned long long)((double(m_CurrentWeapon->TickCooldown()) - 1500.0 / Closest) * 10))
				{
					input.shooting = true;
				}
			}
		}
	}
}

const char *CharacterNPC::toString() const
{
	return "CharacterNPC";
}