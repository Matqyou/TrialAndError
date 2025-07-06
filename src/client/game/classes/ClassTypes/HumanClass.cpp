#include "HumanClass.h"
#include "../../Player.h"

HumanClass::HumanClass()
	: PlayerClass(PLAYERCLASS_HUMAN)
{

}

void HumanClass::ApplyPrimaryClassBonuses(Player *player)
{
	player->SetExtraLife(true);         // One-time revive
	player->IncreaseMaxHealthAmp(0.15); // +15% health
}

void HumanClass::ApplySecondaryClassBonuses(Player *player)
{
	player->SetExtraLife(true);         // One-time revive
	player->IncreaseMaxHealthAmp(0.15); // +15% health
}

void HumanClass::LevelupStats(Player *player)
{
	player->IncreaseDamageAmp(0.05);    // +5% extra damage per level
}
