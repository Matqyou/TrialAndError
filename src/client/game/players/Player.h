//
// Created by T450s on 5/4/2023.
//

#pragma once

#include <client/game/world/GameWorld.h>
#include <client/game/interface/LevelUpMenu.h>
#include <client/game/players/classes/PlayerClass.h>
#include <string>
#include <unordered_map>
#include <queue>

enum
{
	MAX_USERNAME_LENGTH = 32
};

class Character;
class Player
{
private:
	unsigned int m_XP;
	unsigned int m_Level;
	friend class GameWorld;
	GameWorld *m_GameWorld;
	Character *m_Character;
	PlayerClass *m_Class;
	PlayerClass *m_SecondaryClass;
	LevelUpMenu *m_LevelUpMenu;
	std::string m_Username;
	unsigned int m_Index;

	int m_BaseDamage;
	double m_DamageAmp;
	double m_BossDamageAmp;
	double m_MaxHealthAmp;
	bool m_ExtraLife;
	TextSurface *m_NamePlate;
	std::queue<LevelUpMenu *> m_levelUpMenuQueue;
	std::unordered_map<std::string, int> m_UpgradeCounts;
	Player *m_Prev, *m_Next;

public:
	// If assignDefaultClass is true and primaryClass is nullptr, a default class will be assigned.
	// If assignDefaultClass is false, the player's class will remain nullptr until chosen later.
	Player(GameWorld *game_world, const std::string& username, PlayerClass *primaryClass = nullptr, bool assignDefaultClass = true);
	~Player();

	// Getting
	[[nodiscard]] Character *GetCharacter() const { return m_Character; }
	[[nodiscard]] std::string GetUsername() const { return m_Username; }
	[[nodiscard]] unsigned int GetIndex() const { return m_Index; }
	[[nodiscard]] Player *Next() const { return m_Next; }
	[[nodiscard]] Player *Prev() const { return m_Prev; }
	[[nodiscard]] LevelUpMenu *GetLevelUpMenu() const { return m_LevelUpMenu; }
	[[nodiscard]] TextSurface *GetNamePlate() const { return m_NamePlate; }
	[[nodiscard]] std::queue<LevelUpMenu *> GetLevelUpMenuQueue() const { return m_levelUpMenuQueue; }
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
	void SetGameWorld(GameWorld *game_world) { m_GameWorld = game_world; }
	void SetLevelUpMenuQueue(std::queue<LevelUpMenu *> MenuQueue) { m_levelUpMenuQueue = MenuQueue; };

	// XP and Leveling
	void GainXP(unsigned int amount);
	void LevelUp();

	void AddPowerupUpgrade(const std::string& name);

	int GetPowerupUpgradeCount(const std::string& name);
};
