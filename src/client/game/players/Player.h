//
// Created by T450s on 5/4/2023.
//

#pragma once

#include <client/game/players/classes/PlayerClass.h>
//#include <client/game/interface/LevelUpMenu.h>
#include "client/game/powerups/PowerupManager.h"
#include <client/game/world/GameWorld.h>

#include <unordered_map>
#include <string>
#include <string>
#include <queue>

enum
{
	MAX_USERNAME_LENGTH = 32
};

class Character;
class Player
{
private:
	friend class GameWorld;
	unsigned int m_XP;
	unsigned int m_Level;

	GameWorld *m_GameWorld;
	Character *m_Character;
	PlayerClass *m_Class;
//	LevelupMenu *m_LevelUpMenu;
	std::string m_Username;
	unsigned int m_Index;

	int m_BaseDamage;
	double m_DamageAmp;
	double m_BossDamageAmp;
	double m_MaxHealthAmp;
	bool m_ExtraLife;
	TextSurface *m_NamePlate;
//	std::queue<LevelUpMenu *> m_levelUpMenuQueue;
	int m_UpgradeCounts[(size_t)Powerup::NUM_POWERUPS];

public:
	Player(GameWorld *game_world, const std::string& username, PlayerClass *player_class = nullptr);
	~Player();

	// Getting
	[[nodiscard]] Character *GetCharacter() const { return m_Character; }
	[[nodiscard]] std::string GetUsername() const { return m_Username; }
	[[nodiscard]] unsigned int GetIndex() const { return m_Index; }
//	[[nodiscard]] LevelupMenu *GetLevelUpMenu() const { return m_LevelUpMenu; }
	[[nodiscard]] TextSurface *GetNamePlate() const { return m_NamePlate; }
//	[[nodiscard]] std::queue<LevelUpMenu *> GetLevelUpMenuQueue() const { return m_levelUpMenuQueue; }
	[[nodiscard]] unsigned int GetXP() const { return m_XP; }
	[[nodiscard]] int GetBaseDamage() const { return m_BaseDamage; }
	[[nodiscard]] double GetDamageAmp() const { return m_DamageAmp; }
	[[nodiscard]] double GetBossDamageAmp() const { return m_BossDamageAmp; }
	[[nodiscard]] double GetMaxHealthAmp() const { return m_MaxHealthAmp; }
	[[nodiscard]] bool GetExtraLifeStatus() const { return m_ExtraLife; }

	[[nodiscard]] unsigned int GetLevel() const { return m_Level; }

	// Setting
	void SetBaseDamage(int BaseDamage) { m_BaseDamage = BaseDamage; };
	void IncreaseDamageAmp(double DamageAmp) { m_DamageAmp += DamageAmp; };
	void IncreaseBossDamageAmp(double DamageAmp) { m_BossDamageAmp += DamageAmp; };
	void IncreaseMaxHealthAmp(double HealthAmp) { m_MaxHealthAmp += HealthAmp; };
	void SetExtraLife(bool ExtraLife) { m_ExtraLife = ExtraLife; };
	void SetCharacter(Character *character);
	void SetUsername(const std::string& username);
//	void SetLevelUpMenuQueue(std::queue<LevelUpMenu *> MenuQueue) { m_levelUpMenuQueue = MenuQueue; };
	void SetGameWorld(GameWorld *game_world) { m_GameWorld = game_world; }
	// XP and Leveling
	void GainXP(unsigned int amount);
	void LevelUp();

	void AddPowerupUpgrade(Powerup type, int times = 1);
	int GetPowerupUpgradeCount(Powerup type);

};
