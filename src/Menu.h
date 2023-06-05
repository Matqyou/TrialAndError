//
// Created by Mim on 6/5/2023.
//

#ifndef TRIALANDERROR_MENU_H
#define TRIALANDERROR_MENU_H
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


#endif //TRIALANDERROR_MENU_H
