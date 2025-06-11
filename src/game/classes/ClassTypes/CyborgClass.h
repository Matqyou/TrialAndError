#pragma once
#include "../PlayerClass.h"

class CyborgClass : public PlayerClass
{
public:
    std::string GetName() const override { return "Cyborg"; }

    void ApplyPrimaryClassBonuses(Player *player) override;
    void ApplySecondaryClassBonuses(Player *player) override;
    void LevelupStats(Player *player) override;
};
