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

class Crate : public Entity {
protected:
    friend class GameWorld;
    DropType m_DropType;
    int m_ErrorType;
    Texture* m_Texture;

public:
    static LoadedSound sHitSound[3];
    static LoadedSound sBoxSound;
    static LoadedTexture sBoxTexture;
    static LoadedTexture sBreakingBox1Texture;
    static LoadedTexture sBreakingBox2Texture;

    Crate(GameWorld* world,
          const Vec2d& start_pos,
          DropType RandomDrop);
    ~Crate() override;

    // Getting
    [[nodiscard]] DropType GetDropType() const { return m_DropType; }

    // Manipulating
    void Damage(double value, Entity* damager) ;
    void Heal(double value) ;

    // Ticking
    void Tick() override;
    void Draw() override;
};
