//
// Created by Matq on 14/04/2023.
//

#include "Hook.h"
#include "Character.h"
#include <cmath>

Hook::Hook(Character* parent) {
    m_Parent = parent;
    m_x = 0.0;
    m_y = 0.0;
    m_xvel = 0.0;
    m_yvel = 0.0;
    m_MaxLength = 300.0;
    m_HookTravelSpeed = 35.0;
    m_WallDragForce = 1.5;

    m_HookStrength = 2.0;
    m_HookerInfluenceRatio = 0.0;
    m_HookistInfluenceRatio = 0.0;
    SetInfluenceRatio(0.7);

    m_Deployed = false;
    m_Grabbed = GRABBED_NONE;
    m_GrabbedEntity = nullptr;
}

// The percentage of force that is applied to the hookist
// the remaining percentage will be applied to the hooker.
void Hook::SetInfluenceRatio(double ratio) {
    m_HookerInfluenceRatio = 1.0 - ratio;
    m_HookistInfluenceRatio = ratio;
}

// Set the hook as retracted
void Hook::Unhook() {
    m_Deployed = false;
    m_Grabbed = GRABBED_NONE;
    m_GrabbedEntity = nullptr;
}

// Set the hook as grabbed to the wall
void Hook::HookWall() {
    m_xvel = 0.0;
    m_yvel = 0.0;
    m_Grabbed = GRABBED_WALL;
}

void Hook::Tick(bool hooking, bool last_hooking) {
    GameWorld* World = m_Parent->World();

    if (!m_Deployed && hooking && !last_hooking) {
        m_Deployed = true;
        m_x = m_Parent->m_x;
        m_y = m_Parent->m_y;
        m_xvel = m_Parent->m_xLook * m_HookTravelSpeed;
        m_yvel = m_Parent->m_yLook * m_HookTravelSpeed;
    } else if (m_Deployed && !hooking && last_hooking) {  // Instant retraction for now
        Unhook();
    }

    if (!m_Deployed)
        return;

    if (m_Grabbed == GRABBED_NONE) {
        m_x += m_xvel;
        m_y += m_yvel;
    }

    double TravelX = m_x - m_Parent->m_x;
    double TravelY = m_y - m_Parent->m_y;
    double Length = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
    if (Length != 0.0) {
        TravelX /= Length;
        TravelY /= Length;
    }

    if (m_Grabbed == GRABBED_NONE) {
        // Make sure hook isn't longer than it is allowed to be
        if (Length > m_MaxLength) {
            m_x = m_Parent->m_x + TravelX * m_MaxLength;
            m_y = m_Parent->m_y + TravelY * m_MaxLength;
            m_xvel -= TravelX * 2.0;
            m_yvel -= TravelY * 2.0;
        }

        // Hook snaps to player - idk if its good or not cus i havent made it yet
        Character* Player = m_Parent->World()->FirstPlayer();
        for (; Player; Player = (Character*)(Player->NextType())) {
            if (Player == m_Parent)
                continue;

            if (Player->PointCollides(m_x, m_y)) {
                m_Grabbed = GRABBED_ENTITY;
                m_GrabbedEntity = Player;
                break;
            }
        }

        if (m_Grabbed == GRABBED_NONE) {
            // Hook snaps to wall - fix later cus ugly, prob fix when adding tiles and stuff cus doesnt rly matter tbh
            if (m_x < 0.0) {
                m_x = 0.0;
                HookWall();
            } else if (m_y < 0.0) {
                m_y = 0.0;
                HookWall();
            } else if (m_x > World->Width()) {
                m_x = World->Width();
                HookWall();
            } else if (m_y > World->Height()) {
                m_y = World->Height();
                HookWall();
            }
        }
    } else if (m_Grabbed == GRABBED_ENTITY) {
        m_x = m_GrabbedEntity->GetX();
        m_y = m_GrabbedEntity->GetY();
        if (m_GrabbedEntity->EntityType() == GameWorld::ENTTYPE_CHARACTER) {
            auto Player = (Character*)(m_GrabbedEntity);
            double Acceleration = m_HookStrength * Length / m_MaxLength * (1 - m_HookerInfluenceRatio);
            double Influence = m_HookStrength * Length / m_MaxLength * m_HookerInfluenceRatio;
            Player->Accelerate(-TravelX * Acceleration, -TravelY * Acceleration);
            m_Parent->Accelerate(TravelX * Influence, TravelY * Influence);
        }
    } else if (m_Grabbed == GRABBED_WALL) {
        m_Parent->m_xvel += TravelX * m_WallDragForce;
        m_Parent->m_yvel += TravelY * m_WallDragForce;
    }
}