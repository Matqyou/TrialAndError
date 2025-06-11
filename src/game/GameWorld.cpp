//
// Created by Matq on 22/03/2023.
//

#include "GameWorld.h"
#include "Player.h"
#include "entities/Entity.h"
#include "entities/characters/character/Character.h"
#include "entities/characters/CharacterNPC.h"
#include <cmath>

GameWorld::GameWorld(GameReference *game_window, int width, int height)
{
    m_GameWindow = game_window;
    m_PauseMenu = new PauseMenu(this);
    m_LevelUpMenu = nullptr;
    m_Particles = new Particles(this);
    m_Tiles = new TileMap(game_window->Render(), 32, width, height);
    m_Width = m_Tiles->TotalWidth();
    m_Height = m_Tiles->TotalHeight();
    m_ShowNamesVisibility = 0.0;
    m_ShowNames = false;
    m_Paused = false;
    m_GameOver = false;
    m_CurrentTick = 0;

    m_TestingMode = false;

    m_First = nullptr;
    m_Last = nullptr;
    memset(m_FirstType, 0, sizeof(m_FirstType));
    memset(m_LastType, 0, sizeof(m_LastType));
    m_FirstPlayer = nullptr;
    m_LastPlayer = nullptr;

    m_Background = Assets::Get()->GetTexture("backgrounds.background_pattern");
    m_BackgroundW = m_Background->GetWidth();
    m_BackgroundH = m_Background->GetHeight();

    m_LastWave = 0;
    m_TimeBetweenWaves = 300;
    m_NumEnemiesPerWave = 1;
    m_Round = 0;
    m_Score = 0;
    m_ScoreText = new TextSurface(m_GameWindow->Assetz(),
                                  m_GameWindow->Assetz()->TextHandler()->GetMainFont(),
                                  "Score: 0", {150, 150, 0});
}

GameWorld::~GameWorld()
{
    delete m_Tiles;
    delete m_Particles;
    Entity *CurrentEntity = m_Last;
    while (CurrentEntity)
    {
        auto NextEntity = CurrentEntity->m_Prev;
        delete CurrentEntity;
        CurrentEntity = NextEntity;
    }
}

void GameWorld::EnemyKilled(Player *player, Character *enemy)
{
    // Add score for killing the enemy
    AddScore(100); // Example score value

    // Check if the enemy is a boss and add more score
    // if (enemy->IsBoss()) {
    //     AddScore(500); // Example boss score value
    // }

    // Grant XP to the player
    player->GainXP(10); // Example XP reward
}

void GameWorld::CheckLevelUps()
{
    for (auto player = m_GameWindow->World()->FirstPlayer(); player != nullptr; player = player->Next())
    {
        std::queue<LevelUpMenu *> playerQueue = player->GetLevelUpMenuQueue();
        if (!playerQueue.empty())
        {
            m_LevelUpMenu = (playerQueue.front());
            playerQueue.pop();
            player->SetLevelUpMenuQueue(playerQueue);
        }
        else
            m_LevelUpMenu = nullptr;
    }

    if (m_LevelUpMenu != nullptr)
        m_LevelUpMenu->Show();
}

unsigned int GameWorld::GetNextPlayerIndex() const
{
    unsigned int Index = 0;

    while (true)
    {
        bool Used = false;
        for (auto pPlayer = m_FirstPlayer; pPlayer != nullptr; pPlayer = pPlayer->m_Next)
        {
            if (pPlayer->GetIndex() == Index)
                Used = true;
        }

        if (!Used)
            return Index;
        Index++;
    }
}

void GameWorld::AddScore(unsigned int score)
{
    m_Score += score;
    char msg[64];
    std::snprintf(msg, sizeof(msg), "Score: %i", m_Score);
    m_ScoreText->SetText(msg);
    m_ScoreText->FlagForUpdate();
}

void GameWorld::EnemiesKilled()
{
    if (m_TimeBetweenWaves - (m_CurrentTick - m_LastWave) > 300)
        m_LastWave = m_CurrentTick - m_TimeBetweenWaves + 300;
}

void GameWorld::AlliesGone()
{
    m_GameOver = true;
    char msg[64];
    std::snprintf(msg, sizeof(msg), "%i", m_Score);
    m_ScoreText->SetText(msg);
    m_ScoreText->FlagForUpdate();
}

Player *GameWorld::AddPlayer(Player *player)
{
    if (!m_FirstPlayer)
    {
        m_FirstPlayer = player;
        m_LastPlayer = player;
        player->m_Prev = nullptr;
        player->m_Next = nullptr;
    }
    else
    {
        m_LastPlayer->m_Next = player;
        player->m_Prev = m_LastPlayer;
        m_LastPlayer = player;
    }

    return player;
}

// ::RemovePlayer() doesn't reset players Previous and Next player pointers
void GameWorld::RemovePlayer(Player *player)
{
    // Remove player from list of all players
    if (player->m_Prev)
        player->m_Prev->m_Next = player->m_Next;
    if (player->m_Next)
        player->m_Next->m_Prev = player->m_Prev;
    if (m_FirstPlayer == player)
        m_FirstPlayer = player->m_Next;
    if (m_LastPlayer == player)
        m_LastPlayer = player->m_Prev;
}

Entity *GameWorld::AddEntity(Entity *entity)
{
    EntityType Enttype = entity->GetType();
    Entity *&FirstType = m_FirstType[Enttype];
    Entity *&LastType = m_LastType[Enttype];

    if (!FirstType)
    { // Then there also shouldn't be a last type
        FirstType = entity;
        LastType = entity;
        entity->m_PrevType = nullptr;
        entity->m_NextType = nullptr;
    }
    else
    { // Then there also should be a last type
        LastType->m_NextType = entity;
        entity->m_PrevType = LastType;
        LastType = entity;
    }

    if (!m_First)
    {
        m_First = entity;
        m_Last = entity;
        entity->m_Prev = nullptr;
        entity->m_Next = nullptr;
    }
    else
    {
        m_Last->m_Next = entity;
        entity->m_Prev = m_Last;
        m_Last = entity;
    }

    return entity;
}

// ::RemoveEntity() doesn't reset entities Previous and Next entity pointers
void GameWorld::RemoveEntity(Entity *entity)
{
    EntityType Type = entity->GetType();
    Entity *&FirstType = m_FirstType[Type];
    Entity *&LastType = m_LastType[Type];

    // Remove entity from list of same type
    if (entity->m_PrevType)
        entity->m_PrevType->m_NextType = entity->m_NextType;
    if (entity->m_NextType)
        entity->m_NextType->m_PrevType = entity->m_PrevType;
    if (FirstType == entity)
        FirstType = entity->m_NextType;
    if (LastType == entity)
        LastType = entity->m_PrevType;

    // Remove entity from list of all entities
    if (entity->m_Prev)
        entity->m_Prev->m_Next = entity->m_Next;
    if (entity->m_Next)
        entity->m_Next->m_Prev = entity->m_Prev;
    if (m_First == entity)
        m_First = entity->m_Next;
    if (m_Last == entity)
        m_Last = entity->m_Prev;
}

void GameWorld::DestroyPlayerByController(GameController *DeletedController) const
{
    Player *Plr = m_FirstPlayer;
    for (; Plr; Plr = Plr->m_Next)
    {
        Character *Char = Plr->GetCharacter();
        if (Char->GetGameController() == DeletedController)
        {
            delete Plr;
            return;
        }
    }
}

void GameWorld::DestroyCharacterByController(GameController *DeletedController) const
{
    Character *Char = FirstCharacter();
    for (; Char; Char = (Character *)(Char->NextType()))
    {
        if (Char->GetGameController() == DeletedController)
        {
            delete Char;
            return;
        }
    }
}

void GameWorld::ToggleShowNames()
{
    m_ShowNames = !m_ShowNames;
    if (m_ShowNames)
        m_ShowNamesVisibility = 1.0;
}

void GameWorld::Event(const SDL_Event &currentEvent)
{
    if (m_Paused)
        return;

    if (currentEvent.type == SDL_KEYDOWN)
    {
        if (currentEvent.key.keysym.scancode == SDL_SCANCODE_SPACE)
            ToggleShowNames();
        else if (currentEvent.key.keysym.scancode == SDL_SCANCODE_O)
        {
            m_Tiles->LoadTilemap("assets/tilemaps/test_level");
            m_Width = m_Tiles->TotalWidth();
            m_Height = m_Tiles->TotalHeight();
        }
        else if (currentEvent.key.keysym.scancode == SDL_SCANCODE_P)
            m_Tiles->SaveTilemap("assets/tilemaps/test_level");
    }

    // Loop allows self-destruction in the ::Event() method
    Character *Next, *Current = FirstCharacter();
    for (; Current; Current = Next)
    {
        Next = (Character *)(Current->NextType());
        Current->Event(currentEvent);
    }
}

void GameWorld::TickCamera()
{
    if (!m_FirstType[CHARACTER_ENTITY])
        return;

    bool FirstIteration = true;
    double minX, maxX, minY, maxY;

    auto Char = FirstCharacter();
    for (; Char; Char = (Character *)Char->NextType())
    {
        if (Char->IsNPC())
        {
            auto NPC = (CharacterNPC *)Char;
            if (!NPC->GetCurrentWeapon())
                continue;
        }

        EntityCore &Core = Char->GetCore();

        if (FirstIteration)
        {
            FirstIteration = false;
            minX = Core.Pos.x;
            maxX = Core.Pos.x;
            minY = Core.Pos.y;
            maxY = Core.Pos.y;
        }
        else
        {
            if (Core.Pos.x < minX)
                minX = Core.Pos.x;
            if (Core.Pos.x > maxX)
                maxX = Core.Pos.x;
            if (Core.Pos.y < minY)
                minY = Core.Pos.y;
            if (Core.Pos.y > maxY)
                maxY = Core.Pos.y;
        }
    }

    if (!FirstIteration)
    {
        double CameraX = (maxX + minX) / 2.0;
        double CameraY = (maxY + minY) / 2.0;
        double ZoomX = m_GameWindow->GetWidth() / (maxX - minX + 500);
        double ZoomY = m_GameWindow->GetHeight() / (maxY - minY + 500);
        double Zoom = std::min(ZoomX, ZoomY);

        Drawing *Render = m_GameWindow->Render();
        double OldCamX = Render->GetCameraX();
        double OldCamY = Render->GetCameraY();
        Render->SetCameraPos(OldCamX + 0.1 * (CameraX - OldCamX),
                             OldCamY + 0.1 * (CameraY - OldCamY));

        double OldZoom = Render->GetZoom();
        double NewZoom = OldZoom + 0.1 * (Zoom - OldZoom);
        Render->SetZoom(NewZoom > 1.0 ? 1.0 : NewZoom);
    }
}

void GameWorld::TickSpawner()
{
    if (m_TestingMode || m_CurrentTick - m_LastWave < m_TimeBetweenWaves)
        return;

    m_LastWave = m_CurrentTick;
    m_Round += 1;
    m_NumEnemiesPerWave = (unsigned int)(1.0 + std::pow(m_Round, 0.5) * 2);
    m_TimeBetweenWaves = (unsigned long long)((m_Round * m_NumEnemiesPerWave) * m_GameWindow->Timer()->GetFramerate());
    m_Score += m_Round * 50;
    char msg[64];
    std::snprintf(msg, sizeof(msg), "Score: %i", m_Score);
    m_ScoreText->SetText(msg);
    m_ScoreText->FlagForUpdate();

    double Width2 = m_Width / 2.0;
    double Height2 = m_Height / 2.0;

    // Boss every 10 rounds
    if (m_Round % 10 == 0)
    {
        double Angle = (180.0 + (rand() % 180)) / 180.0 * M_PI;
        Vec2d SpawnPos = Vec2d(Width2 + std::cos(Angle) * Width2, Height2 + std::sin(Angle) * Height2);
        auto NewNPC = new CharacterNPC(this,
                                       200.0 + m_Round * 10.0,
                                       SpawnPos,
                                       Vec2d(0.0, 0.0),
                                       NPC_TURRET,
                                       true);
        NewNPC->GiveWeapon(new WeaponMinigun(nullptr));
    }

    // Wave enemies
    for (int i = 0; i < m_NumEnemiesPerWave; i++)
    {
        double Angle = (180.0 + (rand() % 180)) / 180.0 * M_PI;
        Vec2d SpawnPos = Vec2d(Width2 + std::cos(Angle) * Width2, Height2 + std::sin(Angle) * Height2);
        double Health = std::pow(m_Round, 1.0 / 3) * 10.0;
        auto NewNPC = new CharacterNPC(this,
                                       Health,
                                       SpawnPos,
                                       Vec2d(0.0, 0.0),
                                       NPC_TURRET,
                                       false);

        int Weaponizer = rand() % 100;
        if (m_Round >= 15)
        {
            if (Weaponizer < 10)
                NewNPC->GiveWeapon(new WeaponGlock(nullptr));
            else if (Weaponizer < 20)
                NewNPC->GiveWeapon(new WeaponShotgun(nullptr));
            else if (Weaponizer < 30)
                NewNPC->GiveWeapon(new WeaponBurst(nullptr));
        }
        else if (m_Round >= 10)
        {
            if (Weaponizer < 10)
                NewNPC->GiveWeapon(new WeaponGlock(nullptr));
            else if (Weaponizer < 20)
                NewNPC->GiveWeapon(new WeaponShotgun(nullptr));
        }
        else if (m_Round >= 5)
        {
            if (Weaponizer < 10)
                NewNPC->GiveWeapon(new WeaponGlock(nullptr));
        }
    }
}

void GameWorld::TickEntities()
{
    // Loop through each entity and tick
    for (auto Current = m_First; Current; Current = Current->m_Next)
        Current->Tick();
}

void GameWorld::TickDestroy()
{
    // Loop through each entity and destroy aliven't entities
    Entity *Current, *Next;
    for (Current = m_First; Current; Current = Next)
    {
        Next = Current->m_Next;
        if (!Current->IsAlive())
            delete Current;
    }
}

void GameWorld::Tick()
{
    if (m_Paused || m_GameOver)
        return;

    if (!m_ShowNames)
        m_ShowNamesVisibility *= 0.98;

    TickSpawner();
    TickEntities();
    TickDestroy();
    m_Particles->Tick();
    TickCamera();

    m_CurrentTick++;
}

void GameWorld::Draw()
{
    Drawing *Render = m_GameWindow->Render();

    // Stop drawing when the game has been triggered as over
    if (!m_GameOver)
    {
        SDL_Rect DestinationRect = {0, 0, int(m_Width), int(m_Height)};
        Render->RenderTextureCamera(m_Background->SDLTexture(), nullptr, DestinationRect);

        SDL_Rect DrawRect = {0, 0, int(m_Width), int(m_Height)};
        Render->SetColor(100, 100, 100, 255);
        Render->DrawRectCamera(DrawRect);

        m_Particles->Draw();
        for (auto Current : m_FirstType)
            for (; Current; Current = Current->NextType())
                Current->Draw();

        m_Tiles->Draw();
    }

    // Draw the score value
    Texture *ScoreTexture = m_ScoreText->RequestUpdate();
    int ScoreWidth = int(ScoreTexture->GetWidth() * 2.5);
    int ScoreHeight = int(ScoreTexture->GetHeight() * 2.5);
    SDL_Rect ScoreRect = {0, int(m_GameWindow->GetHeight() - ScoreHeight), ScoreWidth, ScoreHeight};
    if (!m_GameOver)
        Render->RenderTexture(ScoreTexture->SDLTexture(), nullptr, ScoreRect);
    else
        Render->RenderTextureFullscreen(ScoreTexture->SDLTexture(), nullptr);
}
