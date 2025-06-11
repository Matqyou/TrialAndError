#pragma once

#include <string>

class Player;

class PlayerClass
{
public:

    virtual ~PlayerClass() = default;
    virtual std::string GetName() const = 0;
    virtual void ApplyPrimaryClassBonuses(Player *player) = 0;
    virtual void ApplySecondaryClassBonuses(Player *player) = 0;
    virtual void LevelupStats(Player *player) {}

    static PlayerClass *CreateDefaultClass();
    static PlayerClass *FromString(const std::string &name);
};
