//
// Created by T450s on 5/4/2023.
//

#pragma once

#include "GameWorld.h"
#include "./interface/LevelUpMenu.h"
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
    unsigned int m_XP;
    unsigned int m_Level;
    friend class GameWorld;
    GameWorld *m_GameWorld;
    Character *m_Character;
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
    Player *m_Prev, *m_Next;

public:
    Player(GameWorld *game_world, const std::string &username);
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
    void SetUsername(const std::string &username);
    void SetLevelUpMenuQueue(std::queue<LevelUpMenu *> MenuQueue) { m_levelUpMenuQueue = MenuQueue; };

    // XP and Leveling
    void GainXP(unsigned int amount);
    void LevelUp();
};
