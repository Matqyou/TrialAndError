//
// Created by Matq on 14/04/2023.
//

#pragma once

#include "SoundManager.h"
#include "TextManager.h"

class AssetsManager {
private:
    SoundManager* m_SoundHandler;
    TextManager* m_TextHandler;

    bool m_ImagesInitialized;
    bool m_SoundInitialized;

public:
    AssetsManager(SDL_Renderer* renderer, bool audio_initialized);
    ~AssetsManager();

    // Getting
    [[nodiscard]] SoundManager* SoundHandler() const { return m_SoundHandler; }
    [[nodiscard]] TextManager* TextHandler() const { return m_TextHandler; }

    // Manipulating
    void DeinitializeImages();
    void DeinitializeSounds();

};
