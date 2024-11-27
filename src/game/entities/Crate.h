//
// Created by Mim on 5/08/2023.
//

#pragma once

#include "Entity.h"
#include "SDL.h"
#include "AmmoBox.h"
#include "Error.h"

enum DropType {
    AMMO,
    ERROR,
    NUM_DROPS
};

class Crate : public Entity, public IEntityHasHealth {
protected:
    friend class GameWorld;
    DropType m_DropType;
    int m_ErrorType;
    Texture** m_Texture;

public:
    static Sound* ms_HitSound;
    static Sound* ms_BoxSound;
    static Texture* ms_TextureBox;
    static Texture* ms_TextureBreakingBox1;
    static Texture* ms_TextureBreakingBox2;

    Crate(GameWorld* world,
          const Vec2d& start_pos,
          DropType RandomDrop);
    ~Crate();

    // Getting
    [[nodiscard]] DropType GetDropType() const { return m_DropType; }

    // Manipulating
    void Damage(double value, Entity* damager) override;
    void Heal(double value) override;

    // Ticking
    void Tick() override;
    void Draw() override;
};
