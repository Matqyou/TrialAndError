//
// Created by 11dpmbozko on 18.05.2023.
//

#include "CharacterNPC.h"
#include <cmath>

CharacterNPC::CharacterNPC(GameWorld *world,
                           double max_health,
                           const Vec2d &start_pos,
                           const Vec2d &start_vel,
                           NPCType ai_type,
                           bool is_boss)
 : Character(world,
                nullptr,
                max_health,
                start_pos,
                start_vel,
                true) {
    m_LastAttacker = nullptr;
    m_AIType = ai_type;
    m_IsBoss = is_boss;
    m_ColorHue = is_boss ? 250.0 + (rand() % 90) - 45 : 120.0 + (rand() % 90) - 45;
}

CharacterNPC::~CharacterNPC() = default;

void CharacterNPC::EventDeath()
{
    Character::EventDeath();

    m_World->AddScore(m_IsBoss ? 20 * 5 : 20);

    Player *killer = m_LastAttacker; // Use m_Player directly
    if (killer)
    {
        m_World->EnemyKilled(killer, this);
    }

    if (rand() % 100 <= 20)
        new Crate(m_World, m_Core.Pos, DropType(rand() % 2));

    int NumNPCS = 0;
    for (auto Char = m_World->FirstCharacter(); Char; Char = (Character *)Char->NextType())
    {
        if (Char->IsNPC() && Char->IsAlive())
        {
            NumNPCS++;
        }
    }
    if (NumNPCS == 0)
    {
        m_World->EnemiesKilled();
        for (auto Char = m_World->FirstCharacter(); Char; Char = (Character *)Char->NextType())
            Char->RemoveCombat();
    }
}
void CharacterNPC::UpdateAttacker(Player *attacker)
{
    m_LastAttacker = attacker;
}

void CharacterNPC::TickControls()
{
    if (m_AIType == NPC_DUMMY)
        return;

    auto CurrentTick = m_World->GetTick();

    auto Char = m_World->FirstCharacter();
    Character *ClosestChar = nullptr;
    double Closest = -1;
    for (; Char != nullptr; Char = (Character *)(Char->NextType()))
    {
        if (Char == this || Char->IsNPC())
            continue;

        auto &EntCore = Char->GetDirectionalCore();
        double Distance = DistanceVec2(m_Core.Pos, EntCore.Pos);
        if (Distance < 1000.0 && (!ClosestChar || Distance < Closest))
        {
            Closest = Distance;
            ClosestChar = Char;
        }
    }

    m_Input.m_Shooting = false;
    m_Input.m_NextItem = false;

    if (!ClosestChar)
    {
        m_Input.m_GoingLength = 0.0;
        m_Input.m_LookingLength = 1.0;
        const double OneDegree = double(rand() % 10 - 5) / 180.0 * M_PI;
        double Angle = std::atan2(m_Input.m_LookingY, m_Input.m_LookingX) + OneDegree;
        m_Input.m_LookingX = cos(Angle);
        m_Input.m_LookingY = sin(Angle);
        m_Input.m_Shooting = false;
    }
    else
    {
        EntityCore &ClosestCore = ClosestChar->GetCore();
        double TravelX = ClosestCore.Pos.x - m_Core.Pos.x;
        double TravelY = ClosestCore.Pos.y - m_Core.Pos.y;
        m_Input.m_GoingLength = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
        m_Input.m_GoingX = TravelX / m_Input.m_GoingLength * (m_CurrentWeapon ? 1 : 0.5);
        m_Input.m_GoingY = TravelY / m_Input.m_GoingLength * (m_CurrentWeapon ? 1 : 0.5);
        m_Input.m_LookingLength = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
        m_Input.m_LookingX = TravelX / m_Input.m_GoingLength;
        m_Input.m_LookingY = TravelY / m_Input.m_GoingLength;

        m_Input.m_Reloading = false;

        if (!m_CurrentWeapon)
        {
            m_Input.m_NextItem = true;
            if (CurrentTick - m_Hands.LastFisted() > 20)
                m_Input.m_Shooting = true;
        }
        else
        {
            if (Closest <= 300.0)
                m_Input.m_GoingLength = 0.0;

            if (!m_CurrentWeapon->GetMagAmmo())
            {
                if (m_CurrentWeapon->GetTrueAmmo())
                {
                    m_Input.m_Reloading = true;
                }
                else
                {
                    m_Input.m_NextItem = true;
                }
            }
            else
            {
                if (m_CurrentWeapon->IsAutomatic() || CurrentTick - m_CurrentWeapon->LastShot() > (unsigned long long)((double(m_CurrentWeapon->TickCooldown()) - 1500.0 / Closest) * 10))
                {
                    m_Input.m_Shooting = true;
                }
            }
        }
    }
}

const char* CharacterNPC::toString() const {
    return "CharacterNPC";
}