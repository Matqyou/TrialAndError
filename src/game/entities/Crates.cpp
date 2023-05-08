//
// Created by Mim on 5/8/2023.
//
#include "Crates.h"
#include "character/Character.h"
#include <iostream>
Texture* Crates::ms_TextureBox = nullptr;
Sound* Crates::ms_HitSound = nullptr;
Crates::Crates(GameWorld* world, DropType type, double start_x, double start_y, double Health)
        : Entity(world, GameWorld::ENTTYPE_BOX, start_x, start_y, 100, 100, 0.95){
    m_World = world;
    m_Health = Health;
    m_Type = type;

    m_Texture = &ms_TextureBox;

}

Crates::~Crates(){
    m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(ms_HitSound);
    if (m_Type != POWERUPS){
        srand (time(NULL));
        int Ammo_type = rand()%4;
        new Ammo(m_World, static_cast<AmmoType>(Ammo_type), m_x, m_y, 20);
    }
}


void Crates::TickImpact(double x, double y) {
    // TODO Make it so the crates take damage from bullets and punches,
    // for now they get damaged from player colission cuz im lazy
    auto Player = m_World->FirstPlayer();
    for (; Player; Player = (Character*)(Player->NextType())) {
        bool Collides = (Player->GetX() - 25 < x) &&
                        (Player->GetX() + 25 > x) &&
                        (Player->GetY() - 25 < y) &&
                        (Player->GetY() + 25 > y);

        if (!Collides)
            continue;

        m_Health -= 2;
    }
}


void Crates::Tick() {
    TickImpact(m_x, m_y);
    TickWalls();

    if (m_Health <= 0) delete this;
}

void Crates::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_FRect DrawRect = {float(m_x) - float(m_w / 2.0),
                          float(m_y) - float(m_h / 2.0),
                          float(m_w),
                          float(m_h)};

    Render->RenderTextureFWorld((*m_Texture)->SDLTexture(), nullptr, DrawRect);
}
#include "Crates.h"
