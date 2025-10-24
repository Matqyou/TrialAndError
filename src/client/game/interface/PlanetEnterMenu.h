// PlanetEnterMenu.h
#pragma once

#include "Interface.h"
#include <client/game/entities/cartesian/Planet.h>
#include <client/GameData.h>
#include "SDL.h"

class PlanetEnterMenu : public InterfaceElement
{
public:
    explicit PlanetEnterMenu(Interface *parent);
    ~PlanetEnterMenu();

    // Open the menu for a given planet (world coordinates)
    void Open(Planet *planet);
    void Close();

    // Draw (called from Interface)
    void Draw() override;

private:
    Planet *m_ActivePlanet{nullptr};
    SDL_Rect m_PopupRect{};
    SDL_Rect m_PopupStartButton{};
    SDL_Rect m_PopupCancelButton{};
    bool m_PopupStartHover{false};
    bool m_PopupCancelHover{false};
};
