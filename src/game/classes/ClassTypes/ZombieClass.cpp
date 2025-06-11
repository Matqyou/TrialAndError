#include "ZombieClass.h"
#include "../../Player.h"

void ZombieClass::ApplyPrimaryClassBonuses(Player *player)
{
    player->SetExtraLife(true);         // One-time revive
    player->IncreaseMaxHealthAmp(0.15); // +15% health
}

void ZombieClass::ApplySecondaryClassBonuses(Player* player) {
    player->IncreaseMaxHealthAmp(0.3);    // +30% health
    player->SetBaseDamage(player->GetBaseDamage() - 5); // slightly lower base dmg
}

void ZombieClass::LevelupStats(Player* player) {
    player->IncreaseDamageAmp(0.05);    // +5% extra damage per level
}
