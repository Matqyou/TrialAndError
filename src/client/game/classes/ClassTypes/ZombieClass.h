#pragma once
#include "../PlayerClass.h"

class ZombieClass : public PlayerClass
{
public:
	std::string GetName() const override { return "Zombie"; }

	void ApplySecondaryClassBonuses(Player *player) override;
	void ApplyPrimaryClassBonuses(Player *player) override;
	void LevelupStats(Player *player) override;
};
