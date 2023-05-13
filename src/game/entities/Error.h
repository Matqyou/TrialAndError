//
// Created by Mim on 5/13/2023.
//

#ifndef TRIALANDERROR_ERROR_H
#define TRIALANDERROR_ERROR_H
#include "Entity.h"
#include "../weapons/ProjectileWeapon.h"
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
    NUM_ERRORS
};

class Error : public Entity {
protected:
    ErrorTypes m_Type;
    double m_Health;
    Texture** m_Texture;
    void TickImpact(double x, double y);
public:
    static Sound* ms_HitSound;
    static Texture* ms_TextureError;
    static Sound* ms_PickupSounds[7];
    Error(GameWorld* world, double start_x, double start_y);

    void Tick() override;
    void Draw() override;
};


#endif //TRIALANDERROR_ERROR_H
