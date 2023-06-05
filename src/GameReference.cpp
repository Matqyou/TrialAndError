//
// Created by Matq on 22/03/2023.
//

#include "GameReference.h"

GameReference::GameReference() {
    m_Window = nullptr;
    m_Renderer = nullptr;
    m_Timer = nullptr;
    m_Random = nullptr;
    m_Draw = nullptr;
    m_AssetsHandler = nullptr;
    m_Width = 0;
    m_Height = 0;
    m_Width2 = 0.0;
    m_Height2 = 0.0;
    m_InitializedSDL = false;
    m_InitializedMix = false;
    m_InitializedAudio = false;
    m_InitializedImages = false;
    m_InitializedTTF = false;
}

GameReference::~GameReference() {
    Deinitialize(false);
    delete m_AssetsHandler;
}

void GameReference::UpdateDimensions(int width, int height) {
    m_Width = width;
    m_Height = height;
    m_Width2 = width / 2.0;
    m_Height2 = height / 2.0;
}

bool GameReference::InitializeSDL() {
    if (!m_InitializedSDL) {
        m_InitializedSDL = !SDL_Init(SDL_INIT_EVERYTHING);
        if (!m_InitializedSDL) {
            std::printf("Error while initializing main %s\n", SDL_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeMix() {
    if (!m_InitializedMix) {
        int MixFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
        m_InitializedMix = Mix_Init(MixFlags) == MixFlags;
        if (!m_InitializedMix) {
            std::printf("Error while initializing Mix %s\n", Mix_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeAudio() {
    if (!m_InitializedAudio) {
        m_InitializedAudio = !Mix_OpenAudio(44100 * 2, MIX_DEFAULT_FORMAT, 2, 1024 * 4);
        Mix_AllocateChannels(16);
        if (!m_InitializedAudio) std::printf("Warning while opening audio %s\n", Mix_GetError());
    }

    return true;
}

bool GameReference::InitializeImages() {
    if (!m_InitializedImages) {
        int ImageFlags = IMG_INIT_PNG | IMG_INIT_JPG;
        m_InitializedImages = IMG_Init(ImageFlags) == ImageFlags;
        if (!m_InitializedImages) {
            std::printf("Error while initializing Image %s\n", IMG_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeTTF() {
    if (!m_InitializedTTF) {
        m_InitializedTTF = !TTF_Init();
        if (!m_InitializedTTF) {
            std::printf("Error while initializing TTF %s\n", TTF_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::Initialize() {
    SDL_version Version;
    SDL_GetVersion(&Version);
    std::cout << "Using SDL " << (int)Version.major << "." << (int)Version.minor << "." << (int)Version.patch
              << std::endl;

    if (!InitializeSDL() ||
        !InitializeMix() ||
        !InitializeAudio() ||
        !InitializeImages() ||
        !InitializeTTF())
        return false;

    if (!m_Window) {
        UpdateDimensions(1280, 720);
        m_Window = SDL_CreateWindow("TrialAndError",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    m_Width,
                                    m_Height,
                                    SDL_WINDOW_RESIZABLE);
        if (!m_Window) {
            std::printf("Error while creating the window %s\n", SDL_GetError());
            return false;
        }
    }

    if (!m_Renderer) {
        m_Renderer = SDL_CreateRenderer(m_Window, -1, 0);
        if (!m_Renderer) {
            std::printf("Error while creating the renderer %s\n", SDL_GetError());
            return false;
        }
    }

    if (!m_Timer) m_Timer = new Clock(60);
    if (!m_Random) m_Random = new Randomizer();
    if (!m_Draw) m_Draw = new Drawing(this);
    if (!m_AssetsHandler) m_AssetsHandler = new AssetsManager(m_Renderer, m_InitializedAudio);
    return true;
}

void GameReference::Deinitialize(bool keep_sound) {
    m_AssetsHandler->DeinitializeImages();
    delete m_Draw;
    m_Draw = nullptr;
    delete m_Random;
    m_Random = nullptr;
    delete m_Timer;
    m_Timer = nullptr;
    if (m_Renderer) SDL_DestroyRenderer(m_Renderer);
    if (m_Window) SDL_DestroyWindow(m_Window);

    m_Draw = nullptr;
    m_Timer = nullptr;
    m_Renderer = nullptr;
    m_Window = nullptr;

    if (m_InitializedTTF) {
        m_InitializedTTF = false;
        TTF_Quit();
    }

    if (m_InitializedImages) {
        m_InitializedImages = false;
        IMG_Quit();
    }

    if (!keep_sound) { // TODO: Check this out -_- looks very sus
        m_AssetsHandler->DeinitializeSounds();

        if (m_InitializedAudio) {
            m_InitializedAudio = false;
            Mix_CloseAudio();
        }
        if (m_InitializedMix) {
            m_InitializedMix = false;
            Mix_Quit();
        }
        if (m_InitializedSDL) {
            m_InitializedSDL = false;
            SDL_Quit();
        }
    }
}

void GameReference::Event(const SDL_Event& event) {
    if (event.type != SDL_WINDOWEVENT ||
        event.window.event != SDL_WINDOWEVENT_SIZE_CHANGED)
        return;

    UpdateDimensions(event.window.data1, event.window.data2);
}