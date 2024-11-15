//
// Created by Mim on 6/5/2023.
//

#pragma once

#include "game/indicators/TextSurface.h"
#include "GameReference.h"

class Menu {
private:
    double m_Width, m_Height;
    Drawing* Renderer;
    Texture* TextureButtonStart;
public:
    SDL_Rect StartButtonRect;
    Menu(GameReference* GameWindow,Drawing* Render,Texture* TextureStart, int width, int height);
    void Event(const SDL_Event& currentEvent);
    void Tick();
    void Draw();
};
