//
// Created by T450s on 5/4/2023.
//

#pragma once

#include "GameWorld.h"
#include <string>

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
    std::string m_Username;
    unsigned int m_Index;
    TextSurface *m_NamePlate;

    Player *m_Prev, *m_Next;

public:
    Player(GameWorld *game_world, const std::string &username);
    ~Player();

    // Getting
    [[nodiscard]] Character *GetCharacter() const { return m_Character; }
    [[nodiscard]] std::string GetUsername() const { return m_Username; }
    [[nodiscard]] unsigned int GetIndex() const { return m_Index; }
    [[nodiscard]] TextSurface *GetNamePlate() const { return m_NamePlate; }
    [[nodiscard]] unsigned int GetXP() const { return m_XP; }
    [[nodiscard]] unsigned int GetLevel() const { return m_Level; }

    // Setting
    void SetCharacter(Character *character);
    void SetUsername(const std::string &username);

    // XP and Leveling
    void GainXP(unsigned int amount);
    void LevelUp();
};
