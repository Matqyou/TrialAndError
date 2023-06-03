//
// Created by Matq on 29/03/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_SOUNDMANAGER_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_SOUNDMANAGER_H_

#include <SDL_mixer.h>

class SoundManager;
class Sound {
private:
    friend class SoundManager;
    SoundManager* m_SoundHandler;
    Mix_Chunk* m_MixChunk;
    Sound* m_NextSound, *m_PrevSound;
    bool m_AutoCleanup;

public:
    Sound(SoundManager* sound_handler, Mix_Chunk* mix_chunk, bool auto_cleanup);
    ~Sound();

    // Getting
    [[nodiscard]] Mix_Chunk* MixChunk() const { return m_MixChunk; }

    // Setting
    void SetVolume(int volume);
    void SetAutoCleanup(bool auto_cleanup);
};

class SoundManager {
private:
    Sound* m_FirstSound;
    bool m_Enabled;

public:
    explicit SoundManager(bool enabled);
    ~SoundManager();

    // Generating
    Sound* LoadSound(const char* filepath, bool auto_cleanup);

    // Managing
    void AddSoundAutoCleanup(Sound* sound);
    void RemoveSoundAutoCleanup(Sound* sound);

    // Manipulating
    void PlaySound(Sound* sound) const;
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_SOUNDMANAGER_H_
