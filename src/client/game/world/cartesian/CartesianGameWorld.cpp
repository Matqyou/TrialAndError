// CartesianGameWorld.cpp
#include "CartesianGameWorld.h"
#include <client/game/entities/planetary/characters/character/PlanetaryCharacter.h>
#include <client/game/entities/characters/character/Character.h>
#include <client/game/world/planetary/PlanetaryGameWorld.h>
#include <client/game/ui/PlanetEnterMenu.h>
#include <client/core/Assets.h>

CartesianGameWorld::CartesianGameWorld(int width, int height)
    : GameWorld(width, height)
{
    // Create some dummy planets in the world (with color)
    m_Planets.push_back({Vec2f(width * 3, height * 4), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2f(width * 5, height * 5), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2f(width, height), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2f(width * 0.7, height * 0.6), 64, SDL_Color{200, 160, 255, 255}});

    // Spawn Planet entities
    for (size_t i = 0; i < m_Planets.size(); ++i)
    {
        auto &p = m_Planets[i];
        // Choose a plausible planet texture; fall back to no texture if missing
        std::string tex = (i == 0) ? std::string("backgrounds.planet2") : std::string("backgrounds.planet1");
        Planet *pe = new Planet(this, p.pos, p.radius, p.color, tex);
        m_PlanetEntities.push_back(pe);
    }

    // Create the interface menu for entering planets (inactive until opened)
    m_PlanetEnterMenu = new PlanetEnterMenu(GameReference.GetInterface());
}

CartesianGameWorld::~CartesianGameWorld()
{
    for (auto *p : m_PlanetEntities)
        delete p;

    if (m_PlanetEnterMenu)
        delete m_PlanetEnterMenu;
}

void CartesianGameWorld::Tick(double seconds_elapsed)
{
    GameWorld::Tick(seconds_elapsed);

	auto player = players.front();
    if (!player || !player->GetCharacter())
        return;

	// Use the character's world position for proximity checks (not the camera target)
    Character *character = player->GetCharacter();
    Vec2f character_pos = character->GetCore().pos;

    // If the interface menu is open, let it handle input/rendering; skip further checks
    if (m_PlanetEnterMenu && m_PlanetEnterMenu->IsActive())
        return;

    // Detect proximity to any planet
    for (auto *p : m_PlanetEntities)
    {
        // Simple proximity check (same as Overworld): distance from player to planet center
        Vec2f difference = character_pos - p->GetCore().pos;
        if (difference.LengthF() < p->Radius() + 8.0f)
        {
            if (m_PlanetEnterMenu)
                m_PlanetEnterMenu->Open(p);
            break;
        }
    }
}

void CartesianGameWorld::Draw()
{
    Drawing *drawing = Application.GetDrawing();

    drawing->SetColor(10, 10, 30, 255);
    drawing->Clear();

    // Draw planets via their entities
    for (auto *pe : m_PlanetEntities)
        pe->Draw();

    // Draw rest via base (players, UI)
    GameWorld::Draw();
}