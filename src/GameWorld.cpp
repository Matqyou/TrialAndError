//
// Created by Matq on 22/03/2023.
//

#include "GameWorld.h"
#include "game/entities/Entity.h"
#include "game/entities/Character.h"

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

    m_First = nullptr;
    m_Last = nullptr;
    for (auto& i : m_FirstType) i = nullptr;
    for (auto& i : m_LastType) i = nullptr;

    m_Background = GameWindow()->ImageHandler()->LoadTexture("assets/images/backgrounds/background_pattern.png", true);
    m_Background->Query(nullptr, nullptr, &m_BackgroundW, &m_BackgroundH);
    SDL_SetTextureAlphaMod(m_Background->SDLTexture(), 10);
    SDL_SetTextureBlendMode(m_Background->SDLTexture(), SDL_BLENDMODE_BLEND);
}

GameWorld::~GameWorld() {
    Entity* CurrentEntity = m_Last;
    while (CurrentEntity) {
        auto NextEntity = CurrentEntity->m_Prev;
        delete CurrentEntity;
        CurrentEntity = NextEntity;
    }
}

Character* GameWorld::GetPlayerByIndex(int index) const {
    Character* Player = FirstPlayer();
    for (; Player; Player = (Character*)(Player->NextType()))
        if (Player->PlayerIndex() == index)
            return Player;

    return nullptr;
}

void GameWorld::GetNextPlayerIndex(Character* player) const {
    int Index = 1;
    while (GetPlayerByIndex(Index))
        Index++;

    player->m_PlayerIndex = Index;
}

void GameWorld::GetPointInWorld(double relative_x, double relative_y, double& out_x, double& out_y) const {
    out_x = m_x + (relative_x - m_GameWindow->Width() / 2.0);
    out_y = m_y + (relative_y - m_GameWindow->Height() / 2.0);
}

void GameWorld::SetCameraPos(double x, double y) {
    m_x = x;
    m_y = y;
}

void GameWorld::AddEntity(Entity* entity) {
    EntityType Enttype = entity->EntityType();
    Entity*& FirstType = m_FirstType[Enttype];
    Entity*& LastType = m_LastType[Enttype];

    if (!FirstType) { // Then there also shouldn't be a last type
        FirstType = entity;
        LastType = entity;
        entity->m_PrevType = nullptr;
        entity->m_NextType = nullptr;
    } else { // Then there also should be a last type
        LastType->m_NextType = entity;
        entity->m_PrevType = LastType;
        LastType = entity;
    }

    if (!m_First) {
        m_First = entity;
        m_Last = entity;
        entity->m_Prev = nullptr;
        entity->m_Next = nullptr;
    } else {
        m_Last->m_Next = entity;
        entity->m_Prev = m_Last;
        m_Last = entity;
    }
}

// ::RemoveEntity() doesn't reset entities Previous and Next entity pointers
void GameWorld::RemoveEntity(Entity* entity) {
    EntityType Type = entity->EntityType();
    Entity*& FirstType = m_FirstType[Type];
    Entity*& LastType = m_LastType[Type];

    // Remove entity from list of same type
    if (entity->m_PrevType) entity->m_PrevType->m_NextType = entity->m_NextType;
    if (entity->m_NextType) entity->m_NextType->m_PrevType = entity->m_PrevType;
    if (FirstType == entity) FirstType = entity->m_NextType;
    if (LastType == entity) LastType = entity->m_PrevType;

    // Remove entity from list of all entities
    if (entity->m_Prev) entity->m_Prev->m_Next = entity->m_Next;
    if (entity->m_Next) entity->m_Next->m_Prev = entity->m_Prev;
    if (m_First == entity) m_First = entity->m_Next;
    if (m_Last == entity) m_Last = entity->m_Prev;
}

void GameWorld::DestroyPlayerByController(GameController* DeletedController) const {
    Character* Player = FirstPlayer();
    for (; Player; Player = (Character*)(Player->NextType())) {
        if (Player->GetGameController() == DeletedController) {
            delete Player;
            return;
        }
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

    // Loop allows self-destruction in the ::Event() method
    Character* Next, *Current = FirstPlayer();
    for (; Current; Current = Next) {
        Next = (Character*)(Current->NextType());
        Current->Event(currentEvent);
    }
}

void GameWorld::Tick() {
    if (m_Paused)
        return;

    if (!m_ShowNames)
        m_ShowNamesVisibility *= 0.98;

    // Loop allows self-destruction in the ::Tick() method
    Entity* Next, *Current = m_First;
    for (; Current; Current = Next) {
        Next = Current->Next();
        Current->Tick();
    }

    if (m_LastType[ENTTYPE_CHARACTER]) {
        int num_player = 0;

        double CameraX = 0.0;
        double CameraY = 0.0;

        auto Player = (Character*)(m_LastType[ENTTYPE_CHARACTER]);
        for (; Player; Player = (Character*)Player->PrevType()) {
            num_player++;

            CameraX += Player->m_x;
            CameraY += Player->m_y;
        }
        CameraX /= num_player;
        CameraY /= num_player;
        // Accelerate camera closer to
        // the midpoint of characters
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

    Entity* Current = m_Last;
    for (; Current; Current = Current->Prev())
        Current->Draw();

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
