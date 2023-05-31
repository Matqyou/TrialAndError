//
// Created by Matq on 22/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAMEWORLD_H_
#define TRIALANDERROR_SRC_GAMEWORLD_H_

#include "GameReference.h"
#include "technical stuff/GameControllers.h"
#include "game/indicators/TextSurface.h"
#include "game/collision/TileMap.h"

class Player;
class Entity;
class Character;
class Crates;
class GameWorld {
public:
    enum EntityType {
        ENTTYPE_ERROR,
        ENTTYPE_BOX,
        ENTTYPE_AMMO,
        ENTTYPE_BULLET,
        ENTTYPE_CHARACTER,
        NUM_ENTTYPES
    };

private:
    GameReference* m_GameWindow;
    TileMap* m_Tiles;
    double m_Width, m_Height;
    double m_ShowNamesVisibility;
    bool m_ShowNames;
    bool m_Paused;
    Player* m_FirstPlayer, *m_LastPlayer;
    Entity* m_FirstType[NUM_ENTTYPES], *m_LastType[NUM_ENTTYPES];
    Entity* m_First, *m_Last;
    double m_x, m_y;
    double m_xLast ,m_yLast;
    unsigned long long m_CurrentTick;

    TextSurface* m_CoordinatePlate;

    // Cool scrolling background                                        cap
    Texture* m_Background;
    int m_BackgroundW{}, m_BackgroundH{};

    unsigned long long m_LastWave;
    unsigned long long m_TimeBetweenWaves;
    unsigned int m_NumEnemiesPerWave;
    unsigned int m_Round;

    void TickCamera();
    void TickSpawner();
    void TickEntities();
    void TickDestroy();

public:
    GameWorld(GameReference* game_window, int width, int height);
    ~GameWorld();
    static Texture* Chad;
    GameReference* GameWindow() const { return m_GameWindow; }
    double Width() const { return m_Width; }
    double Height() const { return m_Height; }
    double NamesShown() const { return m_ShowNamesVisibility; }
    bool Paused() const { return m_Paused; }
    double CameraX() const { return m_x; }
    double CameraY() const { return m_y; }
    unsigned long long CurrentTick() const { return m_CurrentTick; }
    unsigned int NextPlayerIndex() const;
    void GetPointInWorld(double relative_x, double relative_y, double& out_x, double& out_y) const;

    void EnemiesKilled();
    void SetCameraPos(double x, double y);  // Move the camera to a position
    Player* AddPlayer(Player* player);
    void RemovePlayer(Player* player);
    Entity* AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);
    void DestroyPlayerByController(GameController* DeletedController) const;
    void DestroyCharacterByController(GameController* DeletedController) const;
    void ToggleShowNames();
    void SetPaused(bool state);
    Entity* FirstEntity() const { return m_First; }
    Entity* LastEntity() const { return m_Last; }
    Entity* FirstEntityType(EntityType type) const { return m_FirstType[type]; }
    Entity* LastEntityType(EntityType type) const { return m_LastType[type]; }

    Character* FirstPlayer() const { return (Character*)(FirstEntityType(ENTTYPE_CHARACTER)); }
    Character* LastPlayer() const { return (Character*)(LastEntityType(ENTTYPE_CHARACTER)); }
    Crates* FirstCrate() const {return (Crates*)(FirstEntityType(ENTTYPE_BOX));}
    Crates* lASTCrate() const {return (Crates*)(LastEntityType(ENTTYPE_BOX));}
    void Event(const SDL_Event& currentEvent);
    void Tick();
    void Draw();
};

#endif //TRIALANDERROR_SRC_GAMEWORLD_H_
