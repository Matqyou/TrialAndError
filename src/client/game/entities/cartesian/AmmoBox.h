// Copied AmmoBox.h into cartesian
#pragma once

#include <client/game/entities/cartesian/Entity.h>
#include "SDL.h"

enum AmmoType
{
    AMMO_GLOCK,
    AMMO_BURST,
    AMMO_SHOTGUN,
    AMMO_MINIGUN,
    AMMO_SNIPER,
    NUM_AMMO_TYPES
};

class AmmoBox : public Entity
{
protected:
    unsigned int m_AmmoCount;
    AmmoType m_Type;
    Texture *m_Texture;

    // Ticking
    void TickPickup();

public:
    static LoadedTexture sTextureGlock;
    static LoadedTexture sTextureShotgun;
    static LoadedTexture sTextureBurst;
    static LoadedTexture sTextureMinigun;
    static LoadedTexture sTextureSniper;

    AmmoBox(GameWorld *world,
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
