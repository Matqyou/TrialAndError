#pragma once
#include "../PlayerClass.h"

class HumanClass : public PlayerClass
{
public:
	HumanClass();
//	std::string GetName() const override { return "Human"; }

	void ApplyPrimaryClassBonuses(Player *player) override;
	void ApplySecondaryClassBonuses(Player *player) override;
	void LevelupStats(Player *player) override;

};
