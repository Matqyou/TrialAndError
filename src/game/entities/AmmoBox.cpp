//
// Created by Mim on 4/19/2023.
//
#include "AmmoBox.h"
#include "character/Character.h"
#include <cmath>
#include <iostream>
Texture* AmmoBox::ms_TextureGlock = nullptr;
Texture* AmmoBox::ms_TextureShotgun = nullptr;
Texture* AmmoBox::ms_TextureBurst = nullptr;
Texture* AmmoBox::ms_TextureMinigun = nullptr;
// Sound* AmmoBox::ms_PickupSounds[7] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

AmmoBox::AmmoBox(GameWorld* world,
                 AmmoType type,
                 const Vec2d& start_pos,
                 unsigned int AmmoCount)
    : Entity(world,
             ENTFORM_NORMAL,
             ENTTYPE_AMMO,
             start_pos,
             Vec2d(40, 28),
             Vec2d(0.0, 0.0),
             0.95) {
    m_AmmoCount = AmmoCount;
    m_Type = type;

    if (type == AMMO_GLOCK) m_Texture = &ms_TextureGlock;
    else if (type == AMMO_SHOTGUN) m_Texture = &ms_TextureShotgun;
    else if (type == AMMO_BURST) m_Texture = &ms_TextureBurst;
    else if (type == AMMO_MINIGUN) m_Texture = &ms_TextureMinigun;
}
void AmmoBox::TickPickup() {
    // Check if position collides any of the players
    auto Char = m_World->FirstCharacter();
    for (; Char; Char = (Character*)(Char->NextType())) {
        EntityCore& CharCore = Char->GetCore();
        double Distance = DistanceVec2(m_Core.Pos, CharCore.Pos);

        if (Distance > m_Core.sizeRatio +Char->GetCore().sizeRatio) continue;

        Char->AmmoPickup(this);

        // TODO Increase ammo amount, also check which type of ammo has been picked up aswell
        // something like if(m_Type == AMMO_GLOCK){
        // m_WeaponType->m_Ammo == m_Amm oCapacity
        // }
    }
}

unsigned int AmmoBox::TakeAmmo(unsigned int request) {
    if (request > m_AmmoCount)
        request = m_AmmoCount;

    m_AmmoCount -= request;
    // TODO fix whatever the fuck this is, or remove it, since i added a pickup sound that is actually playing for me
    // if (request)  {
    //    Sound* PickupSound = ms_PickupSounds[rand()%7];
    //    m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(PickupSound);
    //}

    return request;
}

void AmmoBox::Tick() {
    TickPickup();

    TickVelocity();
    TickWalls();

    if (m_AmmoCount <= 0) m_Alive = false;
}

void AmmoBox::Draw() {
    Drawing* Render = m_World->GameWindow()->Render();

    SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2.0),
                           float(m_Core.Pos.y) - float(m_Core.Size.y / 2.0),
                           float(m_Core.Size.x),
                           float(m_Core.Size.y) };

    Render->RenderTextureFCamera((*m_Texture)->SDLTexture(), nullptr, DrawRect);
}