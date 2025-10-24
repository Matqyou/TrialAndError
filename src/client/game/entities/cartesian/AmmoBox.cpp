// Copied AmmoBox.cpp into cartesian
#include "AmmoBox.h"
#include "../characters/character/Character.h"
#include <cmath>
#include <iostream>

LoadedTexture AmmoBox::sTextureGlock("entity.ammo_box.glock");
LoadedTexture AmmoBox::sTextureShotgun("entity.ammo_box.shotgun");
LoadedTexture AmmoBox::sTextureBurst("entity.ammo_box.burst");
LoadedTexture AmmoBox::sTextureMinigun("entity.ammo_box.minigun");
LoadedTexture AmmoBox::sTextureSniper("entity.ammo_box.sniper");

AmmoBox::AmmoBox(GameWorld *world,
                 AmmoType type,
                 const Vec2d& start_pos,
                 unsigned int AmmoCount)
    : Entity(world,
             NORMAL_ENTITY,
             AMMO_BOX_ENTITY,
             start_pos,
             Vec2d(40, 28),
             Vec2d(0.0, 0.0),
             0.95,
             false)
{
    m_AmmoCount = AmmoCount;
    m_Type = type;

    if (type == AMMO_GLOCK) m_Texture = sTextureGlock.GetTexture();
    else if (type == AMMO_SHOTGUN) m_Texture = sTextureShotgun.GetTexture();
    else if (type == AMMO_BURST) m_Texture = sTextureBurst.GetTexture();
    else if (type == AMMO_MINIGUN) m_Texture = sTextureMinigun.GetTexture();
    else if (type == AMMO_SNIPER) m_Texture = sTextureSniper.GetTexture();
}
void AmmoBox::TickPickup()
{
    auto Char = m_World->FirstCharacter();
    for (; Char; Char = (Character *)(Char->NextType()))
    {
        EntityCore& CharCore = Char->GetCore();
        double Distance = DistanceVec2d(m_Core.Pos, CharCore.Pos);

        if (Distance > m_Core.sizeRatio + Char->GetCore().sizeRatio) continue;

        Char->AmmoPickup(this);
    }
}

unsigned int AmmoBox::TakeAmmo(unsigned int request)
{
    if (request > m_AmmoCount)
        request = m_AmmoCount;

    m_AmmoCount -= request;
    return request;
}

void AmmoBox::Tick()
{
    TickPickup();

    TickVelocity();
    TickWalls();

    if (m_AmmoCount <= 0) m_Alive = false;
}

void AmmoBox::Draw()
{
    Drawing *Render = m_World->GameWindow()->Render();

    SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2.0),
                           float(m_Core.Pos.y) - float(m_Core.Size.y / 2.0),
                           float(m_Core.Size.x),
                           float(m_Core.Size.y) };

    Render->RenderTextureFCamera(m_Texture->SDLTexture(), nullptr, DrawRect);
}
