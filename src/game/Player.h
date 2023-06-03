//
// Created by T450s on 5/4/2023.
//

#ifndef TRIALANDERROR_PLAYER_H
#define TRIALANDERROR_PLAYER_H

#include "../GameWorld.h"
#include <string>

enum {
    MAX_USERNAME_LENGTH = 32
};

class Character;
class Player {
private:
    friend class GameWorld;
    GameWorld* m_GameWorld;
    Character* m_Character;
    std::string m_Username;
    unsigned int m_Index;
    TextSurface* m_NamePlate;

    Player* m_Prev, * m_Next;

public:
    Player(GameWorld* game_world, const std::string& username);
    ~Player();

    // Getting
    [[nodiscard]] Character* GetCharacter() const { return m_Character; }
    [[nodiscard]] std::string GetUsername() const { return m_Username; }
    [[nodiscard]] unsigned int GetIndex() const { return m_Index; }
    [[nodiscard]] TextSurface* GetNamePlate() const { return m_NamePlate; }

    // Setting
    void SetCharacter(Character* character);
    void SetUsername(const std::string& username);
};

#endif //TRIALANDERROR_PLAYER_H
