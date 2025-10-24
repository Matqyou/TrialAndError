// CartesianGameWorld.h
#pragma once

#include "client/GameData.h"
#include "GameWorld.h"
#include "shared/utility/Vec2.h"
#include "entities/Planet.h"
#include <vector>

struct PlanetEntityRef { Planet *planet; };

class CartesianGameWorld : public GameWorld
{
public:
    CartesianGameWorld(GameData *game_window, int width, int height);
    ~CartesianGameWorld() override;

    void Tick() override;
    void Draw() override;

private:
    struct PlanetInfo { Vec2d pos; double radius; SDL_Color color; };
    std::vector<PlanetInfo> m_Planets;

    // Spawned entity pointers
    std::vector<Planet*> m_PlanetEntities;

    // Popup state when near a planet
    bool m_ShowEnterPopup = false;
    Planet *m_ActivePlanet = nullptr;
    SDL_Rect m_PopupRect{};
    SDL_Rect m_PopupStartButton{};
    SDL_Rect m_PopupCancelButton{};
    bool m_PopupStartHover = false;
    bool m_PopupCancelHover = false;
};
