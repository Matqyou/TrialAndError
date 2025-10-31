// Copied AmmoBox.h into cartesian
#pragma once

#include <client/game/entities/cartesian/Entity.h>

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
    static LinkTexture sTextureGlock;
    static LinkTexture sTextureShotgun;
    static LinkTexture sTextureBurst;
    static LinkTexture sTextureMinigun;
    static LinkTexture sTextureSniper;

    AmmoBox(GameWorld *world,
            AmmoType type,
            const Vec2f& start_pos,
            unsigned int AmmoCount);

    // Getting
    [[nodiscard]] AmmoType Type() const { return m_Type; }

    // Manipulating
    unsigned int TakeAmmo(unsigned int amount);

    // Ticking
    void Tick(double elapsed_seconds) override;
    void Draw() override;
};
