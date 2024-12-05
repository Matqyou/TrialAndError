// src/game/interface/MainMenu.h

#pragma once

#include "../../GameReference.h"
#include <SDL.h>

class MainMenu {
private:
    GameReference* m_GameWindow;
    Texture* m_MenuTexture;
    Texture* m_TexturePlay;
    Texture* m_TextureExit;
    SDL_Rect m_PlayButtonRect;
    SDL_Rect m_ExitButtonRect;

public:
    MainMenu(GameReference* gameWindow);
    ~MainMenu();

    void Show();
    void HandleEvent(const SDL_Event& event, bool& running, bool& menuOpen);
    void Render();
};