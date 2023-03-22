//
// Created by Matq on 22/03/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMEREFERENCE_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMEREFERENCE_H_

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "technical stuff/Clock.h"
#include "technical stuff/TextManager.h"

class GameReference {
private:
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    Clock* m_Timer;
    TextManager* m_TextHandler;

    const int m_Width = 900;
    const int m_Height = 700;

public:
    GameReference();
    ~GameReference();

    SDL_Window* Window() const { return m_Window; }
    SDL_Renderer* Renderer() const { return m_Renderer; }
    Clock* Timer() const { return m_Timer; }
    TextManager* TextHandler() const { return m_TextHandler; }
    int Width() const { return m_Width; }
    int Height() const { return m_Height; }

    bool Initialize();
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMEREFERENCE_H_
