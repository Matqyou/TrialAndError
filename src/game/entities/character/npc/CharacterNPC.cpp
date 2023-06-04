//
// Created by 11dpmbozko on 18.05.2023.
//

#include "CharacterNPC.h"
#include <cmath>

CharacterNPC::CharacterNPC(GameWorld* world, double max_health,
                           double start_x, double start_y,
                           double start_xvel, double start_yvel,
                           NPCType ai_type, bool is_boss)
    : Character(world, nullptr, max_health, start_x, start_y, start_xvel, start_yvel) {
    m_NPC = true;
    m_NPCLastShot = 0;
    m_AIType = ai_type;
    m_IsBoss = is_boss;
    m_ColorHue = is_boss ? 250.0 + (rand() % 90) - 45 : 120.0 + (rand() % 90) - 45;
}

CharacterNPC::~CharacterNPC() = default;

void CharacterNPC::TickControls() {
    if (m_AIType == NPC_DUMMY)
        return;

    auto CurrentTick = m_World->GetTick();

    auto Char = m_World->FirstCharacter();
    Character* ClosestChar = nullptr;
    double Closest = -1;
    for (; Char != nullptr; Char = (Character*) (Char->NextType())) {
        if (Char == this || Char->IsNPC()) continue;

        auto& EntCore = Char->GetDirectionalCore();
        double Distance = DistanceVec2(m_Core.Pos, EntCore.Pos);
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
        const double OneDegree = double(rand() % 10 - 5) / 180.0 * M_PI;
        double Angle = std::atan2(m_Input.m_LookingY, m_Input.m_LookingX) + OneDegree;
        m_Input.m_LookingX = cos(Angle);
        m_Input.m_LookingY = sin(Angle);
        m_Input.m_Shooting = false;
    } else {
        EntityCore& ClosestCore = ClosestChar->GetCore();
        double TravelX = ClosestCore.Pos.x - m_Core.Pos.x;
        double TravelY = ClosestCore.Pos.y - m_Core.Pos.y;
        m_Input.m_GoingLength = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
        m_Input.m_GoingX = TravelX / m_Input.m_GoingLength * (m_CurrentWeapon ? 1 : 0.5);
        m_Input.m_GoingY = TravelY / m_Input.m_GoingLength * (m_CurrentWeapon ? 1 : 0.5);
        m_Input.m_LookingLength = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
        m_Input.m_LookingX = TravelX / m_Input.m_GoingLength;
        m_Input.m_LookingY = TravelY / m_Input.m_GoingLength;

        m_Input.m_Reloading = false;

        if (!m_CurrentWeapon) {
            m_Input.m_NextItem = true;
            if (CurrentTick - m_NPCLastShot > 20) {
                m_NPCLastShot = CurrentTick;
                m_Input.m_Shooting = true;
            }
        } else {
            if (Closest <= 300.0)
                m_Input.m_GoingLength = 0.0;

            if (!m_CurrentWeapon->Ammo()) {
                if (m_CurrentWeapon->TrueAmmo()) {
                    m_Input.m_Reloading = true;
                } else {
                    m_Input.m_NextItem = true;
                }
            } else {
                if (m_CurrentWeapon->IsAutomatic() || CurrentTick - m_NPCLastShot
                    > (unsigned long long) (m_CurrentWeapon->TickCooldown() * 10 - 150.0 / Closest)) {
                    m_NPCLastShot = CurrentTick;
                    m_Input.m_Shooting = true;
                }
            }
        }
    }
}

void CharacterNPC::Tick() {
    TickHealth();
    TickControls(); // Parse the inputs of each device keyboard/controller/AI
    TickProcessInputs(); // Do stuff depending on the current held buttons
    TickHook();  // Move hook and or player etc.
    TickCurrentWeapon(); // Shoot accelerate reload etc.
    m_Hands.Tick();
    TickErrorTimers(); // Ticks timer for errors
    // Need every character to get here..
    // then we apply the accelerations of all
    // hooks and continue with the code below v v v
    // like collisions and velocity tick

    TickCollision();
    TickVelocity();  // Move the character entity
    TickWalls();  // Check if colliding with walls

    if ((int) (m_Health) != (int) (m_LastHealth)) m_HealthInt->FlagForUpdate();
    if (m_World->GetNamesShown() > 0.05 &&
        ((int) (m_Core.Pos.x) != (int) (m_LastCore.Pos.x) ||
         (int) (m_Core.Pos.y) != (int) (m_LastCore.Pos.y)))
        m_CoordinatePlate->FlagForUpdate();

    m_LastHealth = m_Health;
    TickLastCore();
    memcpy(&m_LastInput, &m_Input, sizeof(CharacterInput));

    m_HitTicks -= 1;
    if (m_HitTicks < 0)
        m_HitTicks = 0;

    if (m_Health <= 0.0) {
        m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(ms_DeathSound);
        m_Alive = false;
        if (rand() % 100 <= 20) new Crate(m_World, m_Core.Pos.x, m_Core.Pos.y, 20.0, rand() % 2);

        int NumNPCS = 0;
        for (auto Char = m_World->FirstCharacter(); Char; Char = (Character*) Char->NextType()) {
            if (Char->IsNPC() && Char->IsAlive())
                NumNPCS++;
        }
        m_World->AddScore(m_IsBoss ? 20 * 5 : 20);
        if (NumNPCS == 0) {
            m_World->EnemiesKilled();
            for (auto Char = m_World->FirstCharacter(); Char; Char = (Character*) Char->NextType())
                Char->RemoveCombat();
        }
    }
}