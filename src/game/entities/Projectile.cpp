//
// Created by janis on 3/22/2023.
//

#include "Projectile.h"
#include <cmath>
#include <iostream>
#include "Crates.h"
#include "character/Character.h"

Texture* Projectile::ms_TextureGlock = nullptr;
Texture* Projectile::ms_TextureBurst = nullptr;
Texture* Projectile::ms_TextureShotgun = nullptr;
Texture* Projectile::ms_TextureMinigun = nullptr;

Projectile::Projectile(GameWorld* world, Entity* shooter, WeaponType weapon_type, double damage, double start_x, double start_y, double start_xvel, double start_yvel)
 : Entity(world, ENTITY_NORMAL, GameWorld::ENTTYPE_BULLET, start_x, start_y, 6, 10, start_xvel, start_yvel, 1.0){
    switch (weapon_type) {
        case WEAPON_GLOCK: {
            m_Texture = ms_TextureGlock;
        } break;
        case WEAPON_BURST: {
            m_Texture = ms_TextureBurst;
        } break;
        case WEAPON_SHOTGUN: {
            m_Texture = ms_TextureShotgun;
        } break;
        case WEAPON_MINIGUN: {
            m_Texture = ms_TextureMinigun;
        } break;
    }
    m_Shooter = shooter;
    m_Damage = damage;
    m_StillCollidesShooter = true;
}

bool Projectile::TickVelocity() {
    m_Core->m_xvel *= m_Core->m_BaseDamping;
    m_Core->m_yvel *= m_Core->m_BaseDamping;

    m_Core->m_x += m_Core->m_xvel;
    m_Core->m_y += m_Core->m_yvel;

    return TickHitPoint(m_Core->m_x, m_Core->m_y); // TODO: interpolate
}

bool Projectile::TickHitPoint(double x, double y) {
    // Check if position collides any of the players
    auto Char = m_World->FirstCharacter();
    for (; Char; Char = (Character*)(Char->NextType())) {
        bool Shooter = m_Shooter == Char;
        if (!Char->IsAlive() || Char->m_Health <= 0.0) continue;
        if (m_Shooter->EntityType() == GameWorld::ENTTYPE_CHARACTER) {
            if (!Shooter && ((Character*) m_Shooter)->IsNPC() == Char->IsNPC()) continue;
        }

        EntityCore* CharCore = Char->GetCore();
        bool CollidesPlayer = (CharCore->m_x - CharCore->m_w < x) &&
                              (CharCore->m_x + CharCore->m_w > x) &&
                              (CharCore->m_y - CharCore->m_h < y) &&
                              (CharCore->m_y + CharCore->m_h > y);

        if (Shooter && !CollidesPlayer) { m_StillCollidesShooter = false; }
        else if (CollidesPlayer && !Shooter || (Shooter && !m_StillCollidesShooter)) {
            Char->Damage(m_Damage, true);
            Char->Accelerate(m_Core->m_xvel * 0.05, m_Core->m_yvel * 0.05);
            m_Alive = false;
            return true;
        }
    }
    auto Crate = m_World->FirstCrate();
    for(; Crate; Crate = (Crates*)(Crate->NextType())){
        EntityCore* CrateCore = Crate->GetCore();
        bool CollidesCrate = (CrateCore->m_x - CrateCore->m_w /2 < x) &&
                             (CrateCore->m_x + CrateCore->m_w /2 > x) &&
                             (CrateCore->m_y - CrateCore->m_h /2 < y) &&
                             (CrateCore->m_y + CrateCore->m_h /2 > y);
        if(CollidesCrate){
            Crate->DamageCrate(m_Damage);
            m_Alive = false;
            return true;
        }
    }
    return false;
}

void Projectile::TickImpact() {
    // Deletes the bullet if it hits the border
    if (m_Core->m_x < 0 || m_Core->m_x > m_World->GetWidth() ||
        m_Core->m_y < 0 || m_Core->m_y > m_World->GetHeight())
        m_Alive = false;
}

void Projectile::Tick() {
    if (!TickVelocity())
        TickImpact();
}

void Projectile::Draw() {
    Drawing* Render = m_World->GameWindow()->Render();
    double Angle = std::atan2(m_Core->m_yvel, m_Core->m_xvel) / M_PI * 180.0 + 90.0;
    SDL_Rect BulletRect = { int(m_Core->m_x - m_Core->m_w / 2.0), int(m_Core->m_y - m_Core->m_h / 2.0), int(m_Core->m_w), int(m_Core->m_h) };
    Render->RenderTextureExWorld(m_Texture->SDLTexture(), nullptr, BulletRect, Angle, nullptr, SDL_FLIP_NONE);
}
