//
// Created by Mim on 4/19/2023.
//
#include "Ammo.h"
#include <cmath>
#include <iostream>
#include "character/Character.h"
Texture* Ammo::ms_Texture = nullptr;

Ammo::Ammo(GameWorld* world, AmmoType type, double start_x, double start_y)
        : Entity(world, GameWorld::ENTTYPE_AMMO, start_x, start_y, 15, 15, 1.0){
    m_Type = type;
}
bool Ammo::TickImpact(double x, double y) {
    // Check if position collides any of the players
    auto Player = m_World->FirstPlayer();
    for (; Player; Player = (Character*)(Player->NextType())) {
        bool Collides = (Player->GetX() - 25 < x) &&
                        (Player->GetX() + 25 > x) &&
                        (Player->GetY() - 25 < y) &&
                        (Player->GetY() + 25 > y);

        if (Collides) {
            std::cout << "Ammo picked up";
            //TO DO Increase ammo amount, also check which type of ammo has been picked up aswell
            // something like if(m_Type == GLOCK_AMMO){
            // m_WeaponType->m_Ammo == m_AmmoCapacity
            // }

            delete this;
            return true;
        }
    }
    return false;
}


void Ammo::Tick() {
    TickImpact(m_x, m_y);
}