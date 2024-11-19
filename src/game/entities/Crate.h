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

class Crate : public Entity, public virtual HasHealth {
protected:
    DropType m_Type;
    int typeID;
    Texture** m_Texture;

public:
    static Sound* ms_HitSound;
    static Sound* ms_BoxSound;
    static Texture* ms_TextureBox;
    static Texture* ms_TextureBreakingBox1;
    static Texture* ms_TextureBreakingBox2;

    Crate(GameWorld* world,
          const Vec2d& start_pos,
          double Health,
          DropType RandomDrop);
    ~Crate();

    // Getting
    [[nodiscard]] DropType Type() const { return m_Type; }

    // Manipulating
    void Damage(double value) override;

    // Ticking
    void Tick() override;
    void Draw() override;
};
