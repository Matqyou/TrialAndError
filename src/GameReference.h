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
    double m_Width2, m_Height2;
    bool m_InitializedSDL;
    bool m_InitializedMix;
    bool m_InitializedAudio;
    bool m_InitializedImages;
    bool m_InitializedTTF;

    // Manipulating
    void UpdateDimensions(int width, int height);
    bool InitializeSDL();
    bool InitializeMix();
    bool InitializeAudio();
    bool InitializeImages();
    bool InitializeTTF();

public:
    GameReference();
    ~GameReference();

    // Getting
    [[nodiscard]] SDL_Window* Window() const { return m_Window; }
    [[nodiscard]] SDL_Renderer* Renderer() const { return m_Renderer; }
    [[nodiscard]] Clock* Timer() const { return m_Timer; }
    [[nodiscard]] Drawing* Render() const { return m_Draw; }
    [[nodiscard]] AssetsManager* Assets() const { return m_AssetsHandler; }
    [[nodiscard]] int GetWidth() const { return m_Width; }
    [[nodiscard]] int GetHeight() const { return m_Height; }
    [[nodiscard]] double GetWidth2() const { return m_Width2; }
    [[nodiscard]] double GetHeight2() const { return m_Height2; }

    // Manipulating
    bool Initialize();
    void Deinitialize(bool keep_sound);

    // Listening
    void Event(const SDL_Event& event);
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_GAMEREFERENCE_H_
