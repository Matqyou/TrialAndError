//
// Created by Matq on 22/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAMEWORLD_H_
#define TRIALANDERROR_SRC_GAMEWORLD_H_

#include "GameReference.h"
#include "technical stuff/GameControllers.h"
#include "game/indicators/TextSurface.h"
#include "game/collision/TileMap.h"

class Entity;
class Character;
class GameWorld {
public:
    enum EntityType {
        ENTTYPE_CHARACTER,
        ENTTYPE_BULLET,
        NUM_ENTTYPES
    };

private:
    GameReference* m_GameWindow;
    TileMap* m_Tiles;
    double m_Width, m_Height;  // maybe better if it was int
    double m_ShowNamesVisibility;
    bool m_ShowNames;
    bool m_Paused;
    Entity* m_FirstType[NUM_ENTTYPES] {}, *m_LastType[NUM_ENTTYPES] {};
    Entity* m_First, *m_Last;
    double m_x, m_y;
    unsigned long long m_CurrentTick;

    TextSurface* m_CoordinatePlate;

    // Cool scrolling background                                        cap
    Texture* m_Background;
    int m_BackgroundW{}, m_BackgroundH{};

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
    Character* GetPlayerByIndex(int index) const;
    void GetNextPlayerIndex(Character* player) const;
    void GetPointInWorld(double relative_x, double relative_y, double& out_x, double& out_y) const;

    void SetCameraPos(double x, double y);  // Move the camera to a position
    void AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);
    void DestroyPlayerByController(GameController* DeletedController) const;
    void ToggleShowNames();
    void SetPaused(bool state);
    Entity* FirstEntity() const { return m_First; }
    Entity* LastEntity() const { return m_Last; }
    Entity* FirstEntityType(EntityType type) const { return m_FirstType[type]; }
    Entity* LastEntityType(EntityType type) const { return m_LastType[type]; }

    Character* FirstPlayer() const { return (Character*)(FirstEntityType(ENTTYPE_CHARACTER)); }
    Character* LastPlayer() const { return (Character*)(LastEntityType(ENTTYPE_CHARACTER)); }

    void Event(const SDL_Event& currentEvent);
    void Tick();
    void Draw();
};

#endif //TRIALANDERROR_SRC_GAMEWORLD_H_
