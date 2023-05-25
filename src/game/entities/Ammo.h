//
// Created by Mim on 4/19/2023.
//

#ifndef TRIALANDERROR_AMMO_H
#define TRIALANDERROR_AMMO_H

#include "Entity.h"
#include "SDL.h"

enum AmmoType {
    AMMO_GLOCK,
    AMMO_BURST,
    AMMO_SHOTGUN,
    AMMO_MINIGUN,
    NUM_AMMO
};

class Ammo : public Entity {
protected:
    unsigned int m_AmmoCount;
    AmmoType m_Type;
    Texture** m_Texture;

    void TickPickup();
public:
    static Texture* ms_TextureGlock;
    static Texture* ms_TextureShotgun;
    static Texture* ms_TextureBurst;
    static Texture* ms_TextureMinigun;
    //static Sound* ms_PickupSounds[7];

    Ammo(GameWorld* world, AmmoType type, double start_x, double start_y, double AmmoCount);
    AmmoType Type() const{ return m_Type; }

    unsigned int TakeAmmo(unsigned int request);

    void Tick() override;
    void Draw() override;
};
#endif //TRIALANDERROR_AMMO_H
