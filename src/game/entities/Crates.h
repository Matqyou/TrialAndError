//
// Created by Mim on 5/08/2023.
//

#ifndef TRIALANDERROR_CRATES_H
#define TRIALANDERROR_CRATES_H

#include "Entity.h"
#include "SDL.h"
#include "Ammo.h"
#include "Error.h"

enum DropType {
    AMMO,
    ERROR,
    NUM_DROPS
};

class Crates : public Entity {
protected:
    DropType m_Type;
    double m_Health;
    Texture** m_Texture;
    void TickImpact(double x, double y);
public:
    static Sound* ms_HitSound;
    static Texture* ms_TextureBox;
    static Sound* ms_PickupSounds[7];
    ~Crates();
    Crates(GameWorld* world, double start_x, double start_y, double Health);
    DropType Type() const{ return m_Type; }
    void DamageCrate(double Damage);
    void Tick() override;
    void Draw() override;
};
#endif //TRIALANDERROR_CRATES_H
