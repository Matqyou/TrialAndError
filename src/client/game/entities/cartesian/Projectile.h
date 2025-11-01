// Copied Projectile.h into cartesian
#pragma once

#include <client/game/entities/cartesian/Entity.h>
#include "client/game/weapons/projectile/base/ProjectileWeapon.h"

class Projectile : public Entity
{
protected:
    Texture *m_Texture;
    Entity *m_Shooter;
    WeaponType m_Type;
    double m_Damage;
    bool m_StillCollidesShooter;

    void TickCollision();
    void TickWallCollision();

public:
    static LinkTexture sTextureSpark;
    static LinkSound sMetalImpactSounds[2];

    Projectile(Entity *shooter,
               WeaponType weapon_type,
               Texture *projectile_texture,
               double damage,
               const Vec2f& start_pos,
               const Vec2f& start_vel);

	// Ticking
    void Tick(double seconds_elapsed) override;
    void Draw() override;

};
