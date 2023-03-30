//
// Created by Matq on 29/03/2023.
//

#include "SoundManager.h"
#include <iostream>

Sound::Sound(SoundManager* sound_handler, Mix_Chunk* mix_chunk, bool auto_cleanup) {
    m_SoundHandler = sound_handler;
    m_MixChunk = mix_chunk;
    m_NextSound = nullptr;
    m_PrevSound = nullptr;
    m_AutoCleanup = false;

    this->SetAutoCleanup(auto_cleanup);
}

Sound::~Sound() {
    m_SoundHandler->RemoveSoundAutoCleanup(this);
    Mix_FreeChunk(m_MixChunk);
}

void Sound::SetAutoCleanup(bool auto_cleanup) {
    if (auto_cleanup) m_SoundHandler->AddSoundAutoCleanup(this);
    else m_SoundHandler->RemoveSoundAutoCleanup(this);
}

SoundManager::SoundManager(bool enabled) {
    m_FirstSound = nullptr;
    m_Enabled = enabled;
}

SoundManager::~SoundManager() {
    Sound* Next;
    for (auto Current = m_FirstSound; Current != nullptr; Current = Next) {
        Next = Current->m_NextSound;
        delete Current;
    }
}

void SoundManager::AddSoundAutoCleanup(Sound* sound) {
    if (sound->m_AutoCleanup)
        return;

    sound->m_AutoCleanup = true;
    if (m_FirstSound) {
        m_FirstSound->m_PrevSound = sound;
        sound->m_NextSound = m_FirstSound;
        m_FirstSound = sound;
    } else {
        m_FirstSound = sound;
    }
}

void SoundManager::RemoveSoundAutoCleanup(Sound* sound) {
    if (sound == nullptr || !sound->m_AutoCleanup)
        return;

    sound->m_AutoCleanup = false;
    if (m_FirstSound == sound) {
        auto Next = sound->m_NextSound;

        m_FirstSound = Next;
        if (Next) Next->m_PrevSound = nullptr;
    } else {
        auto Next = sound->m_NextSound;
        auto Prev = sound->m_PrevSound;

        if (Next) Next->m_PrevSound = Prev;
        if (Prev) Prev->m_NextSound = Next;
    }
}

Sound* SoundManager::LoadSound(const char *filepath, bool auto_cleanup) {
    Mix_Chunk* NewMixChunk = Mix_LoadWAV(filepath);
    if (!NewMixChunk) {
        std::printf("Failed to load sound '%s'\n", filepath);
        return new Sound(this, nullptr, true);
    }

    return new Sound(this, NewMixChunk, auto_cleanup);
}

void SoundManager::PlaySound(Sound* sound) {
    if (!m_Enabled)
        return;

    Mix_PlayChannel(-1, sound->MixChunk(), 0);
}
