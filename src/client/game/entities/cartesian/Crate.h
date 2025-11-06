// Copied Crate.h into cartesian
#pragma once

#include "client/game/entities/cartesian/base/Entity.h"
#include <client/game/entities/cartesian/AmmoBox.h>
#include <client/core/Assets.h>

enum DropType
{
    AMMO,
    ERROR,
	WEAPON,
    NUM_DROP_TYPES,
};

class Crate : public Entity
{
protected:
    friend class GameWorld;
    DropType m_DropType;
    int m_ErrorType;
    Texture *m_Texture;

public:
    static LinkSound sHitSound[3];
    static LinkSound sBoxSound;
    static LoadTexture sBoxTexture;
    static LoadTexture sBreakingBox1Texture;
    static LoadTexture sBreakingBox2Texture;

    Crate(const Vec3f& start_pos, DropType RandomDrop);
    ~Crate() override;

    // Getting
    [[nodiscard]] DropType GetDropType() const { return m_DropType; }

    // Manipulating
    void Damage(double value, Entity *damager);
    void Heal(double value);

    // Ticking
    void Tick(double elapsed_seconds) override;
    void Draw() override;
};
