//
// Created by Matq on 22/03/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMEREFERENCE_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMEREFERENCE_H_

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "technical stuff/Clock.h"
#include "technical stuff/Drawing.h"
#include "technical stuff/SoundManager.h"
#include "technical stuff/ImageManager.h"
#include "technical stuff/TextManager.h"

class GameReference {
public:
    enum GameState {
        STATE_NONE,
        STATE_SOUND_ONLY,
        STATE_ALL
    };

private:
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    Clock* m_Timer;
    Drawing* m_Draw;
    SoundManager* m_SoundHandler;
    ImageManager* m_ImageHandler;
    TextManager* m_TextHandler;

    int m_Width, m_Height;
    GameState m_State;

public:
    GameReference();
    ~GameReference();

    SDL_Window* Window() const { return m_Window; }
    SDL_Renderer* Renderer() const { return m_Renderer; }
    Clock* Timer() const { return m_Timer; }
    Drawing* RenderClass() const { return m_Draw; }
    SoundManager* SoundHandler() const { return m_SoundHandler; }
    ImageManager* ImageHandler() const { return m_ImageHandler; }
    TextManager* TextHandler() const { return m_TextHandler; }
    int Width() const { return m_Width; }
    int Height() const { return m_Height; }

    bool Initialize();
    void Deinitialize(bool keep_sound);

    void Event(const SDL_Event& currentEvent);
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMEREFERENCE_H_
