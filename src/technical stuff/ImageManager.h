//
// Created by T450s on 24/03/2023.
//

#pragma once

#include <SDL_image.h>
#include <vector>
#include <string>
#include <iostream>

struct TextureInfo {
    Uint32 format;
    int access, w, h;
};

class ImageManager;
class Texture {
private:
    friend class ImageManager;
    ImageManager* m_ImageHandler;
    SDL_Texture* m_SDLTexture;
    TextureInfo m_Information;
    Texture* m_NextTexture, * m_PrevTexture;
    bool m_AutoCleanup;

public:
    explicit Texture(ImageManager* image_handler, SDL_Texture* sdl_texture, bool auto_cleanup);
    ~Texture();

    // Getting
    [[nodiscard]] SDL_Texture* SDLTexture() const { return m_SDLTexture; }
    [[nodiscard]] Uint32 GetFormat() const { return m_Information.format; }
    [[nodiscard]] int GetAccess() const { return m_Information.access; }
    [[nodiscard]] int GetWidth() const { return m_Information.w; }
    [[nodiscard]] int GetHeight() const { return m_Information.h; }

    // Generating
    Texture* CopyTexture(bool auto_cleanup);

    // Manipulating
    void SetBlendMode(SDL_BlendMode blend_mode);
    void SetColorMod(Uint8 r, Uint8 g, Uint8 b);
    void SetAlphaMod(int alpha);
    void SetAutoCleanup(bool auto_cleanup);
};

class ImageManager {
private:
    SDL_Renderer* m_Renderer;
    Texture* m_FirstTexture;

public:
    explicit ImageManager(SDL_Renderer* renderer);
    ~ImageManager();

    // Getting
    [[nodiscard]] SDL_Renderer* Renderer() const { return m_Renderer; }

    // Generating
    [[nodiscard]] Texture* LoadTexture(const char* filepath, bool auto_cleanup);
    [[nodiscard]] Texture* TextureFromSurface(SDL_Surface* sdl_surface, bool auto_cleanup);
    [[nodiscard]] Texture* CreateTexture(Uint32 format, int access, int w, int h, bool auto_cleanup);
    [[nodiscard]] Texture* CopyTexture(Texture* original, bool auto_cleanup);

    // Managing
    void AddTextureAutoCleanup(Texture* texture);
    void RemoveTextureAutoCleanup(Texture* texture);
};
