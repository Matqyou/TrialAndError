//
// Created by 11dpmbozko on 18.05.2023.
//

#pragma once

#include <client/game/entities/cartesian/characters/character/Character.h>

enum NPCType
{
	NPC_DUMMY,
	NPC_TURRET
};

class CharacterNPC : public Character
{
protected:
	bool is_boss;
	Player *last_attacker;
	NPCType ai_type;

	// Listening & Ticking
	void EventDeath() override;
	void TickControls() override;

public:
	CharacterNPC(double max_health,
				 const Vec3f& start_pos,
				 const Vec3f& start_vel,
				 NPCType npc_type,
				 bool npc_is_boss);
	~CharacterNPC() override;

	// Getting
	[[nodiscard]] bool IsBoss() const { return is_boss; }
	[[nodiscard]] const char *toString() const override;

	// Manipulating
	void UpdateAttacker(Player *new_attacker);

};
