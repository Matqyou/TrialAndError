// Copied Error.h into cartesian
#pragma once

#include "client/game/entities/cartesian/base/Entity.h"

enum ErrorType {
    HEALERS_PARADISE,
    DISORIENTED,
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
    static LoadTexture sTextureErrorDisorianted;
    static LoadTexture sTextureErrorSpiky;
    static LoadTexture sTextureErrorConfusingHP;
    static LoadTexture sTextureErrorInvincible;
    static LoadTexture sTextureErrorHealersParadise;
    static LoadTexture sTextureErrorRanged;
    static LoadTexture sTextureErrorSlowDown;
    static LoadTexture sTextureErrorDangerousRecoil;
    static LoadTexture sTextureMagicParticle;
    static LinkSound ms_PickupSounds[7];

    Error(const Vec3f& start_pos, int typeID);

    // Ticking
    void Tick(double elapsed_seconds) override;
    void Draw() override;

};
