//
// Created by Matq on 22/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAMEWORLD_H_
#define TRIALANDERROR_SRC_GAMEWORLD_H_

#include "GameReference.h"
#include "technical stuff/GameControllers.h"

class Entity;
class GameWorld {
private:
    GameReference* m_GameWindow;
    double m_Width, m_Height;  // maybe better if it was int

    Entity* m_LastEntity;
    bool m_ShowNames;
    bool m_Paused;

public:
    enum EntityType {
        ENTTYPE_CHARACTER,
        ENTTYPE_BULLET,
        NUM_ENTTYPES
    };

    GameWorld(GameReference* gameWindow, double width, double height);
    ~GameWorld();

    GameReference* GameWindow() const { return m_GameWindow; }
    double Width() const { return m_Width; }
    double Height() const { return m_Height; }
    bool NamesShown() const { return m_ShowNames; }
    bool Paused() const { return m_Paused; }
    int NextPlayerIndex();

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
