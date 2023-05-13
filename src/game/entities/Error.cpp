//
// Created by Mim on 5/13/2023.
//

#include "Error.h"
Texture* Error::ms_TextureError = nullptr;
Error::Error(GameWorld* world,double start_x, double start_y)
        : Entity(world, GameWorld::ENTTYPE_ERROR, start_x, start_y, 100, 100, 0.95){
    m_Texture = &ms_TextureError;
    //Make it random which one of  the errors it is
    m_Type = CONFUSING_HP;
}

void Error::TickImpact(double x, double y) {
    auto Player = m_World->FirstPlayer();
    //TODO change it from collision to bullets/punches breaking it
    for (; Player; Player = (Character*)(Player->NextType())) {
        bool Collides = (Player->GetX() - 25 < x) &&
                        (Player->GetX() + 25 > x) &&
                        (Player->GetY() - 25 < y) &&
                        (Player->GetY() + 25 > y);

        if (!Collides)
            continue;

        if(m_Type == DISORIANTED){
            auto Players = m_World->FirstPlayer();
            for (; Players; Players = (Character*)(Players->NextType())) {
                Players->ReverseMovement();
            }
        }

        else if(m_Type == CONFUSING_HP){
            auto Players = m_World->FirstPlayer();
            for (; Players; Players = (Character*)(Players->NextType())) {
                Players->ConfuseHP();
            }
        }
        delete this;
    }
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