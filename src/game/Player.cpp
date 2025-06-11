//
// Created by T450s on 5/4/2023.
//

#include "Player.h"
#include "entities/characters/character/Character.h"

Player::Player(GameWorld *game_world, const std::string &username, PlayerClass *primaryClass)
    : m_GameWorld(game_world),
      m_Character(nullptr),
      m_Username(),
      m_XP(0),
      m_Level(1),
      m_Prev(nullptr),
      m_Next(nullptr),
      m_levelUpMenuQueue(),
      m_Class(primaryClass)
{
    m_BossDamageAmp = 1;
    m_BaseDamage = 10;
    m_DamageAmp = 1;
    m_MaxHealthAmp = 1;
    m_ExtraLife = false;
    SetUsername(username);
    m_LevelUpMenu = new LevelUpMenu(m_GameWorld, this);
    m_Index = -1;
    m_Index = m_GameWorld->GetNextPlayerIndex();
    m_NamePlate = new TextSurface(m_GameWorld->GameWindow()->Assetz(),
                                  m_GameWorld->GameWindow()->Assetz()->TextHandler()->GetMainFont(),
                                  m_Username, {255, 255, 255, 255});
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
        m_levelUpMenuQueue.push(this->GetLevelUpMenu());
        m_Class->LevelupStats(this);
        m_Character->LevelupStats(m_Level);
    }
    m_BaseDamage += 1;
}

void Player::AddPowerupUpgrade(const std::string &name)
{
    m_UpgradeCounts[name]++;
}

int Player::GetPowerupUpgradeCount(const std::string &name)
{
    auto it = m_UpgradeCounts.find(name);
    return it != m_UpgradeCounts.end() ? it->second : 0;
}

void Player::SetCharacter(Character *character)
{
    if (m_Character)
        std::printf("Player `%s` already has a characters..\n", m_Username.c_str());
    else
        m_Character = character;
}

// If username is longer than the limit,
// it will use a substring of it with
// the length of MAX_USERNAME_LENGTH.
void Player::SetUsername(const std::string &username)
{
    if (username.length() > MAX_USERNAME_LENGTH)
    {
        m_Username = username.substr(0, MAX_USERNAME_LENGTH);
    }
    else
    {
        std::cout << FStringColors("[Player] &8Setting username to: &f") << m_Class->GetName() << std::endl;
        m_Username = m_Class->GetName() + " - " + username;
    }
}