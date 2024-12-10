//
// Created by janis on 3/22/2023.
//

#pragma once

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
    void TickCollision();
    void TickWallCollision();


public:
    static LoadedTexture sTextureGlock;
    static LoadedTexture sTextureBurst;
    static LoadedTexture sTextureShotgun;
    static LoadedTexture sTextureSniper;
    static LoadedTexture sTextureMinigun;
    static LoadedTexture sTextureSpark;
    static LoadedSound sMetalImpactSounds[2];

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
