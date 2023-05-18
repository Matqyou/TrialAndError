//
// Created by Mim on 5/13/2023.
//

#include "Error.h"
#include <random>

Texture* Error::ms_TextureError = nullptr;
Error::Error(GameWorld* world,double start_x, double start_y)
        : Entity(world, GameWorld::ENTTYPE_ERROR, start_x, start_y, 100, 100, 0.95){
    m_Texture = &ms_TextureError;
    m_ErrorText = new TextSurface(m_World->GameWindow()->Assets(),
                                  m_World->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                  "0", { 0, 0, 0 });
    //Make it random which one of  the errors it is
    std::random_device rd;
    std::mt19937 gen(rd());
    double interval[] = {0, 0, 1, 100};
    double weights[] = { .10, 0, .9};
    std::piecewise_constant_distribution<> dist(std::begin(interval),
                                                std::end(interval),
                                                weights);
    int RandomNumber = dist(gen);
    ErrorTypes type;
    if(RandomNumber < 20) type = DISORIANTED;
    else if(RandomNumber < 40) { type = SPIKY; std::printf("Collected SPIKY %i\n", RandomNumber); }
    else if(RandomNumber < 50) { type = CONFUSING_HP; std::printf("Collected CONFUSING_HP %i\n", RandomNumber); }
    else if(RandomNumber < 60) { type = INVINCIBLE; std::printf("Collected INVINCIBLE %i\n", RandomNumber); }
    else if(RandomNumber < 70) { type = HEALERS_PARADISE; std::printf("Collected HEALERS_PARADISE %i\n", RandomNumber); }
    else if(RandomNumber < 80) { type = RANGED; std::printf("Collected RANGED %i\n", RandomNumber); }
    else if(RandomNumber < 90) { type = SLOW_DOWN; std::printf("Collected SLOW_DOWN %i\n", RandomNumber); }
    else if(RandomNumber < 100) { type = DANGEROUS_RECOIL; std::printf("Collected DANGEROUS_RECOIL %i\n", RandomNumber); }
    m_Type = type;
}

void Error::TickImpact(double x, double y) {
    auto Player = m_World->FirstPlayer();
    for (; Player; Player = (Character*)(Player->NextType())) {
        bool Collides = (Player->GetX() - 50 < x) &&
                        (Player->GetX() + 50 > x) &&
                        (Player->GetY() - 50 < y) &&
                        (Player->GetY() + 50 > y);

        if (!Collides)
            continue;

        if(m_Type == DISORIANTED){
            auto Players = m_World->FirstPlayer();
            for (; Players; Players = (Character*)(Players->NextType())) Players->ReverseMovement();
        }

        else if(m_Type == CONFUSING_HP){
            auto Players = m_World->FirstPlayer();
            for (; Players; Players = (Character*)(Players->NextType())) Players->ConfuseHP();
        }
        else if(m_Type == INVINCIBLE)Player->MakeInvincible();
        else if(m_Type == SPIKY) Player->MakeSpiky();
        else if(m_Type == HEALERS_PARADISE){
            auto Players = m_World->FirstPlayer();
            for (; Players; Players = (Character*)(Players->NextType())) Players->MakeHealer();
    }
        else if(m_Type == RANGED)Player->MakeRanged();
        else if(m_Type == SLOW_DOWN) {
            auto Players = m_World->FirstPlayer();
            for (; Players; Players = (Character *) (Players->NextType())) Players->SlowDown();
        }
        else if(m_Type == DANGEROUS_RECOIL)Player->ActivateDangerousRecoil();
        delete this;
        DrawName();
    }
}

void Error::DrawName() {
    // TODO Make it so the name of the error appears on top of the screen when its collected, for like 5 seconds
    Drawing* Render = m_World->GameWindow()->RenderClass();


    std::snprintf(msg, sizeof(msg), "%s", "REVERSE MOVEMENT");

    SDL_Rect ErrorRect = { int(m_World->CameraX() /2) ,int(m_World->CameraX()), int(m_World->CameraY()), int(m_World->Height()) /10};
    // Render->RenderTextureWorld(reinterpret_cast<SDL_Texture *>(m_ErrorText), nullptr, ErrorRect);
}



void Error::Tick(){
    TickImpact(m_x, m_y);
    TickWalls();
}

void Error::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_FRect DrawRect = {float(m_x) - float(m_w / 2.0),
                          float(m_y) - float(m_h / 2.0),
                          float(m_w),
                          float(m_h)};

    Render->RenderTextureFWorld((*m_Texture)->SDLTexture(), nullptr, DrawRect);
}