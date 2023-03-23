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
    auto CopyEntities = m_Entities;
    for (Entity* CurrentEntity : CopyEntities)
        delete CurrentEntity;
}

int GameWorld::NextPlayerIndex() {
    int CurrentIndex = 1;
    while (true) {
        for (Entity *CurrentEntity : m_Entities) {
            if (CurrentEntity->EntityType() != ENTTYPE_CHARACTER)
                continue;

            auto CurrentPlayer = (Character*)CurrentEntity;
            if (CurrentIndex != CurrentPlayer->PlayerIndex())
                return CurrentIndex;

            CurrentIndex++;
        }
    }
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

void GameWorld::MarkForDestruction(Entity* destroyEntity) {
    m_DestroyEntities.push_back(destroyEntity);
}

void GameWorld::ShowNames() {
    m_ShowNames = true;
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

    for (Entity* DestroyEntity : m_DestroyEntities)
        delete DestroyEntity;
    m_DestroyEntities.clear();
}

void GameWorld::Draw() {
    SDL_Renderer* Renderer = m_GameWindow->Renderer();

    SDL_Rect DrawRect = { 0, 0, int(m_Width), int(m_Height) };
    SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(Renderer, &DrawRect);

    for (Entity* CurrentEntity : m_Entities)
        CurrentEntity->Draw();

    m_ShowNames = false;
}
