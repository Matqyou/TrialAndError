// Copied Error.h into cartesian
#pragma once

#include <client/game/entities/cartesian/Entity.h>

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
    static LinkTexture sTextureErrorDisorianted;
    static LinkTexture sTextureErrorSpiky;
    static LinkTexture sTextureErrorConfusingHP;
    static LinkTexture sTextureErrorInvincible;
    static LinkTexture sTextureErrorHealersParadise;
    static LinkTexture sTextureErrorRanged;
    static LinkTexture sTextureErrorSlowDown;
    static LinkTexture sTextureErrorDangerousRecoil;
    static LinkTexture sTextureMagicParticle;
    static LinkSound ms_PickupSounds[7];

    Error(const Vec2f& start_pos, int typeID);

    // Ticking
    void Tick(double elapsed_seconds) override;
    void Draw() override;

};
