//
// Created by Matq on 22/03/2023.
//

#include "GameWorld.h"
#include "entities/Entity.h"
#include "entities/Character.h"

GameWorld::GameWorld(GameReference* gameWindow, double width, double height) {
    m_GameWindow = gameWindow;
    m_Width = width;
    m_Height = height;
}

GameWorld::~GameWorld() {
    for (Entity* CurrentEntity : m_Entities)
        delete CurrentEntity;
}

void GameWorld::AddEntity(Entity* entity) {
    m_Entities.push_back(entity);
}

void GameWorld::RemoveEntity(Entity* entity) {
    for (auto Iterator = m_Entities.begin(); Iterator != m_Entities.end(); Iterator++) {
        Entity* CurrentEntity = *Iterator;
        if (CurrentEntity != entity)
            continue;

        m_Entities.erase(Iterator);
        break;
    }
}

void GameWorld::Event(const SDL_Event& currentEvent) {
    for (Entity* CurrentEntity : m_Entities) {
        if (CurrentEntity->EntityType() != ENTTYPE_CHARACTER)
            continue;

        auto CurrentPlayer = (Character*)CurrentEntity;
        CurrentPlayer->Event(currentEvent);
    }
}

void GameWorld::Tick() {
    for (Entity* CurrentEntity : m_Entities)
        CurrentEntity->Tick();
}

void GameWorld::Draw() {
    SDL_Renderer* Renderer = m_GameWindow->Renderer();

    SDL_Rect DrawRect = { 0, 0, int(m_Width), int(m_Height) };
    SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(Renderer, &DrawRect);

    for (Entity* CurrentEntity : m_Entities)
        CurrentEntity->Draw();
}
