// src/game/interface/ClassSelectmenu.h

#pragma once

#include "../../GameReference.h"
#include <SDL.h>
#include <chrono>

class GameModeMnu;
class ClassSelectMenu
{
private:
    GameReference *m_GameWindow;
    static LoadedTexture sMenuTexture;
    static LoadedTexture sTextureTitle;
    static LoadedTexture sTextureClassButton1;
    static LoadedTexture sTextureClassButton2;
    static LoadedTexture sTextureClassButton3;
    static LoadedTexture sTextureClassButton4;
    static LoadedTexture sTextureBack;
    SDL_Rect m_TitleRect;
    SDL_Rect m_ClassButtonRect1;
    SDL_Rect m_ClassButtonRect2;
    SDL_Rect m_ClassButtonRect3;
    SDL_Rect m_ClassButtonRect4;
    SDL_Rect m_BackButtonRect;

    std::vector<std::tuple<Vec2d, Vec2d, double>> m_Stars;

public:
    ClassSelectMenu(GameReference *game_window);
    ~ClassSelectMenu();

    void Show(GameMode mode);
    void HandleEvent(const SDL_Event &event, bool &running, bool &menuOpen, GameMode mode);
    void Tick();
    void Render();
};