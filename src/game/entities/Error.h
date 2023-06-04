//
// Created by Mim on 5/13/2023.
//

#ifndef TRIALANDERROR_ERROR_H
#define TRIALANDERROR_ERROR_H
#include "Entity.h"
#include "../weapons/projectile/ProjectileWeapon.h"
#include "character/Character.h"

enum ErrorTypes {
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
    NUM_ERRORS
};

class Error : public Entity {
protected:
    ErrorTypes m_Type;
    Texture* m_Texture;

    void TickPickup(double x, double y);

public:
    static Sound* ms_HitSound;
    static Texture* ms_TextureError;
    static Texture* ms_TextureErrorDisorianted;
    static Texture* ms_TextureErrorSpiky;
    static Texture* ms_TextureErrorConfusingHP;
    static Texture* ms_TextureErrorInvincible;
    static Texture* ms_TextureErrorHealersParadise;
    static Texture* ms_TextureErrorRanged;
    static Texture* ms_TextureErrorSlowDown;
    static Texture* ms_TextureErrorDangerousRecoil;
    static Sound* ms_PickupSounds[7];

    Error(GameWorld* world, const Vec2d& start_pos, int typeID);

    void DrawName();
    void Tick() override;
    void Draw() override;
};

#endif //TRIALANDERROR_ERROR_H
