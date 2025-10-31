// PlanetEnterMenu.h
#pragma once

#include "Interface.h"
#include <client/game/entities/cartesian/Planet.h>

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
    SDL_FRect m_PopupRect{};
    SDL_FRect m_PopupStartButton{};
    SDL_FRect m_PopupCancelButton{};
    bool m_PopupStartHover{false};
    bool m_PopupCancelHover{false};

};
