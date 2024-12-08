//
// Created by Matq on 22/03/2023.
//

#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "technical stuff/Vec2.h"
#include "technical stuff/Clock.h"
#include "technical stuff/Drawing.h"
#include "technical stuff/Randomizer.h"
#include "technical stuff/AssetsManager.h"
#include "game/GameWorld.h"
#include "technical stuff/GameControllers.h"

class GameReference {
private:
    SDL_Window* m_Window;
    SDL_Renderer* m_Renderer;
    SDL_GLContext m_GLContext;
    Clock* m_Timer;
    Drawing* m_Draw;
    Randomizer* m_Random;
    AssetsManager* m_AssetsHandler;

    GameWorld* m_GameWorld;
    GameControllers* m_Controllers;

    int m_Width, m_Height;
    double m_Width2, m_Height2;
    bool m_InitializedSDL;
    bool m_InitializedMix;
    bool m_InitializedAudio;
    bool m_InitializedImages;
    bool m_InitializedTTF;

    static LoadedSound sQuitSound;

    // Manipulating
    void UpdateDimensions(int width, int height);
    bool InitializeSDL();
    bool InitializeMix();
    bool InitializeAudio();
    bool InitializeImages();
    bool InitializeTTF();

    // Waiting
    void WaitForSoundToFinish();

public:
    GameReference();
    ~GameReference();

    // Getting
    [[nodiscard]] SDL_Window* Window() const { return m_Window; }
    [[nodiscard]] SDL_Renderer* Renderer() const { return m_Renderer; }
    [[nodiscard]] SDL_GLContext& GlContext() { return m_GLContext; }
    [[nodiscard]] Clock* Timer() const { return m_Timer; }
    [[nodiscard]] Randomizer* Random() const { return m_Random; }
    [[nodiscard]] Drawing* Render() const { return m_Draw; }
    [[nodiscard]] AssetsManager* Assetz() const { return m_AssetsHandler; }
    [[nodiscard]] GameWorld* World() const { return m_GameWorld; }
    [[nodiscard]] GameControllers* Controllers() const { return m_Controllers; }
    [[nodiscard]] int GetWidth() const { return m_Width; }
    [[nodiscard]] int GetHeight() const { return m_Height; }
    [[nodiscard]] double GetWidth2() const { return m_Width2; }
    [[nodiscard]] double GetHeight2() const { return m_Height2; }

    // Manipulating
    bool Initialize();
    void Deinitialize(bool play_quit_sound);
    void TestEnvironment();

    // Listening
    void Event(const SDL_Event& event);
};
