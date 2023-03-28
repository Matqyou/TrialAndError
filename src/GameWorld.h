//
// Created by Matq on 22/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAMEWORLD_H_
#define TRIALANDERROR_SRC_GAMEWORLD_H_

#include "GameReference.h"
#include "technical stuff/GameControllers.h"

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
    double m_Width, m_Height;  // maybe better if it was int
    double m_ShowNames{};
    bool m_Paused;
    Entity* m_LastEntityType[NUM_ENTTYPES]{};
    Entity* m_LastEntity;
    double m_x, m_y;

public:
    GameWorld(GameReference* gameWindow, double width, double height);
    ~GameWorld();

    GameReference* GameWindow() const { return m_GameWindow; }
    double Width() const { return m_Width; }
    double Height() const { return m_Height; }
    double NamesShown() const { return m_ShowNames; }
    bool Paused() const { return m_Paused; }
    Character* GetPlayerByIndex(int index);
    void GetNextPlayerIndex(Character* player);
    double CameraX() const { return m_x; }
    double CameraY() const { return m_y; }
    void GetPointInWorld(double relative_x, double relative_y, double& out_x, double& out_y) const;

    void SetCameraPos(double x, double y);  // Move the camera to a position
    void AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);
    void DestroyPlayerByController(GameController* DeletedController);
    void ShowNames();
    void SetPaused(bool state);

    void Event(const SDL_Event& currentEvent);
    void Tick();
    void Draw();
};

#endif //TRIALANDERROR_SRC_GAMEWORLD_H_
