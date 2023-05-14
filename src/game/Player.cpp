//
// Created by T450s on 5/4/2023.
//

#include "Player.h"
#include "entities/character/Character.h"

Player::Player(GameWorld* game_world, const std::string& username)
: m_GameWorld(game_world),
  m_Character(nullptr),
  m_Username(),
  m_Prev(nullptr),
  m_Next(nullptr) {
    SetUsername(username);
    m_Index = -1;
    m_Index = m_GameWorld->NextPlayerIndex();

    m_NamePlate = new TextSurface(m_GameWorld->GameWindow()->Assets(),
                                  m_GameWorld->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                  m_Username, { 255, 255, 255, 255 });

    m_GameWorld->AddPlayer(this);
}

Player::~Player() {
    delete m_NamePlate;
    m_GameWorld->RemovePlayer(this);
}

void Player::SetCharacter(Character* character) {
    if (m_Character) std::printf("WARNING: Overwriting existing character for player `%s`\n", m_Username.c_str());
    m_Character = character;
}

// If username is longer than the limit,
// it will use a substring of it with
// the length of MAX_USERNAME_LENGTH.
void Player::SetUsername(const std::string& username) {
    if (username.length() > MAX_USERNAME_LENGTH) { m_Username = username.substr(0, MAX_USERNAME_LENGTH); }
    else { m_Username = username; }
}