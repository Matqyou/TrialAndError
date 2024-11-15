//
// Created by janis on 3/22/2023.
//

#include "Projectile.h"
#include <cmath>
#include <iostream>
#include "Crate.h"
#include "characters/character/Character.h"

Texture* Projectile::ms_TextureGlock = nullptr;
Texture* Projectile::ms_TextureBurst = nullptr;
Texture* Projectile::ms_TextureShotgun = nullptr;
Texture* Projectile::ms_TextureMinigun = nullptr;

Projectile::Projectile(GameWorld* world,
                       Entity* shooter,
                       WeaponType weapon_type,
                       double damage,
                       const Vec2d& start_pos,
                       const Vec2d& start_vel)
    : Entity(world, ENTFORM_NORMAL, ENTTYPE_BULLET, start_pos, Vec2d(6, 10), start_vel, 1.0) {
    switch (weapon_type) {
        case WEAPON_GLOCK: {
            m_Texture = ms_TextureGlock;
        }
            break;
        case WEAPON_BURST: {
            m_Texture = ms_TextureBurst;
        }
            break;
        case WEAPON_SHOTGUN: {
            m_Texture = ms_TextureShotgun;
        }
            break;
        case WEAPON_MINIGUN: {
            m_Texture = ms_TextureMinigun;
        }
            break;
    }
    m_Shooter = shooter;
    m_Damage = damage;
    m_StillCollidesShooter = true;
}

bool Projectile::TickHitPoint(double x, double y) {
    // Check if position collides any of the players
    auto Char = m_World->FirstCharacter();
    for (; Char; Char = (Character*)(Char->NextType())) {
        bool Shooter = m_Shooter == Char;
        if (!Char->IsAlive() || Char->m_Health <= 0.0) continue;
        if (m_Shooter->GetEntityType() == ENTTYPE_CHARACTER) {
            if (!Shooter && ((Character*)m_Shooter)->IsNPC() == Char->IsNPC()) continue;
        }

        EntityCore& CharCore = Char->GetCore();
        bool CollidesPlayer = (CharCore.Pos.x - CharCore.Size.x < x) &&
            (CharCore.Pos.x + CharCore.Size.x > x) &&
            (CharCore.Pos.y - CharCore.Size.y < y) &&
            (CharCore.Pos.y + CharCore.Size.y > y);

        if (Shooter && !CollidesPlayer) { m_StillCollidesShooter = false; }
        else if (CollidesPlayer && !Shooter || (Shooter && !m_StillCollidesShooter)) {
            Char->Damage(m_Damage, true);
            Char->Accelerate(m_Core.Vel * 0.05);
            m_Alive = false;
            return true;
        }
    }
    auto Crte = m_World->FirstCrate();
    for (; Crte; Crte = (Crate*)(Crte->NextType())) {
        EntityCore& CrateCore = Crte->GetCore();
        bool CollidesCrate = (CrateCore.Pos.x - CrateCore.Size.x / 2 < x) &&
            (CrateCore.Pos.x + CrateCore.Size.x / 2 > x) &&
            (CrateCore.Pos.y - CrateCore.Size.y / 2 < y) &&
            (CrateCore.Pos.y + CrateCore.Size.y / 2 > y);
        if (CollidesCrate) {
            Crte->DamageCrate(m_Damage);
            m_Alive = false;
            return true;
        }
    }
    return false;
}

void Projectile::TickImpact() {
    TickHitPoint(m_Core.Pos.x, m_Core.Pos.y); // TODO: interpolate

    // Deletes the bullet if it hits the border
    if (m_Core.Pos.x < 0 || m_Core.Pos.x > m_World->GetWidth() ||
        m_Core.Pos.y < 0 || m_Core.Pos.y > m_World->GetHeight())
        m_Alive = false;
}

void Projectile::Tick() {
    TickVelocity();
    TickImpact();
}

void Projectile::Draw() {
    Drawing* Render = m_World->GameWindow()->Render();
    double Angle = std::atan2(m_Core.Vel.y, m_Core.Vel.x) / M_PI * 180.0 + 90.0;
    SDL_Rect BulletRect = { int(m_Core.Pos.x - m_Core.Size.x / 2.0),
                            int(m_Core.Pos.y - m_Core.Size.y / 2.0),
                            int(m_Core.Size.x),
                            int(m_Core.Size.y) };
    Render->RenderTextureExCamera(m_Texture->SDLTexture(), nullptr, BulletRect, Angle, nullptr, SDL_FLIP_NONE);
}
