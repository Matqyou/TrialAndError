#include "VampireClass.h"
#include <client/game/players/Player.h>

VampireClass::VampireClass()
	: PlayerClass(PLAYERCLASS_VAMPIRE)
{

}

void VampireClass::ApplyPrimaryClassBonuses(Player *player)
{
	player->SetExtraLife(true);         // One-time revive
	player->IncreaseMaxHealthAmp(0.15); // +15% health
}

void VampireClass::ApplySecondaryClassBonuses(Player *player)
{
	player->SetExtraLife(true);              // One-time revive
	player->IncreaseDamageAmp(0.15);      // +15% damage
}

void VampireClass::LevelupStats(Player *player)
{
	player->IncreaseDamageAmp(0.05);    // +5% extra damage per level
}
