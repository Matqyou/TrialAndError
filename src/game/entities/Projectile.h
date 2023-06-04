//
// Created by janis on 3/22/2023.
//

#ifndef TRIALANDERROR_BULLETS_H
#define TRIALANDERROR_BULLETS_H

#include "../weapons/projectile/ProjectileWeapon.h"
#include "Entity.h"
#include "SDL.h"

class Projectile : public Entity {
private:
    Texture* m_Texture;
    Entity* m_Shooter;
    double m_Damage;
    bool m_StillCollidesShooter;

    // Ticking
    void TickImpact();
    bool TickVelocity();
    bool TickHitPoint(double x, double y);

public:
    static Texture* ms_TextureGlock;
    static Texture* ms_TextureBurst;
    static Texture* ms_TextureShotgun;
    static Texture* ms_TextureMinigun;

    Projectile(GameWorld* world,
               Entity* shooter,
               WeaponType weapon_type,
               double damage,
               const Vec2d& start_pos,
               const Vec2d& start_vel);

    // Ticking
    void Tick() override;
    void Draw() override;
};

#endif //TRIALANDERROR_BULLETS_H
