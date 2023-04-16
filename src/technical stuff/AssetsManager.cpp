//
// Created by Matq on 14/04/2023.
//

#include "AssetsManager.h"

AssetsManager::AssetsManager(SDL_Renderer* renderer, bool audio_initialized) {
    m_SoundHandler = new SoundManager(audio_initialized);
    m_ImageHandler = new ImageManager(renderer);
    m_TextHandler = new TextManager(m_ImageHandler);

    m_ImagesInitialized = true;
    m_SoundInitialized = audio_initialized;
}

AssetsManager::~AssetsManager() {
    DeinitializeImages();
    DeinitializeSounds();
}

void AssetsManager::DeinitializeImages() {
    if (m_ImagesInitialized) {
        m_ImagesInitialized = false;
        delete m_ImageHandler;
        delete m_TextHandler;
    }
}

void AssetsManager::DeinitializeSounds() {
    if (m_SoundInitialized) {
        m_SoundInitialized = false;
        delete m_SoundHandler;
    }
}
