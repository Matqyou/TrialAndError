//
// Created by T450s on 5/4/2023.
//

#include "Player.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include "client/game/ui/CommonUI.h"

Player::Player(GameWorld *game_world, const std::string& username, PlayerClass *player_class)
	: m_GameWorld(game_world),
	  m_Character(nullptr),
	  m_Username(),
	  m_XP(0),
	  m_Level(1),
	  m_Class(player_class ? player_class : PlayerClass::CreateClass(PLAYERCLASS_HUMAN))
{
	m_BossDamageAmp = 1;
	m_BaseDamage = 10;
	m_DamageAmp = 1;
	m_MaxHealthAmp = 1;
	m_ExtraLife = false;
	SetUsername(username);
	m_Index = -1;
	m_Index = m_GameWorld->GetNextPlayerIndex();
	m_NamePlate = new TextSurface(CommonUI::fontDefault, m_Username, { 255, 255, 255, 255 });
	m_GameWorld->AddPlayer(this);
}

Player::~Player()
{
	delete m_NamePlate;
	m_GameWorld->RemovePlayer(this);
}

void Player::GainXP(unsigned int amount)
{
	m_XP += amount;
	unsigned int m_xpForNextLevel = m_Level * 100;
	while (m_XP >= m_xpForNextLevel)
	{
		m_XP -= m_xpForNextLevel;
		m_xpForNextLevel = m_Level * 100;
		LevelUp();
	}
}

void Player::LevelUp()
{
	m_Level++;
	if (m_Character)
	{
//		m_levelUpMenuQueue.push(this->GetLevelUpMenu());
		m_Class->LevelupStats(this);
		m_Character->LevelupStats(m_Level);
	}
	m_BaseDamage += 1;
}

void Player::AddPowerupUpgrade(Powerup type, int times)
{
	// Cast to size_t because Powerup is an enum class
	auto type_index = static_cast<size_t>(type);
	m_UpgradeCounts[type_index] += times;
}

int Player::GetPowerupUpgradeCount(Powerup type)
{
	// Cast to size_t because Powerup is an enum class
	auto type_index = static_cast<size_t>(type);
	return m_UpgradeCounts[type_index];
}

void Player::SetCharacter(Character *character)
{
	if (m_Character && character)
		std::printf("Player `%s` already has a character.\n", m_Username.c_str());
	else
		m_Character = character;
}

// If username is longer than the limit,
// it will use a substring of it with
// the length of MAX_USERNAME_LENGTH.
void Player::SetUsername(const std::string& username)
{
	if (username.length() > MAX_USERNAME_LENGTH)
	{
		m_Username = username.substr(0, MAX_USERNAME_LENGTH);
	}
	else
	{
		// TODO: something not makin sense here
		std::cout << FStringColors("[Player] &8Setting username to: &f") << m_Class->GetName() << std::endl;
		m_Username = Strings::FString("%s - %s", m_Class->GetName(), username.c_str());
	}
}