// CartesianGameWorld.cpp
#include "CartesianGameWorld.h"
#include <client/core/Assets.h>
#include <client/game/entities/characters/character/Character.h>
#include <client/core/Drawing.h>
#include <client/game/world/planetary/PlanetaryGameWorld.h>
#include <client/game/entities/planetary/characters/character/PlanetaryCharacter.h>
#include <client/game/interface/PlanetEnterMenu.h>

CartesianGameWorld::CartesianGameWorld(GameData *game_window, int width, int height)
    : GameWorld(game_window, width, height)
{
    // Create some dummy planets in the world (with color)
    m_Planets.push_back({Vec2d(width * 3, height * 4), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2d(width * 5, height * 5), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2d(width, height), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2d(width * 0.7, height * 0.6), 64, SDL_Color{200, 160, 255, 255}});

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
    m_PlanetEnterMenu = new PlanetEnterMenu(GameWindow()->GetInterface());
}

CartesianGameWorld::~CartesianGameWorld()
{
    for (auto *p : m_PlanetEntities)
        delete p;
    if (m_PlanetEnterMenu)
        delete m_PlanetEnterMenu;
}

void CartesianGameWorld::Tick()
{
    GameWorld::Tick();

    if (!FirstPlayer())
        return;
    Character *c = FirstPlayer()->GetCharacter();
    if (!c)
        return;

    // Use the character's world position for proximity checks (not the camera target)
    Vec2d ppos = c->GetCore().Pos;

    // If the interface menu is open, let it handle input/rendering; skip further checks
    if (m_PlanetEnterMenu && m_PlanetEnterMenu->IsActive())
        return;

    // Detect proximity to any planet
    for (auto *p : m_PlanetEntities)
    {
        // Simple proximity check (same as Overworld): distance from player to planet center
        Vec2d d = ppos - p->GetCore().Pos;
        if (d.Length() < p->Radius() + 8)
        {
            if (m_PlanetEnterMenu)
                m_PlanetEnterMenu->Open(p);
            break;
        }
    }
}

void CartesianGameWorld::Draw()
{
    Drawing *render = GameWindow()->Render();
    SDL_Renderer *r = GameWindow()->Renderer();
    render->SetColor(10, 10, 30, 255);
    render->Clear();

    // Draw planets via their entities
    for (auto *pe : m_PlanetEntities)
    {
        pe->Draw();
    }

    // Draw rest via base (players, UI)
    GameWorld::Draw();
}