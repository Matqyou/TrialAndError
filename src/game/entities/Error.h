//
// Created by Mim on 5/13/2023.
//

#pragma once

#include "Entity.h"
#include "../weapons/projectile/ProjectileWeapon.h"

enum ErrorType {
    HEALERS_PARADISE,
    DISORIANTED,
    INVINCIBLE,
    DANGEROUS_RECOIL,
    SPIKY,
    BULLET_FRENZY,
    CONFUSING_HP,
    TELEPORT,
    SLOW_DOWN,
    I_CANT_HEAR,
    RANGED,
    NUM_ERROR_TYPES
};

class Error : public Entity {
protected:
    ErrorType m_Type;
    Texture* m_Texture;

    void TickPickup(double x, double y);

public:
    static LoadedTexture sTextureErrorDisorianted;
    static LoadedTexture sTextureErrorSpiky;
    static LoadedTexture sTextureErrorConfusingHP;
    static LoadedTexture sTextureErrorInvincible;
    static LoadedTexture sTextureErrorHealersParadise;
    static LoadedTexture sTextureErrorRanged;
    static LoadedTexture sTextureErrorSlowDown;
    static LoadedTexture sTextureErrorDangerousRecoil;
    static LoadedTexture sTextureMagicParticle;
    static LoadedSound ms_PickupSounds[7];

    Error(GameWorld* world, const Vec2d& start_pos, int typeID);

    // Ticking
    void Tick() override;
    void Draw() override;
};
