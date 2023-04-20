//
// Created by Mim on 4/19/2023.
//
#include "Ammo.h"
#include "character/Character.h"
#include <cmath>
#include <iostream>
Texture* Ammo::ms_Texture = nullptr;

Ammo::Ammo(GameWorld* world, AmmoType type, double start_x, double start_y, double AmmoCount)
        : Entity(world, GameWorld::ENTTYPE_AMMO, start_x, start_y, 15, 15, 1.0){
    m_AmmoCount = AmmoCount;
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
            std::cout << "Ammo picked up"<< std::endl;
            if(Player->AmmoPickup(this)) {delete this; return true;}
            // TODO Increase ammo amount, also check which type of ammo has been picked up aswell
            // something like if(m_Type == GLOCK_AMMO){
            // m_WeaponType->m_Ammo == m_Amm oCapacity
            // }
        }
    }
    return false;
}
void Ammo::SetAmmoCount(double collected){
    m_AmmoCount -= collected;
}

void Ammo::Tick() {
    TickImpact(m_x, m_y);
}