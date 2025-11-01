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
	unsigned int xp;
	unsigned int level;

	Character *character;
	PlayerClass *player_class;
//	LevelupMenu *m_LevelUpMenu;
	std::string username;
	size_t player_id;

	int base_damage;
	double damage_amplifier;
	double boss_damage_amplifier;
	double max_health_amplifier;
	bool extra_life;
	TextSurface *nameplate;
//	std::queue<LevelUpMenu *> m_levelUpMenuQueue;
	int upgrade_counts[(size_t)Powerup::NUM_POWERUPS];

public:
	explicit Player(const std::string& username, PlayerClass *player_class = nullptr);
	~Player();

	// Getting
	[[nodiscard]] Character *GetCharacter() const { return character; }
	[[nodiscard]] const std::string& GetUsername() const { return username; }
	[[nodiscard]] size_t GetPlayerID() const { return player_id; }
//	[[nodiscard]] LevelupMenu *GetLevelUpMenu() const { return m_LevelUpMenu; }
	[[nodiscard]] TextSurface *GetNamePlate() const { return nameplate; }
//	[[nodiscard]] std::queue<LevelUpMenu *> GetLevelUpMenuQueue() const { return m_levelUpMenuQueue; }
	[[nodiscard]] unsigned int GetXP() const { return xp; }
	[[nodiscard]] int GetBaseDamage() const { return base_damage; }
	[[nodiscard]] double GetDamageAmp() const { return damage_amplifier; }
	[[nodiscard]] double GetBossDamageAmp() const { return boss_damage_amplifier; }
	[[nodiscard]] double GetMaxHealthAmp() const { return max_health_amplifier; }
	[[nodiscard]] bool GetExtraLifeStatus() const { return extra_life; }

	[[nodiscard]] unsigned int GetLevel() const { return level; }

	// Setting
	void SetBaseDamage(int BaseDamage) { base_damage = BaseDamage; };
	void IncreaseDamageAmp(double DamageAmp) { damage_amplifier += DamageAmp; };
	void IncreaseBossDamageAmp(double DamageAmp) { boss_damage_amplifier += DamageAmp; };
	void IncreaseMaxHealthAmp(double HealthAmp) { max_health_amplifier += HealthAmp; };
	void SetExtraLife(bool ExtraLife) { extra_life = ExtraLife; };
	void SetCharacter(Character *character);
	void SetUsername(const std::string& username);
//	void SetLevelUpMenuQueue(std::queue<LevelUpMenu *> MenuQueue) { m_levelUpMenuQueue = MenuQueue; };
//	void SetGameWorld(GameWorld *game_world) { world = game_world; }
	// XP and Leveling
	void GainXP(unsigned int amount);
	void LevelUp();

	void AddPowerupUpgrade(Powerup type, int times = 1);
	int GetPowerupUpgradeCount(Powerup type);

};
