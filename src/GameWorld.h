//
// Created by Matq on 22/03/2023.
//

#ifndef TRIALANDERROR_SRC_GAMEWORLD_H_
#define TRIALANDERROR_SRC_GAMEWORLD_H_

#include "GameReference.h"

class Entity;
class GameWorld {
private:
    GameReference* m_GameWindow;
    double m_Width, m_Height;  // maybe better if it was int

    std::vector<Entity*> m_Entities;

public:
    enum EntityType {
        ENTTYPE_CHARACTER,
        ENTTYPE_BULLET,
        NUM_ENTTYPES
    };

    GameWorld(GameReference* gameWindow, double width, double height);
    ~GameWorld();

    GameReference* GameWindow() const { return m_GameWindow; }
    std::vector<Entity*> Entities() const { return m_Entities; }
    double Width() const { return m_Width; }
    double Height() const { return m_Height; }

    void AddEntity(Entity* entity);
    void RemoveEntity(Entity* entity);

    void Event(const SDL_Event& currentEvent);
    void Tick();
    void Draw();
};

#endif //TRIALANDERROR_SRC_GAMEWORLD_H_
