#pragma once
#include <client/game/players/classes/PlayerClass.h>

class VampireClass : public PlayerClass
{
public:
	std::string GetName() const override { return "Vampire"; }

	void ApplyPrimaryClassBonuses(Player *player) override;
	void ApplySecondaryClassBonuses(Player *player) override;
	void LevelupStats(Player *player) override;
};
