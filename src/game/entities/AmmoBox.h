//
// Created by Mim on 4/19/2023.
//

#pragma once

#include "Entity.h"
#include "SDL.h"

enum AmmoType {
    AMMO_GLOCK,
    AMMO_BURST,
    AMMO_SHOTGUN,
    AMMO_MINIGUN,
    NUM_AMMO
};

class AmmoBox : public Entity {
protected:
    unsigned int m_AmmoCount;
    AmmoType m_Type;
    Texture** m_Texture;

    // Ticking
    void TickPickup();

public:
    static Texture* ms_TextureGlock;
    static Texture* ms_TextureShotgun;
    static Texture* ms_TextureBurst;
    static Texture* ms_TextureMinigun;
    // static Sound* ms_PickupSounds[7];

    AmmoBox(GameWorld* world,
            AmmoType type,
            const Vec2d& start_pos,
            unsigned int AmmoCount);

    // Getting
    [[nodiscard]] AmmoType Type() const { return m_Type; }

    // Manipulating
    unsigned int TakeAmmo(unsigned int request);

    // Ticking
    void Tick() override;
    void Draw() override;
};
