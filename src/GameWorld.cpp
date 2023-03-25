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
    m_Paused = false;

    m_LastEntity = nullptr;
    for (auto& i : m_LastEntityType)
        i = nullptr;
}

GameWorld::~GameWorld() {
    Entity* CurrentEntity = m_LastEntity;
    while (CurrentEntity) {
        auto NextEntity = CurrentEntity->m_PrevEntity;
        delete CurrentEntity;
        CurrentEntity = NextEntity;
    }
}

Character* GameWorld::GetPlayerByIndex(int index) {
    for (auto Current = m_LastEntityType[ENTTYPE_CHARACTER]; Current != nullptr; Current = Current->m_PrevEntityType) {
        auto Player = (Character*)Current;
        if (Player->PlayerIndex() == index)
            return Player;
    }

    return nullptr;
}

void GameWorld::GetNextPlayerIndex(Character* player) {
    int Index = 1;
    while (GetPlayerByIndex(Index))
        Index++;

    player->m_PlayerIndex = Index;
}

void GameWorld::AddEntity(Entity* entity) {
    EntityType Enttype = entity->EntityType();
    if (!m_LastEntityType[Enttype]) {
        m_LastEntityType[Enttype] = entity;
        entity->m_PrevEntityType = nullptr;
        entity->m_NextEntityType = nullptr;
    } else {
        m_LastEntityType[Enttype]->m_NextEntityType = entity;
        entity->m_PrevEntityType = m_LastEntityType[Enttype];
        m_LastEntityType[Enttype] = entity;
    }

    if (!m_LastEntity) {
        m_LastEntity = entity;
        entity->m_PrevEntity = nullptr;
        entity->m_NextEntity = nullptr;
    } else {
        m_LastEntity->m_NextEntity = entity;
        entity->m_PrevEntity = m_LastEntity;
        m_LastEntity = entity;
    }
}

void GameWorld::RemoveEntity(Entity* entity) {
    EntityType Enttype = entity->EntityType();
    if (m_LastEntityType[Enttype] == entity) {
        if (entity->m_PrevEntityType) {
            m_LastEntityType[Enttype] = entity->m_PrevEntityType;
            m_LastEntityType[Enttype]->m_NextEntityType = nullptr;
        } else { m_LastEntityType[Enttype] = nullptr; }
    } else {
        Entity* TempNext = entity->m_NextEntityType;
        if (entity->m_NextEntityType) { entity->m_NextEntityType->m_PrevEntityType = entity->m_PrevEntityType; }
        if (entity->m_PrevEntityType) { entity->m_PrevEntityType->m_NextEntityType = TempNext; }
    }

    if (m_LastEntity == entity) {
        if (entity->m_PrevEntity) {
            m_LastEntity = entity->m_PrevEntity;
            m_LastEntity->m_NextEntity = nullptr;
        } else { m_LastEntity = nullptr; }
    } else {
        Entity* TempNext = entity->m_NextEntity;
        if (entity->m_NextEntity) { entity->m_NextEntity->m_PrevEntity = entity->m_PrevEntity; }
        if (entity->m_PrevEntity) { entity->m_PrevEntity->m_NextEntity = TempNext; }
    }
}

void GameWorld::DestroyPlayerByController(GameController* DeletedController) {
    for (Entity* CurrentEntity = m_LastEntity; CurrentEntity != nullptr; CurrentEntity = CurrentEntity->m_PrevEntity) {
        if (CurrentEntity->EntityType() != ENTTYPE_CHARACTER)
            continue;

        auto CurrentPlayer = (Character*)CurrentEntity;
        if (CurrentPlayer->GetGameController() != DeletedController)
            continue;

        delete CurrentEntity;
        return;
    }
}

void GameWorld::ShowNames() {
    m_ShowNames = 1.0;
}

void GameWorld::SetPaused(bool state) {
    m_Paused = state;
}

void GameWorld::Event(const SDL_Event& currentEvent) {
    Entity* NextEntity; // allows deletion while looping
    for (Entity* CurrentEntity = m_LastEntity; CurrentEntity != nullptr; CurrentEntity = NextEntity) {
        NextEntity = CurrentEntity->m_PrevEntity;
        if (CurrentEntity->EntityType() != ENTTYPE_CHARACTER)
            continue;

        auto CurrentPlayer = (Character*)CurrentEntity;
        CurrentPlayer->Event(currentEvent);
    }
}

void GameWorld::Tick() {
    if (m_Paused)
        return;

    m_ShowNames *= 0.95;

    Entity* NextEntity; // allows deletion while looping
    for (Entity* CurrentEntity = m_LastEntity; CurrentEntity != nullptr; CurrentEntity = NextEntity) {
        NextEntity = CurrentEntity->m_PrevEntity;
        CurrentEntity->Tick();
    }
}

void GameWorld::Draw() {
    SDL_Renderer* Renderer = m_GameWindow->Renderer();

    SDL_Rect DrawRect = { 0, 0, int(m_Width), int(m_Height) };
    SDL_SetRenderDrawColor(Renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(Renderer, &DrawRect);

    for (Entity* CurrentEntity = m_LastEntity; CurrentEntity != nullptr; CurrentEntity = CurrentEntity->m_PrevEntity)
        CurrentEntity->Draw();
}
