// src/game/interface/PauseMenu.h

#pragma once

#include "../../GameReference.h"
#include "MainMenu.h"
#include <SDL.h>

class PauseMenu {
private:
    GameReference* m_GameWindow;
    MainMenu* m_MainMenu;
    Texture* m_TextureResume;
    Texture* m_TextureBack;
    SDL_Rect m_ResumeButtonRect;
    SDL_Rect m_BackToMenuButtonRect;

public:
    PauseMenu(GameReference* gameWindow, MainMenu* mainMenu);
    ~PauseMenu();

    void Show();
    void HandleEvent(const SDL_Event& event, bool& running, bool& menuOpen);
    void Render();
};