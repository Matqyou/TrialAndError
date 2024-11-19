//
// Created by T450s on 5/4/2023.
//

#include "Player.h"
#include "entities/characters/character/Character.h"

Player::Player(GameWorld* game_world, const std::string& username)
    : m_GameWorld(game_world),
      m_Character(nullptr),
      m_Username(),
      m_XP(0),
      m_Level(1),
      m_Prev(nullptr),
      m_Next(nullptr) {
    SetUsername(username);
    m_Index = -1;
    m_Index = m_GameWorld->GetNextPlayerIndex();
    m_NamePlate = new TextSurface(m_GameWorld->GameWindow()->Assets(),
                                  m_GameWorld->GameWindow()->Assets()->TextHandler()->GetMainFont(),
                                  m_Username, { 255, 255, 255, 255 });
    m_GameWorld->AddPlayer(this);
}


Player::~Player() {
    delete m_NamePlate;
    m_GameWorld->RemovePlayer(this);
}

void Player::GainXP(unsigned int amount) {
    m_XP += amount;
    // Check if the player should level up
    unsigned int xpForNextLevel = m_Level * 100; // Example XP requirement formula
    while (m_XP >= xpForNextLevel) {
        m_XP -= xpForNextLevel;
        LevelUp();
        xpForNextLevel = m_Level * 100;
    }
}


void Player::LevelUp() {
    std::cout << "Player `" << m_Username << "` leveled up to level " << m_Level + 1 << "!\n";
    m_Level++;
    // Update player stats here
    // Example: Increase health, damage, etc.
    if (m_Character) {
        m_Character->LevelupStats(m_Level); // Example method to increase health

    }
}

void Player::SetCharacter(Character* character) {
    if (m_Character) std::printf("Player `%s` already has a characters..\n", m_Username.c_str());
    else m_Character = character;
}

// If username is longer than the limit,
// it will use a substring of it with
// the length of MAX_USERNAME_LENGTH.
void Player::SetUsername(const std::string& username) {
    if (username.length() > MAX_USERNAME_LENGTH) { m_Username = username.substr(0, MAX_USERNAME_LENGTH); }
    else { m_Username = username; }
}