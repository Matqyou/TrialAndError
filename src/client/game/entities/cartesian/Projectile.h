// Copied Projectile.h into cartesian
#pragma once

#include <client/game/entities/cartesian/Entity.h>
#include <client/game/weapons/projectile/ProjectileWeapon.h>

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
    static LoadedTexture sTextureSpark;
    static LoadedSound sMetalImpactSounds[2];

    Projectile(GameWorld *world,
               Entity *shooter,
               WeaponType weapon_type,
               Texture *projectile_texture,
               double damage,
               const Vec2d& start_pos,
               const Vec2d& start_vel);

    void Tick() override;
    void Draw() override;
};
