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
    double planetRadius = planet->GetConfig().radius;

    // Degrees per second converted to radians per tick (assuming ~60 FPS)
    double angularDegreesPerSecond = 1.0; // feel free to tweak
    double moveSpeed = (angularDegreesPerSecond * M_PI / 180.0) * (650.0 / planetRadius);
    moveSpeed = std::clamp(moveSpeed, 0.0001, 0.05);

    double latChange = 0.0;
    double longChange = 0.0;

    // --- Keyboard input for movement ---
    if (m_Movement[CONTROL_LEFT])
        longChange += moveSpeed;
    if (m_Movement[CONTROL_RIGHT])
        longChange -= moveSpeed;
    if (m_Movement[CONTROL_UP])
        latChange -= moveSpeed;
    if (m_Movement[CONTROL_DOWN])
        latChange += moveSpeed;

    pos.latitude += latChange;
    pos.longitude += longChange;

    world->GetCurrentPlanet()->AddAngles(-longChange, -latChange);

    m_PlanetaryPos = pos;

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
    // Use planetary coordinates for drawing so the character is rendered on the spherical surface
    // without permanently changing the entity core used by physics.
    Vec2d originalPos = m_Core.Pos;
    Vec2d cart = m_PlanetaryPos.ToCartesian();
    m_Core.Pos = cart;

    Character::Draw();

    // Restore original position used for physics and other logic
    m_Core.Pos = originalPos;
}

void PlanetaryCharacter::DrawNameplate()
{
    // Assume planetary characters always have a planet; draw lon/lat in the coordinate plate
    Drawing *Render = m_World->GameWindow()->Render();

    double NameVisibility = m_World->GetNamesShown();
    if (NameVisibility == 0.0)
        return;

    int Opacity = int(NameVisibility * 255.0);

    Texture *NamePlateTexture = m_Player ? m_Player->GetNamePlate()->RequestUpdate() : ms_BotNamePlate->GetTexture();

    int NamePlateW = NamePlateTexture->GetWidth();
    int NamePlateH = NamePlateTexture->GetHeight();
    SDL_Rect NamePlateRect = {int(m_Core.Pos.x - NamePlateW / 2.0),
                              int(m_Core.Pos.y - m_Core.Size.y / 2.0 - NamePlateH),
                              NamePlateW, NamePlateH};

    SDL_SetTextureAlphaMod(NamePlateTexture->SDLTexture(), Opacity);
    Render->RenderTextureCamera(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect);

    // Format planetary coordinates
    auto pc = m_PlanetaryPos;
    // Show degrees for readability
    double lonDeg = pc.longitude * 180.0 / M_PI;
    double latDeg = pc.latitude * 180.0 / M_PI;
    auto CoordinateText = FString("%+.2fLon, %+.2fLat", lonDeg, latDeg);
    m_CoordinatePlate->SetText(CoordinateText);
    m_CoordinatePlate->SetColor(m_NameplateColor);
    Texture *CoordinateTexture = m_CoordinatePlate->RequestUpdate();

    int CoordinatePlateW = NamePlateTexture->GetWidth();
    int CoordinatePlateH = NamePlateTexture->GetHeight();
    SDL_Rect CoordinateRect = {int(m_Core.Pos.x - CoordinatePlateW / 2.0),
                               int(NamePlateRect.y - CoordinatePlateH),
                               CoordinatePlateW, CoordinatePlateH};
    SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), Opacity);
    Render->RenderTextureCamera(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect);
}
