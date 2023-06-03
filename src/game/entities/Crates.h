//
// Created by Mim on 5/08/2023.
//

#ifndef TRIALANDERROR_CRATES_H
#define TRIALANDERROR_CRATES_H

#include "Entity.h"
#include "SDL.h"
#include "AmmoBox.h"
#include "Error.h"

enum DropType {
    AMMO,
    ERROR,
    NUM_DROPS
};

class Crates : public Entity {
protected:
    DropType m_Type;
    int typeID;
    double m_Health;
    Texture** m_Texture;

public:
    static Sound* ms_HitSound;
    static Sound* ms_BoxSound;
    static Texture* ms_TextureBox;
    static Texture* ms_TextureBreakingBox1;
    static Texture* ms_TextureBreakingBox2;

    ~Crates();
    Crates(GameWorld* world, double start_x, double start_y, double Health, int RandomDrop);
    DropType Type() const { return m_Type; }
    void DamageCrate(double Damage);
    void Tick() override;
    void Draw() override;
};
#endif //TRIALANDERROR_CRATES_H
