//
// Created by Mim on 4/19/2023.
//

#ifndef TRIALANDERROR_AMMO_H
#define TRIALANDERROR_AMMO_H

#include "Entity.h"
#include "SDL.h"

enum AmmoType {
    GLOCK_AMMO,
    BURST_AMMO,
    SHOTGUN_AMMO,
    MACHINEGUN_AMMO,
    NUM_AMMO
};

class Ammo : public Entity {
protected:

    double m_AmmoCount;
    bool TickImpact(double x, double y);
    AmmoType m_Type;
public:
static Texture* ms_Texture;

    Ammo(GameWorld* world, AmmoType type, double start_x, double start_y, double AmmoCount);
    AmmoType Type() const{return m_Type;}
    double AmmoCount() const{return m_AmmoCount;}
    void SetAmmoCount(double collected);

    void Tick() override;
};
#endif //TRIALANDERROR_AMMO_H
