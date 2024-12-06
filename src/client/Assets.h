//
// Created by Mat on 12/3/2024.
//

#pragma once

#include <iostream>
#include <memory> // For std::unique_ptr
#include <mutex>  // For std::call_once and std::once_flag
#include <vector>
#include <unordered_map>
#include "SDL_image.h"
#include "SDL_mixer.h"


struct TextureInfo {
    Uint32 format;
    int access, w, h;
};

class Assets;
class Texture {
private:
    friend class Assets;
    const std::string m_Key;
    SDL_Texture * m_SDLTexture;
    TextureInfo m_Information;
    std::string m_LoadExtension;

public:
    explicit Texture(std::string key = "NaN", SDL_Texture* sdl_texture = nullptr, std::string load_extension = "NaN");
    ~Texture();

    // Getting
    [[nodiscard]] SDL_Texture* SDLTexture() const { return m_SDLTexture; }
    [[nodiscard]] Uint32 GetFormat() const { return m_Information.format; }
    [[nodiscard]] int GetAccess() const { return m_Information.access; }
    [[nodiscard]] int GetWidth() const { return m_Information.w; }
    [[nodiscard]] int GetHeight() const { return m_Information.h; }
    [[nodiscard]] const std::string& Key() const { return m_Key; }

    // Manipulating
    void SetBlendMode(SDL_BlendMode blend_mode);
    void SetColorMod(Uint8 r, Uint8 g, Uint8 b);
    void SetAlphaMod(int alpha);
};

class Sound2 {
private:
    friend class Assets;
    const std::string m_Key;
    Mix_Chunk* m_MixChunk;
    std::string m_LoadExtension;

public:
    explicit Sound2(std::string key = "NaN", Mix_Chunk* mix_chunk = nullptr, std::string load_extension = "NaN");
    ~Sound2();

    // Getting
    [[nodiscard]] Mix_Chunk* MixChunk() const { return m_MixChunk; }

    // Manipulating
    void SetVolume(int volume);
    void PlaySound();
};

class Assets {
    static Assets* Instance;
    SDL_Renderer* m_Renderer;
    bool m_SoundsEnabled;
    std::unordered_map<std::string, Texture*> m_Textures;
    std::unordered_map<std::string, Sound2*> m_Sounds;
    // std::vector<Texture2> m_UsedTextures;
    Texture* m_InvalidTexture;

public:
    static void initialize(SDL_Renderer* renderer, bool sounds_enabled);
    static void deinitialize();
    static Assets* Get();

    Assets(const Assets&) = delete;
    Assets& operator=(const Assets&) = delete;

    // Getting
    Texture* GetTexture(const std::string& texture_key);
    Sound2* GetSound(const std::string& sound_key);
    bool SoundsEnabled() const { return m_SoundsEnabled; }

    // Generating
    Texture* TextureFromSurface(SDL_Surface* sdl_surface);
    Texture* CreateTexture(Uint32 format, int access, int w, int h);

private:
    Assets(SDL_Renderer* renderer, bool sounds_enabled);
    ~Assets();

};