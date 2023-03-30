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
    m_ShowNamesVisibility = 0.0;
    m_ShowNames = false;
    m_Paused = false;
    m_x = 0.0;
    m_y = 0.0;
    m_CurrentTick = 0;

    m_LastEntity = nullptr;
    for (auto& i : m_LastEntityType)
        i = nullptr;

    m_Background = GameWindow()->ImageHandler()->LoadTexture("assets/images/background_pattern.png", true);
    m_Background->Query(nullptr, nullptr, &m_BackgroundW, &m_BackgroundH);
    SDL_SetTextureAlphaMod(m_Background->SDLTexture(), 10);
    SDL_SetTextureBlendMode(m_Background->SDLTexture(), SDL_BLENDMODE_BLEND);
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

void GameWorld::GetPointInWorld(double relative_x, double relative_y, double& out_x, double& out_y) const {
    out_x = relative_x - m_x;
    out_x = relative_y - m_y;
}

void GameWorld::SetCameraPos(double x, double y) {
    m_x = x;
    m_y = y;
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

void GameWorld::ToggleShowNames() {
    m_ShowNames = !m_ShowNames;
    if (m_ShowNames)
        m_ShowNamesVisibility = 1.0;
}

void GameWorld::SetPaused(bool state) {
    m_Paused = state;
}

void GameWorld::Event(const SDL_Event& currentEvent) {
    if (currentEvent.type == SDL_KEYDOWN &&
        currentEvent.key.keysym.scancode == SDL_SCANCODE_SPACE)
        ToggleShowNames();

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

    if (!m_ShowNames)
        m_ShowNamesVisibility *= 0.98;

    Entity* NextEntity; // allows deletion while looping
    for (Entity* CurrentEntity = m_LastEntity; CurrentEntity != nullptr; CurrentEntity = NextEntity) {
        NextEntity = CurrentEntity->m_PrevEntity;
        CurrentEntity->Tick();
    }

    if (m_LastEntityType[ENTTYPE_CHARACTER]) {
        int num_player = 0;

        double CameraX = 0.0;
        double CameraY = 0.0;
        for (auto Player = (Character*) m_LastEntityType[ENTTYPE_CHARACTER]; Player != nullptr; Player = (Character*)Player->m_PrevEntityType) {
            num_player++;

            CameraX += Player->m_x;
            CameraY += Player->m_y;
        }
        CameraX /= num_player;
        CameraY /= num_player;
        // Accelerate camera closer to the midpoint of characters
        // TODO: Zoom value
        m_x += (-m_x + CameraX) * 0.1;
        m_y += (-m_y + CameraY) * 0.1;
    }

    m_CurrentTick++;
}

void GameWorld::Draw() {
    Drawing* Render = m_GameWindow->RenderClass();

    SDL_Rect DestinationRect = {0, 0, int(m_Width), int(m_Height) };
    Render->RenderTextureWorld(m_Background->SDLTexture(), nullptr, DestinationRect);

    SDL_Rect DrawRect = { 0, 0, int(m_Width), int(m_Height) };
    Render->SetColor(255, 0, 0, 255);
    Render->DrawRectWorld(DrawRect);

    for (Entity* CurrentEntity = m_LastEntity; CurrentEntity != nullptr; CurrentEntity = CurrentEntity->m_PrevEntity)
        CurrentEntity->Draw();

    if (m_ShowNamesVisibility <= 0.05)
        return;

    int Opacity = int(m_ShowNamesVisibility * 255.0);
    TextManager* TextHandler = m_GameWindow->TextHandler();
    char msg[64];
    std::snprintf(msg, sizeof(msg), "%ix, %iy", int(m_x), int(m_y));
    auto CoordinateTexture = TextHandler->Render(TextHandler->FirstFont(), msg, { 127, 127, 127, 255 }, false);
    int coordinate_w, coordinate_h;
    CoordinateTexture->Query(nullptr, nullptr, &coordinate_w, &coordinate_h);
    SDL_Rect CoordinateRect = { int(m_x - coordinate_w / 2.0), int(m_y - coordinate_h / 2.0), coordinate_w, coordinate_h };
    SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), Opacity);
    Render->RenderTextureWorld(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect);
    delete CoordinateTexture;
}
