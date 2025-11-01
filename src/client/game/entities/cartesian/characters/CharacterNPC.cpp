//
// Created by 11dpmbozko on 18.05.2023.
//

#include "CharacterNPC.h"
#include <cmath>

CharacterNPC::CharacterNPC(double max_health,
						   const Vec2f& start_pos,
						   const Vec2f& start_vel,
						   NPCType npc_type,
						   bool npc_is_boss)
	: Character(nullptr,
				max_health,
				start_pos,
				start_vel,
				true)
{
	last_attacker = nullptr;
	ai_type = npc_type;
	is_boss = npc_is_boss;
	color_hue = is_boss ? 250.0 + (rand() % 90) - 45 : 120.0 + (rand() % 90) - 45;
}

CharacterNPC::~CharacterNPC() = default;

void CharacterNPC::EventDeath()
{
	Character::EventDeath();

	world->AddScore(is_boss ? 20 * 5 : 20);

	Character *killer = (Character *)health_component.GetDamager();
	Player *KillerPlayer = killer->GetPlayer();
	if (KillerPlayer)
		world->EnemyKilled(KillerPlayer, this);

	Randomizer *random = Application.GetRandomizer();
	if (random->Float() <= 0.2f)
	{
		Crate* new_crate = new Crate(core.pos, DropType(random->Int() % NUM_DROP_TYPES));
		world->AddEntity(new_crate, true);
	}

	int num_npcs_alive = 0;
	for (Entity *entity : world->GetEntitiesByType(ENTITY_CHARACTER))
	{
		auto character = (Character *)entity;
		if (character->IsNPC() && character->IsAlive())
			num_npcs_alive++;
	}

	if (num_npcs_alive == 0)
	{
		world->EnemiesKilled();
		for (Entity *entity : world->GetEntitiesByType(ENTITY_CHARACTER))
			((Character *)entity)->RemoveCombat();
	}
}

void CharacterNPC::UpdateAttacker(Player *new_attacker)
{
	last_attacker = new_attacker;
}

void CharacterNPC::TickControls()
{
	if (ai_type == NPC_DUMMY)
		return;

	auto CurrentTick = world->GetTick();

	Character *ClosestChar = nullptr;
	double Closest = -1;
	for (Entity *entity : world->GetEntitiesByType(ENTITY_CHARACTER))
	{
		auto character = (Character *)entity;
		if (character == this || character->IsNPC())
			continue;

		auto& core = character->GetDirectionalCore();
		float distance = DistanceVec2f(core.pos, core.pos);
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
		Vec2f travel_direction = (ClosestCore.pos - core.pos).Normalize();
		input.going_direction = travel_direction * (current_weapon ? 1.0f : 0.5f);
		input.looking_direction = travel_direction;

		input.reloading = false;

		if (!current_weapon)
		{
			input.next_item = true;
			if (CurrentTick - hands.LastFisted() > 20)
				input.shooting = true;
		}
		else
		{
			if (Closest <= 300.0)
				input.going_direction = Vec2f(0.0f, 0.0f);

			if (!current_weapon->GetMagAmmo())
			{
				if (current_weapon->GetTrueAmmo())
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
				if (current_weapon->IsAutomatic() || CurrentTick - current_weapon->LastShot() > (unsigned long long)((double(current_weapon->TickCooldown()) - 1500.0 / Closest) * 10))
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