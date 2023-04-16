//
// Created by Matq on 14/04/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_ASSETSMANAGER_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_ASSETSMANAGER_H_

#include "ImageManager.h"
#include "SoundManager.h"
#include "TextManager.h"

class AssetsManager {
private:
    ImageManager* m_ImageHandler;
    SoundManager* m_SoundHandler;
    TextManager* m_TextHandler;

    bool m_ImagesInitialized;
    bool m_SoundInitialized;

public:
    AssetsManager(SDL_Renderer* renderer, bool audio_initialized);
    ~AssetsManager();

    ImageManager* ImageHandler() const { return m_ImageHandler; }
    SoundManager* SoundHandler() const { return m_SoundHandler; }
    TextManager* TextHandler() const { return m_TextHandler; }

    void DeinitializeImages();
    void DeinitializeSounds();
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_ASSETSMANAGER_H_
