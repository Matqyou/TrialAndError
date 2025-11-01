//
// Created by T450s on 5/4/2023.
//

#include "Player.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include "client/game/ui/CommonUI.h"

Player::Player(const std::string& username, PlayerClass *player_class)
	: character(nullptr),
	  username(),
	  xp(0),
	  level(1),
	  player_class(player_class ? player_class : PlayerClass::CreateClass(PLAYERCLASS_HUMAN))
{
	boss_damage_amplifier = 1;
	base_damage = 10;
	damage_amplifier = 1;
	max_health_amplifier = 1;
	extra_life = false;
	SetUsername(username);
	player_id = -1;
	player_id = GameReference.NextPlayerID();
	nameplate = new TextSurface(CommonUI::fontDefault, username, { 255, 255, 255, 255 });
//	world->AddPlayer(this);
	GameReference.AddPlayer(this);
}

Player::~Player()
{
	delete nameplate;
//	world->RemovePlayer(this);
}

void Player::GainXP(unsigned int amount)
{
	xp += amount;
	unsigned int m_xpForNextLevel = level * 100;
	while (xp >= m_xpForNextLevel)
	{
		xp -= m_xpForNextLevel;
		m_xpForNextLevel = level * 100;
		LevelUp();
	}
}

void Player::LevelUp()
{
	level++;
	if (character)
	{
//		m_levelUpMenuQueue.push(this->GetLevelUpMenu());
		player_class->LevelupStats(this);
		character->LevelupStats(level);
	}
	base_damage += 1;
}

void Player::AddPowerupUpgrade(Powerup type, int times)
{
	// Cast to size_t because Powerup is an enum class
	auto type_index = static_cast<size_t>(type);
	upgrade_counts[type_index] += times;
}

int Player::GetPowerupUpgradeCount(Powerup type)
{
	// Cast to size_t because Powerup is an enum class
	auto type_index = static_cast<size_t>(type);
	return upgrade_counts[type_index];
}

void Player::SetCharacter(Character *new_character)
{
	if (character && new_character)
		std::printf("Player `%s` already has a character.\n", username.c_str());
	else
		character = new_character;
}

// If username is longer than the limit,
// it will use a substring of it with
// the length of MAX_USERNAME_LENGTH.
void Player::SetUsername(const std::string& new_username)
{
	if (new_username.length() > MAX_USERNAME_LENGTH)
	{
		username = new_username.substr(0, MAX_USERNAME_LENGTH);
	}
	else
	{
		// TODO: something not makin sense here
		std::cout << FStringColors("[Player] &8Setting username to: &f") << player_class->GetName() << std::endl;
		username = Strings::FString("%s - %s", player_class->GetName(), username.c_str());
	}
}