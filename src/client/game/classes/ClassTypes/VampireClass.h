#pragma once
#include "../PlayerClass.h"

class VampireClass : public PlayerClass
{
public:
	VampireClass();
//	std::string GetName() const override { return "Vampire"; }

	void ApplyPrimaryClassBonuses(Player *player) override;
	void ApplySecondaryClassBonuses(Player *player) override;
	void LevelupStats(Player *player) override;
};
