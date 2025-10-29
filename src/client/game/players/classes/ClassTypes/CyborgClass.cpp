#include "CyborgClass.h"
#include <client/game/players/Player.h>

CyborgClass::CyborgClass()
	: PlayerClass(PLAYERCLASS_CYBORG)
{

}

void CyborgClass::ApplyPrimaryClassBonuses(Player *player)
{
	player->SetExtraLife(true);         // One-time revive
	player->IncreaseMaxHealthAmp(0.15); // +15% health
}

void CyborgClass::ApplySecondaryClassBonuses(Player *player)
{
	player->IncreaseDamageAmp(0.2);     // +20% base damage
	player->IncreaseMaxHealthAmp(0.1);  // +10% health
}

void CyborgClass::LevelupStats(Player *player)
{
	player->IncreaseDamageAmp(0.05);    // +5% extra damage per level
}
