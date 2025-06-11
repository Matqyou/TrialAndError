// src/game/interface/GameSelectMenu.h

#pragma once

#include "../../GameReference.h"
#include <SDL.h>
#include <chrono>

class GameModeMenu {
private:
    GameReference* m_GameWindow;
    static LoadedTexture sMenuTexture;
    static LoadedTexture sTextureTitle;
    static LoadedTexture sTexturePlay;
    static LoadedTexture sTextureExit;
    SDL_Rect m_TitleRect;
    SDL_Rect m_PlayButtonRect;
    SDL_Rect m_ExitButtonRect;

    std::vector<std::tuple<Vec2d, Vec2d, double>> m_Stars;

public:
    GameModeMenu(GameReference* game_window);
    ~GameModeMenu();

    void Show();
    void HandleEvent(const SDL_Event& event, bool& running, bool& menuOpen);
    void Tick();
    void Render();
};