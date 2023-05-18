//
// Created by 11dpmbozko on 18.05.2023.
//

#include "CharacterNPC.h"
#include <cmath>

CharacterNPC::CharacterNPC(GameWorld* world, double max_health,
                           double start_x, double start_y,
                           double start_xvel, double start_yvel)
 : Character(world, nullptr, max_health, start_x, start_y, start_xvel, start_yvel) {
    m_NPC = true;
    m_NPCLastShot = 0;
}

CharacterNPC::~CharacterNPC() {

}

void CharacterNPC::TickControls() {
    auto CurrentTick = m_World->CurrentTick();

    auto Char = m_World->FirstPlayer();
    Character* ClosestChar = nullptr;
    double Closest = -1;
    for (; Char != nullptr; Char = (Character*)(Char->NextType())) {
        if (Char == this || Char->IsNPC()) continue;

        double Distance = std::sqrt(std::pow(Char->GetX() - m_x, 2) + std::pow(Char->GetY() - m_y, 2));
        if (Distance < 1000.0 && (!ClosestChar || Distance < Closest)) {
            Closest = Distance;
            ClosestChar = Char;
        }
    }

    m_Input.m_Shooting = false;
    m_Input.m_NextItem = false;

    if (!ClosestChar) {
        m_Input.m_GoingLength = 0.0;
        m_Input.m_LookingLength = 1.0;
        const double OneDegree = double(rand()%10-5) /180.0*M_PI;
        double Angle = std::atan2(m_Input.m_LookingY, m_Input.m_LookingX) + OneDegree;
        m_Input.m_LookingX = cos(Angle);
        m_Input.m_LookingY = sin(Angle);
        m_Input.m_Shooting = false;
        if (m_CurrentWeapon)
            m_Input.m_NextItem = true;
    } else {
        double TravelX = ClosestChar->GetX() - m_x;
        double TravelY = ClosestChar->GetY() - m_y;
        m_Input.m_GoingLength = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
        m_Input.m_GoingX = TravelX / m_Input.m_GoingLength;
        m_Input.m_GoingY = TravelY / m_Input.m_GoingLength;
        m_Input.m_LookingLength = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
        m_Input.m_LookingX = TravelX / m_Input.m_GoingLength;
        m_Input.m_LookingY = TravelY / m_Input.m_GoingLength;

        if (Closest <= 300.0)
            m_Input.m_GoingLength = 0.0;

        m_Input.m_Reloading = false;

        if (!m_CurrentWeapon) {
            m_Input.m_NextItem = true;
            if (CurrentTick - m_NPCLastShot > 8) {
                m_NPCLastShot = CurrentTick;
                m_Input.m_Shooting = true;
            }
        } else {
            if (!m_CurrentWeapon->Ammo()) {
                if (m_CurrentWeapon->TrueAmmo()) {
                    m_Input.m_Reloading = true;
                } else {
                    m_Input.m_NextItem = true;
                }
            } else {
                if (CurrentTick - m_NPCLastShot > (unsigned long long)(m_CurrentWeapon->TickCooldown() * 2) + rand()%10) {
                    m_NPCLastShot = CurrentTick;
                    m_Input.m_Shooting = true;
                }
            }
        }
    }
}