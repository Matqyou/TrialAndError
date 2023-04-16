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
#include "technical stuff/AssetsManager.h"

class GameReference {
private:
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    Clock* m_Timer;
    Drawing* m_Draw;
    AssetsManager* m_AssetsHandler;

    int m_Width, m_Height;
    bool m_InitializedSDL;
    bool m_InitializedMix;
    bool m_InitializedAudio;
    bool m_InitializedImages;
    bool m_InitializedTTF;

    bool InitializeSDL();
    bool InitializeMix();
    bool InitializeAudio();
    bool InitializeImages();
    bool InitializeTTF();
public:
    GameReference();
    ~GameReference();

    SDL_Window* Window() const { return m_Window; }
    SDL_Renderer* Renderer() const { return m_Renderer; }
    Clock* Timer() const { return m_Timer; }
    Drawing* RenderClass() const { return m_Draw; }
    AssetsManager* Assets() const { return m_AssetsHandler; }
    int Width() const { return m_Width; }
    int Height() const { return m_Height; }

    bool Initialize();
    void Deinitialize(bool keep_sound);

    void Event(const SDL_Event& currentEvent);
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMEREFERENCE_H_
