#pragma once
#include <client/game/players/classes/PlayerClass.h>

class ZombieClass : public PlayerClass
{
public:
	ZombieClass();
//	std::string GetName() const override { return "Zombie"; }

	void ApplySecondaryClassBonuses(Player *player) override;
	void ApplyPrimaryClassBonuses(Player *player) override;
	void LevelupStats(Player *player) override;
};
