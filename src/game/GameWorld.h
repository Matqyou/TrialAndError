//
// Created by Matq on 22/03/2023.
//

#pragma once

#include "../GameReference.h"
#include "../technical stuff/GameControllers.h"
#include "indicators/TextSurface.h"
#include "collision/TileMap.h"
#include "../Protocol.h"

class Player;
class Entity;
class Character;
class Crate;
class GameWorld {
private:
    GameReference* m_GameWindow;
    TileMap* m_Tiles;
    double m_Width, m_Height;
    double m_ShowNamesVisibility;
    bool m_ShowNames;
    bool m_Paused;
    bool m_GameOver;
    Player* m_FirstPlayer, * m_LastPlayer;
    Entity* m_FirstType[NUM_ENTTYPES]{ }, * m_LastType[NUM_ENTTYPES]{ };
    Entity* m_First, * m_Last;
    unsigned long long m_CurrentTick;

    // Cool scrolling background                                        cap
    Texture* m_Background;
    int m_BackgroundW{ }, m_BackgroundH{ };

    unsigned long long m_LastWave;
    unsigned long long m_TimeBetweenWaves;
    unsigned int m_NumEnemiesPerWave;
    unsigned int m_Round;
    unsigned int m_Score;
    TextSurface* m_ScoreText;

    void TickCamera();
    void TickSpawner();
    void TickEntities();
    void TickDestroy();

public:
    GameWorld(GameReference* game_window, int width, int height);
    ~GameWorld();

    // Getting
    [[nodiscard]] GameReference* GameWindow() const { return m_GameWindow; }
    [[nodiscard]] double GetWidth() const { return m_Width; }
    [[nodiscard]] double GetHeight() const { return m_Height; }
    [[nodiscard]] double GetNamesShown() const { return m_ShowNamesVisibility < 0.1 ? 0.0 : m_ShowNamesVisibility; }
    [[nodiscard]] bool GetPaused() const { return m_Paused; }
    [[nodiscard]] bool GameOver() const { return m_GameOver; }
    [[nodiscard]] unsigned long long GetTick() const { return m_CurrentTick; }
    [[nodiscard]] unsigned int GetNextPlayerIndex() const;
    [[nodiscard]] Entity* FirstEntity() const { return m_First; }
    [[nodiscard]] Entity* LastEntity() const { return m_Last; }
    [[nodiscard]] Entity* FirstEntityType(EntityType entity_type) const { return m_FirstType[entity_type]; }
    [[nodiscard]] Entity* LastEntityType(EntityType entity_type) const { return m_LastType[entity_type]; }
    [[nodiscard]] Character* FirstCharacter() const { return (Character*)(FirstEntityType(ENTTYPE_CHARACTER)); }
    [[nodiscard]] Crate* FirstCrate() const { return (Crate*)(FirstEntityType(ENTTYPE_CRATE)); }
    // void GetPointInWorld(double relative_x, double relative_y, double& out_x, double& out_y) const;

    // Setting
    void SetPaused(bool state) { m_Paused = state; }

    // Events
    void EnemiesKilled();
    void AlliesGone();

    // Manipulating
    void AddScore(unsigned int score);
    void DestroyPlayerByController(GameController* DeletedController) const;
    void DestroyCharacterByController(GameController* DeletedController) const;
    void ToggleShowNames();

    // Managing
    Player* AddPlayer(Player* player);
    Entity* AddEntity(Entity* entity);
    void RemovePlayer(Player* player);
    void RemoveEntity(Entity* entity);

    // Listening & Ticking
    void Event(const SDL_Event& currentEvent);
    void Tick();
    void Draw();
};
