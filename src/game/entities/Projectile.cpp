//
// Created by janis on 3/22/2023.
//

#include "Projectile.h"
#include <cmath>
#include <iostream>
#include "Crate.h"
#include "characters/character/Character.h"

LoadedTexture Projectile::sTextureSpark("particle.spark");
LoadedSound Projectile::sMetalImpactSounds[2] = {
    LoadedSound("entity.projectile.impact.metal.1"),
    LoadedSound("entity.projectile.impact.metal.2"),
};

Projectile::Projectile(GameWorld* world,
                       Entity* shooter,
                       WeaponType weapon_type,
                       Texture* projectile_texture,
                       double damage,
                       const Vec2d& start_pos,
                       const Vec2d& start_vel)
    : Entity(world,
             NORMAL_ENTITY,
             PROJECTILE_ENTITY,
             start_pos,
             Vec2d(6, 10),
             start_vel,
             1.0,
             false) {
    m_Texture = projectile_texture;
    m_Shooter = shooter;
    m_Type = weapon_type;
    m_Damage = damage;
    m_StillCollidesShooter = true;
}

void Projectile::TickCollision() {
    auto CurrentPosition = m_Core.Pos;
    auto LastPosition = m_LastCore.Pos;
    double distance_traveled = DistanceVec2(CurrentPosition, LastPosition);
    if (distance_traveled <= 0.0)
        return;

    Vec2d direction = (CurrentPosition - LastPosition) / distance_traveled;
    int units_traveled = (int)distance_traveled; // +fraction

    // Sense
    bool ShooterIsCharacter = m_Shooter->GetType() == CHARACTER_ENTITY;
    auto ShooterCharacter = (Character*)m_Shooter; // ⚠ Check for ShooterIsCharacter ⚠

    for (int i = 0; i < units_traveled; i++) {
        if (!m_Alive)
            break;

        Vec2d current_position = LastPosition + direction * (double)i;

        // Check if position collides any of the players
        auto Entity = m_World->FirstEntity();
        for (; Entity; Entity = Entity->Next()) {
            bool IsShooter = m_Shooter == Entity;
            if (!Entity->IsAlive()) continue;
            if (!Entity->HasHealthComponent()) continue;
            if (!Entity->HealthComponent().IsAlive()) continue;

            // Ignore npc friendly fire for now
            if (Entity->GetType() == CHARACTER_ENTITY) {
                auto ShootableCharacter = (Character*)Entity;
                if (ShooterIsCharacter && ShooterCharacter->IsNPC() == ShootableCharacter->IsNPC())
                    continue;
            }

            // Check for (Projectile <-> Entity) collision at the position
            bool Collides = Entity->PointCollides(current_position);
            if (IsShooter && !Collides) { m_StillCollidesShooter = false; }
            else if (Collides && (!IsShooter || !m_StillCollidesShooter)) {
                double victim_health;
                if (Entity->GetType() == CHARACTER_ENTITY) {
                    auto ShootableCharacter = (Character*)Entity;
                    victim_health = ShootableCharacter->HealthComponent().m_Health;
                    ShootableCharacter->Damage(m_Damage, m_Shooter);
                    ShootableCharacter->Accelerate(direction * 0.5 * m_Damage);
                } else if (Entity->GetType() == CRATE_ENTITY) {
                    auto ShootableCrate = (Crate*)Entity;
                    victim_health = ShootableCrate->HealthComponent().m_Health;
                    ShootableCrate->Damage(m_Damage, m_Shooter);
                } else {
                    throw std::runtime_error("Unhandled projectile impact with entity that has health");
                }

                // The projectile has served its purpose (clear immediately all damage has been depleted)
                m_Damage -= victim_health;
                if (m_Damage <= 0.0) {
                    m_Alive = false;
                    break;
                }
            }
        }
    }
}

void Projectile::TickWallCollision() {
    // Deletes the bullet if it hits the border
    if (m_Core.Pos.x < 0 || m_Core.Pos.x > m_World->GetWidth() ||
        m_Core.Pos.y < 0 || m_Core.Pos.y > m_World->GetHeight()) {
        sMetalImpactSounds[rand() % 2].GetSound()->PlaySound();

        if (m_Core.Pos.x < 0) {
            m_Core.Pos.x = 0;
            m_Core.Vel.x *= -1;
        }
        if (m_Core.Pos.x > m_World->GetWidth()) {
            m_Core.Pos.x = m_World->GetWidth();
            m_Core.Vel.x *= -1;
        }
        if (m_Core.Pos.y < 0) {
            m_Core.Pos.y = 0;
            m_Core.Vel.y *= -1;
        }
        if (m_Core.Pos.y > m_World->GetHeight()) {
            m_Core.Pos.y = m_World->GetHeight();
            m_Core.Vel.y *= -1;
        }
        for (int i = 0; i < 6; i++) {
            auto vel = Vec2d(m_Core.Vel.x * (0.1 + 0.17 * (double)(rand()) / RAND_MAX) + 0.05 * (double)(rand()) / RAND_MAX,
                             m_Core.Vel.y * (0.1 + 0.17 * (double)(rand()) / RAND_MAX) + 0.05 * (double)(rand()) / RAND_MAX);
            auto orientation = vel.Atan2() / M_PI * 180.0;
            auto lifetime = (unsigned long long)(6 + rand() % 25);
            auto size = Vec2d(m_Core.Vel.Length() * 0.25 + ((double)(rand() % 100) / 100.0), 3.0);
            m_World->GetParticles()->PlayParticle(Particle(sTextureSpark.GetTexture(), m_Core.Pos, size, vel, 0.98, orientation, 0.0, 1.0, lifetime));
        }

        m_Alive = false;
    }
}

void Projectile::Tick() {
    TickVelocity();
    TickCollision();
    TickWallCollision();
    TickUpdateLastCore();
}

void Projectile::Draw() {
    Drawing* Render = m_World->GameWindow()->Render();
    double Angle = std::atan2(m_Core.Vel.y, m_Core.Vel.x) / M_PI * 180.0 - 90.0;
    SDL_Rect BulletRect = { int(m_Core.Pos.x - m_Core.Size.x / 2.0),
                            int(m_Core.Pos.y - m_Core.Size.y / 2.0),
                            int(m_Core.Size.x),
                            int(m_Core.Size.y) };
    Render->RenderTextureExCamera(m_Texture->SDLTexture(), nullptr, BulletRect, Angle, nullptr, SDL_FLIP_NONE);
}
