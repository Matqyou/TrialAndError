// PlanetarySystem.cpp
#include "client/GameData.h"
#include <random>
#include "PlanetaryUtils.h"
#include "PlanetaryGameWorld.h"
#include "PlanetaryCharacter.h"

// PlanetaryCharacter Implementation
PlanetaryCharacter::PlanetaryCharacter(PlanetaryGameWorld *planetaryGameWorld,
                                       Player *player,
                                       double max_health,
                                       const Vec2d &start_pos,
                                       const Vec2d &start_vel,
                                       bool is_npc)
    : Character((GameWorld *)planetaryGameWorld, player, max_health, start_pos, start_vel, is_npc),
      m_PlanetaryWorld(planetaryGameWorld->GetCurrentPlanet())
{
    if (!m_PlanetaryWorld)
        return; 
    PlanetaryCoords coords = m_PlanetaryWorld->WorldToPlanetary(start_pos);

    // Sync spherical and cartesian position
    SetPlanetaryPosition(coords);
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
    PlanetaryGameWorld *world = dynamic_cast<PlanetaryGameWorld *>(m_World);
    PlanetaryWorld *planet = world ? world->GetCurrentPlanet() : nullptr;
    if (!planet)
        return;

    PlanetaryCoords pos = m_PlanetaryPos;

    // --- Movement speed (depends on planet size) ---
    double baseSpeed = 10;
    double planetRadius = planet->GetConfig().radius;
    double moveSpeed = baseSpeed / (planetRadius * 10);
    moveSpeed = std::clamp(moveSpeed, 0.00005, 100000.0);

    double latChange = 0.0;
    double longChange = 0.0;

    // --- Keyboard input for movement ---
    if (m_Movement[CONTROL_LEFT])
        longChange -= moveSpeed;
    if (m_Movement[CONTROL_RIGHT])
        longChange += moveSpeed;
    if (m_Movement[CONTROL_UP])
        latChange += moveSpeed;
    if (m_Movement[CONTROL_DOWN])
        latChange -= moveSpeed;

    pos.latitude += latChange;
    pos.longitude += longChange;
    world->GetCurrentPlanet()->AddAngles(longChange, latChange);
    // Clamp latitude (radians) and wrap longitude (radians)
    pos.latitude = std::clamp(pos.latitude, -M_PI / 2.0 + 0.0001, M_PI / 2.0 - 0.0001);
    // Normalize longitude into [0, 2PI)
    pos.longitude = std::fmod(pos.longitude, 2.0 * M_PI);
    if (pos.longitude < 0)
        pos.longitude += 2.0 * M_PI;

    m_PlanetaryPos = pos;

    // --- Convert spherical to world-space Cartesian position ---
    Vec2d cartesian = pos.ToCartesian();
    m_Core.Pos.x = cartesian.x;
    m_Core.Pos.y = cartesian.y;

    // --- Update standard movement state ---
    bool movingHorizontally = m_Movement[CONTROL_LEFT] ^ m_Movement[CONTROL_RIGHT];
    bool movingVertically = m_Movement[CONTROL_UP] ^ m_Movement[CONTROL_DOWN];

    if (movingHorizontally || movingVertically)
        m_Input.m_GoingLength = 10.0;
    else
        m_Input.m_GoingLength = 0.0;

    m_Input.m_GoingX = movingHorizontally
                           ? (m_Movement[CONTROL_LEFT] ? -10.0 : 10.0)
                           : 0.0;
    m_Input.m_GoingY = movingVertically
                           ? (m_Movement[CONTROL_UP] ? -10.0 : 10.0)
                           : 0.0;

    // --- Mouse aim direction (relative to camera) ---
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    Drawing *render = world->GameWindow()->Render();
    double zoom = render->GetZoom();

    m_Input.m_LookingX = render->GetCameraX() - m_Core.Pos.x + (mouseX - world->GameWindow()->GetWidth2()) / zoom;
    m_Input.m_LookingY = render->GetCameraY() - m_Core.Pos.y + (mouseY - world->GameWindow()->GetHeight2()) / zoom;
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

    // --- Other actions ---
    auto mouseState = SDL_GetMouseState(nullptr, nullptr);
    m_Input.m_Shooting = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
    m_Input.m_Hooking = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
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
