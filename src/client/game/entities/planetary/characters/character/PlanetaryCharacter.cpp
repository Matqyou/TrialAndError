// PlanetarySystem.cpp
#include "PlanetaryCharacter.h"
#include <client/game/GameReference.h>
#include <client/game/world/planetary/PlanetaryUtils.h>
#include <client/game/world/planetary/PlanetaryGameWorld.h>

#include <random>

// PlanetaryCharacter Implementation
PlanetaryCharacter::PlanetaryCharacter(PlanetaryGameWorld *planetaryGameWorld,
									   Player *player,
									   double max_health,
									   const Vec2f& start_pos,
									   const Vec2f& start_vel,
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
		m_PlanetaryPos = world->WorldToPlanetary(m_Core.pos);
	}
}

void PlanetaryCharacter::SetPlanetaryPosition(const PlanetaryCoords& coords)
{
	m_PlanetaryPos = coords;
	m_Core.pos = coords.ToCartesian();
}

Vec2f PlanetaryCharacter::GetUpDirection() const
{
	return m_Core.pos.NormalizeF();
}

Vec2f PlanetaryCharacter::GetRightDirection() const
{
	Vec2f up = GetUpDirection();
	return { -up.y, up.x }; // Perpendicular to up direction
}

void PlanetaryCharacter::TickControls()
{
//    if (m_GameController)
//        TickGameControllerControls();
//    else
	TickPlanetaryKeyboardControls();

	if (input.looking_direction.Length() <= 0.6 && input.going_direction.Length() > 0.2)
	{
		input.looking_direction = input.going_direction;
//		input.m_LookingX = input.m_GoingX;
//		input.m_LookingY = input.m_GoingY;
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
	float planetRadius = planet->GetConfig().radius;

	// Degrees per second converted to radians per tick (assuming ~60 FPS)
	float angularDegreesPerSecond = 1.0f; // feel free to tweak
	float move_speed = (angularDegreesPerSecond * static_cast<float>(M_PI) / 180.0f) * (650.0f / planetRadius);
	move_speed = std::clamp(move_speed, 0.0001f, 0.05f);

	float lat_change = 0.0f;
	float long_change = 0.0f;

	// --- Keyboard input for movement ---
	if (m_Movement[CONTROL_LEFT])
		long_change += move_speed;
	if (m_Movement[CONTROL_RIGHT])
		long_change -= move_speed;
	if (m_Movement[CONTROL_UP])
		lat_change -= move_speed;
	if (m_Movement[CONTROL_DOWN])
		lat_change += move_speed;

	pos.latitude += lat_change;
	pos.longitude += long_change;

	world->GetCurrentPlanet()->AddAngles(-long_change, -lat_change);

	m_PlanetaryPos = pos;

	// --- Mouse aim direction (relative to camera) ---
	Vec2f mouse_pos = Application.GetMousePosition();

	Drawing *drawing = Application.GetDrawing();
	Camera& camera = GameReference.GetCamera();
//	double zoom = drawing->GetZoom();

	Vec2f world_mouse_pos = camera.ScreenToCameraPoint(mouse_pos);
	input.looking_direction = (m_Core.pos - world_mouse_pos).NormalizeF();
//	input.m_LookingX = drawing->GetCameraX() - m_Core.pos.x + (mouseX - world->GameWindow()->GetWidth2()) / zoom;
//	input.m_LookingY = drawing->GetCameraY() - m_Core.pos.y + (mouseY - world->GameWindow()->GetHeight2()) / zoom;
//	input.m_LookingLength = Vec2d(input.m_LookingX, input.m_LookingY).Length();

//	if (input.m_LookingLength != 0.0)
//	{
//		input.m_LookingX /= input.m_LookingLength;
//		input.m_LookingY /= input.m_LookingLength;
//	}
//	else
//	{
//		input.m_LookingX = 1.0;
//		input.m_LookingY = 0.0;
//	}

	// --- Other actions ---
	auto mouse_state = SDL_GetMouseState(nullptr, nullptr);
	input.shooting = mouse_state & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
	input.hooking = mouse_state & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);
	input.sneaking = m_Movement[CONTROL_SNEAK];
}

void PlanetaryCharacter::TickCollision()
{
	Character::TickCollision();
}

void PlanetaryCharacter::Draw()
{
	// Use planetary coordinates for drawing so the character is rendered on the spherical surface
	// without permanently changing the entity core used by physics.
	Vec2f original_pos = m_Core.pos;
	Vec2f cart = m_PlanetaryPos.ToCartesian();
	m_Core.pos = cart;

	Character::Draw();

	// Restore original position used for physics and other logic
	m_Core.pos = original_pos;
}

void PlanetaryCharacter::DrawNameplate()
{
	// Assume planetary characters always have a planet; draw lon/lat in the coordinate plate
	Drawing *drawing = Application.GetDrawing();

	double NameVisibility = m_World->GetNamesShown();
	if (NameVisibility == 0.0)
		return;

	int Opacity = int(NameVisibility * 255.0);

	Texture *NamePlateTexture = m_Player ? m_Player->GetNamePlate()->RequestUpdate() : ms_BotNamePlate->GetTexture();

	float NamePlateW = NamePlateTexture->GetWidth();
	float NamePlateH = NamePlateTexture->GetHeight();
	SDL_FRect NamePlateRect = {
		m_Core.pos.x - NamePlateW / 2.0f,
		m_Core.pos.y - m_Core.size.y / 2.0f - NamePlateH,
		NamePlateW, NamePlateH
	};

	SDL_SetTextureAlphaMod(NamePlateTexture->SDLTexture(), Opacity);
	drawing->RenderTexture(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect, GameReference.GetCamera());

	// Format planetary coordinates
	auto pc = m_PlanetaryPos;
	// Show degrees for readability
	double lonDeg = pc.longitude * 180.0 / M_PI;
	double latDeg = pc.latitude * 180.0 / M_PI;
	auto CoordinateText = FString("%+.2fLon, %+.2fLat", lonDeg, latDeg);
	m_CoordinatePlate->SetText(CoordinateText);
	m_CoordinatePlate->SetColor(m_NameplateColor);
	Texture *CoordinateTexture = m_CoordinatePlate->RequestUpdate();

	float CoordinatePlateW = NamePlateTexture->GetWidth();
	float CoordinatePlateH = NamePlateTexture->GetHeight();
	SDL_FRect CoordinateRect = {
		m_Core.pos.x - CoordinatePlateW / 2.0f,
		NamePlateRect.y - CoordinatePlateH,
		CoordinatePlateW, CoordinatePlateH
	};
	SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), Opacity);
	drawing->RenderTexture(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect, GameReference.GetCamera());
}

void PlanetaryCharacter::Tick(double seconds_elapsed)
{
	// keep basic tick but skip physics integration of 2D pos for planetary characters
	// we'll sync to planetary pos each tick
	TickControls();
	// Sync core pos
	if (m_PlanetaryWorld)
		m_Core.pos = m_PlanetaryPos.ToCartesian();

	// Run base tick for animations, health, etc., but avoid applying Cartesian velocity
	Character::Tick(seconds_elapsed);
}
