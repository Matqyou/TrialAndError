//
// Created by Mim on 5/13/2023.
//

#include "Error.h"
#include <random>

Texture* Error::ms_TextureErrorDisorianted = nullptr;
Texture* Error::ms_TextureErrorSpiky = nullptr;
Texture* Error::ms_TextureErrorConfusingHP = nullptr;
Texture* Error::ms_TextureErrorInvincible = nullptr;
Texture* Error::ms_TextureErrorHealersParadise = nullptr;
Texture* Error::ms_TextureErrorRanged = nullptr;
Texture* Error::ms_TextureErrorSlowDown = nullptr;
Texture* Error::ms_TextureErrorDangerousRecoil = nullptr;
Texture* Error::ms_TextureError = nullptr;
Error::Error(GameWorld* world,double start_x, double start_y)
 : Entity(world, ENTITY_NORMAL, GameWorld::ENTTYPE_ERROR, start_x, start_y, 100, 100, 0.0, 0.0, 0.95){
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
    if(type == SPIKY)m_Texture = &ms_TextureErrorSpiky;
    else if(type == INVINCIBLE) m_Texture = &ms_TextureErrorInvincible;
    else if(type == SLOW_DOWN) m_Texture =&ms_TextureErrorSlowDown;
    else if(type == HEALERS_PARADISE) m_Texture =&ms_TextureErrorHealersParadise;
    else if(type == DISORIANTED) m_Texture = &ms_TextureErrorDisorianted;
    else if(type == CONFUSING_HP) m_Texture = &ms_TextureErrorConfusingHP;
    else if(type == RANGED) m_Texture = &ms_TextureErrorRanged;
    else m_Texture = &ms_TextureError;
}

void Error::TickImpact(double x, double y) {
    auto Char = m_World->FirstPlayer();
    for (; Char; Char = (Character*)(Char->NextType())) {
        EntityCore* CharCore = Char->GetCore();
        bool Collides = (CharCore->m_x - 50 < x) &&
                        (CharCore->m_x + 50 > x) &&
                        (CharCore->m_y - 50 < y) &&
                        (CharCore->m_y + 50 > y);

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
        else if(m_Type == INVINCIBLE) Char->MakeInvincible();
        else if(m_Type == SPIKY) Char->MakeSpiky();
        else if(m_Type == HEALERS_PARADISE){
            auto Players = m_World->FirstPlayer();
            for (; Players; Players = (Character*)(Players->NextType())) Players->MakeHealer();
    }
        else if(m_Type == RANGED) Char->MakeRanged();
        else if(m_Type == SLOW_DOWN) {
            auto Players = m_World->FirstPlayer();
            for (; Players; Players = (Character *) (Players->NextType())) Players->SlowDown();
        }
        else if(m_Type == DANGEROUS_RECOIL) Char->ActivateDangerousRecoil();
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
    TickImpact(m_Core->m_x, m_Core->m_y);
    TickWalls(); // todo: don't have any functions after a function that self-destructs the object..... (crash)
}

void Error::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_FRect DrawRect = {float(m_Core->m_x) - float(m_Core->m_w / 2.0),
                          float(m_Core->m_y) - float(m_Core->m_h / 2.0),
                          float(m_Core->m_w),
                          float(m_Core->m_h)};

    Render->RenderTextureFWorld((*m_Texture)->SDLTexture(), nullptr, DrawRect);
}