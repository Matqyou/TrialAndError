//
// Created by Matq on 22/03/2023.
//

#include "GameReference.h"

GameReference::GameReference() {
    m_Window = nullptr;
    m_Renderer = nullptr;
    m_Timer = nullptr;
    m_Draw = nullptr;
    m_SoundHandler = nullptr;
    m_ImageHandler = nullptr;
    m_TextHandler = nullptr;
    m_Width = 0;
    m_Height = 0;
    m_InitializedBase = false;
    m_InitializedSound = false;
}

GameReference::~GameReference() {
    Deinitialize(false);
}

bool GameReference::Initialize() {
    if (m_InitializedBase || m_InitializedSound)
        return false;

    m_InitializedBase = true;
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if (Result) {
        std::printf("Error while initializing main %s\n", SDL_GetError());
        return false;
    }
    int MixFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
    int ResultMixFlags = Mix_Init(MixFlags);
    if (ResultMixFlags != MixFlags) {
        std::printf("Error while initializing Mix %s\n", Mix_GetError());
        return false;
    }
    int ResultMix = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    m_InitializedSound = !ResultMix;
    if (ResultMix) {
        std::printf("Warning while opening audio %s\n", Mix_GetError());
    }
    int ImageFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    int ResultImageFlags = IMG_Init(ImageFlags);
    if (ResultImageFlags != ImageFlags) {
        std::printf("Error while initializing Image %s\n", IMG_GetError());
        return false;
    }
    int ResultTTF = TTF_Init();
    if (ResultTTF) {
        std::printf("Error while initializing TTF %s\n", TTF_GetError());
        return false;
    }

    m_Width = 1280;
    m_Height = 720;
    m_Window = SDL_CreateWindow("TrialAndError", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                m_Width, m_Height, SDL_WINDOW_RESIZABLE);
    if (!m_Window) {
        std::printf("Error while creating the window %s\n", SDL_GetError());
        return false;
    }

    m_Renderer = SDL_CreateRenderer(m_Window, -1, 0);
    if (!m_Renderer) {
        std::printf("Error while creating the renderer %s\n", SDL_GetError());
        return false;
    }

    m_Timer = new Clock(60);
    m_Draw = new Drawing(this);
    m_SoundHandler = new SoundManager(m_InitializedSound);
    m_ImageHandler = new ImageManager(m_Renderer);
    m_TextHandler = new TextManager(m_ImageHandler);
    return true;
}

void GameReference::Deinitialize(bool keep_sound) {
    if (m_InitializedBase) {
        m_InitializedBase = false;
        delete m_TextHandler;
        delete m_ImageHandler;
        delete m_Draw;
        delete m_Timer;
        SDL_DestroyRenderer(m_Renderer);
        SDL_DestroyWindow(m_Window);
        TTF_Quit();
        IMG_Quit();
    }
    if (!keep_sound && m_InitializedSound) {
        m_InitializedSound = false;
        delete m_SoundHandler;
        Mix_CloseAudio();
        Mix_Quit();
        SDL_Quit();
    }
}

void GameReference::Event(const SDL_Event& currentEvent) {
    if (currentEvent.type != SDL_WINDOWEVENT ||
        currentEvent.window.event != SDL_WINDOWEVENT_SIZE_CHANGED)
        return;

    m_Width = currentEvent.window.data1;
    m_Height = currentEvent.window.data2;
}