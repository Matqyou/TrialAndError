// PlanetarySystem.cpp
#include "client/GameData.h"
#include <random>
#include "PlanetaryUtils.h"
#include "PlanetaryGameWorld.h"
#include "PlanetaryCharacter.h"

// PlanetaryCharacter Implementation
PlanetaryCharacter::PlanetaryCharacter(PlanetaryGameWorld *planetaryGameWorld, Player *player, double max_health,
                                       const Vec2d &start_pos, const Vec2d &start_vel, bool is_npc)
    : Character((GameWorld *)planetaryGameWorld, player, max_health, start_pos, start_vel, is_npc),
      m_PlanetaryWorld(planetaryGameWorld->GetCurrentPlanet())
{
    if (m_PlanetaryWorld)
    {
        m_PlanetaryPos = m_PlanetaryWorld->WorldToPlanetary(start_pos);
    }
}

void PlanetaryCharacter::SetPlanetaryWorld(PlanetaryWorld *world)
{
    m_PlanetaryWorld = world;
    if (world)
    {
        m_PlanetaryPos = world->WorldToPlanetary(m_Core.Pos);
    }
}

void PlanetaryCharacter::SetPlanetaryPosition(const PlanetaryCoords &coords)
{
    m_PlanetaryPos = coords;
    m_Core.Pos = coords.ToCartesian();
}

Vec2d PlanetaryCharacter::GetUpDirection() const
{
    return m_Core.Pos.Normalize();
}

Vec2d PlanetaryCharacter::GetRightDirection() const
{
    Vec2d up = GetUpDirection();
    return Vec2d(-up.y, up.x); // Perpendicular to up direction
}

void PlanetaryCharacter::TickControls()
{
    if (m_GameController)
        TickGameControllerControls();
    else
        TickPlanetaryKeyboardControls();

    if (m_Input.m_LookingLength <= 0.6 && m_Input.m_GoingLength > 0.2)
    {
        m_Input.m_LookingX = m_Input.m_GoingX;
        m_Input.m_LookingY = m_Input.m_GoingY;
    }
}
void PlanetaryCharacter::TickPlanetaryKeyboardControls()
{
    bool Horizontal = m_Movement[CONTROL_LEFT] ^ m_Movement[CONTROL_RIGHT];
    bool Vertical   = m_Movement[CONTROL_UP]   != m_Movement[CONTROL_DOWN];
    double Unit     = Horizontal && Vertical ? M_SQRT1_2 : 1.0;

    auto* GameWorld = dynamic_cast<PlanetaryGameWorld*>(m_World);
    bool OnPlanet = GameWorld && GameWorld->GetCurrentPlanet();

    if (OnPlanet)
    {
        constexpr double moveSpeed = 0.75; // degrees per tick
        double dLon = 0.0, dLat = 0.0;

        if (Horizontal)
            dLon += m_Movement[CONTROL_RIGHT] ? Unit * moveSpeed : -Unit * moveSpeed;
        if (Vertical)
            dLat += m_Movement[CONTROL_DOWN]  ? Unit * moveSpeed : -Unit * moveSpeed;

        m_PlanetaryPos.longitude += dLon;
        m_PlanetaryPos.latitude  += dLat;
        m_PlanetaryPos.NormalizeAngle();

        // Only update Core.Pos for rendering / compatibility
        m_Core.Pos = m_PlanetaryPos.ToCartesian();

        // Clear flat movement
        m_Input.m_GoingX = m_Input.m_GoingY = m_Input.m_GoingLength = 0.0;
    }
    else
    {
        // Off-planet behavior unchanged
        Character::TickKeyboardControls();
    }

    // Aim/Look logic (unchanged)
    int XMouse, YMouse;
    SDL_GetMouseState(&XMouse, &YMouse);
    Drawing* Render = m_World->GameWindow()->Render();
    double Zoom = Render->GetZoom();
    m_Input.m_LookingX = Render->GetCameraX() - m_Core.Pos.x + (XMouse - m_World->GameWindow()->GetWidth2()) / Zoom;
    m_Input.m_LookingY = Render->GetCameraY() - m_Core.Pos.y + (YMouse - m_World->GameWindow()->GetHeight2()) / Zoom;
    m_Input.m_LookingLength = Vec2d(m_Input.m_LookingX, m_Input.m_LookingY).Length();

    if (m_Input.m_LookingLength != 0.0)
    {
        m_Input.m_LookingX /= m_Input.m_LookingLength;
        m_Input.m_LookingY /= m_Input.m_LookingLength;
    }
    else
    {
        m_Input.m_LookingX = 1.0;
        m_Input.m_LookingY = 0.0;
    }

    auto MouseState = SDL_GetMouseState(nullptr, nullptr);
    m_Input.m_Shooting = MouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
    m_Input.m_Hooking  = MouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
    m_Input.m_Sneaking = m_Movement[CONTROL_SHIFT];
}


void PlanetaryCharacter::TickCollision()
{
    Character::TickCollision();
}

void PlanetaryCharacter::Draw()
{
    Character::Draw();
}
