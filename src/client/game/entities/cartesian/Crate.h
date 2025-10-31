// Copied Crate.h into cartesian
#pragma once

#include <client/game/entities/cartesian/Entity.h>
#include <client/game/entities/cartesian/AmmoBox.h>
#include <client/core/Assets.h>

enum DropType
{
    AMMO,
    ERROR,
    NUM_DROPS
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
    static LinkTexture sBoxTexture;
    static LinkTexture sBreakingBox1Texture;
    static LinkTexture sBreakingBox2Texture;

    Crate(GameWorld *world,
          const Vec2f& start_pos,
          DropType RandomDrop);
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
