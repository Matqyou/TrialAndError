// src/game/interface/PauseMenu.h

#pragma once

#include "../../GameReference.h"
#include <SDL.h>
class PauseMenu
{
private:
    GameWorld *m_GameWorld;
    GameReference *m_GameWindow;
    Texture *m_TextureResume;
    Texture *m_TextureBack;
    SDL_Rect m_ResumeButtonRect;
    SDL_Rect m_BackToMenuButtonRect;
    bool m_Paused;

public:
    PauseMenu(GameWorld *GameWorld);
    ~PauseMenu();

    [[nodiscard]] bool Paused() const { return m_Paused; }
    void Show();
    void HandleEvent(const SDL_Event &event);
    void Render();
};