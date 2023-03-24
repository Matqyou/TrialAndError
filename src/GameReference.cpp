//
// Created by Matq on 22/03/2023.
//

#include "GameReference.h"

GameReference::GameReference() {
    m_Window = nullptr;
    m_Renderer = nullptr;
    m_Timer = nullptr;
    m_TextHandler = nullptr;
    m_ImageHandler = nullptr;
    m_Width = 0;
    m_Height = 0;
    m_Initialized = false;
}

GameReference::~GameReference() {
    if (m_Initialized) {
        delete m_ImageHandler;
        delete m_TextHandler;
        delete m_Timer;
        SDL_DestroyRenderer(m_Renderer);
        SDL_DestroyWindow(m_Window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }
}

bool GameReference::Initialize() {
    m_Initialized = true;
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if (Result) {
        std::printf("Error while initializing main %s\n", SDL_GetError());
        return false;
    }
    int ImageFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    int ResultImage = IMG_Init(ImageFlags);
    if (ResultImage != ImageFlags) {
        std::printf("Error while initializing Image %s\n", SDL_GetError());
        return false;
    }
    int ResultTTF = TTF_Init();
    if (ResultTTF) {
        std::printf("Error while initializing TTF %s\n", SDL_GetError());
        return false;
    }

    m_Width = 960;
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
    m_TextHandler = new TextManager(m_Renderer);
    m_ImageHandler = new ImageManager(m_Renderer);
    return true;
}

void GameReference::Event(const SDL_Event& currentEvent) {
    if (currentEvent.type != SDL_WINDOWEVENT ||
        currentEvent.window.event != SDL_WINDOWEVENT_SIZE_CHANGED)
        return;

    m_Width = currentEvent.window.data1;
    m_Height = currentEvent.window.data2;
}