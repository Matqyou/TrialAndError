//
// Created by T450s on 24/03/2023.
//

#ifndef TRIALANDERROR_IMAGEMANAGER_H
#define TRIALANDERROR_IMAGEMANAGER_H

#include <SDL_image.h>
#include <vector>
#include <string>
#include <iostream>

class ImageManager;
class Texture {
private:
    friend class ImageManager;
    ImageManager* m_ImageHandler;
    SDL_Texture* m_SDLTexture;
    Texture* m_NextTexture, *m_PrevTexture;
    bool m_AutoCleanup;

public:
    explicit Texture(ImageManager* image_handler, SDL_Texture* sdl_texture, bool auto_cleanup);
    ~Texture();

    SDL_Texture* SDLTexture() const { return m_SDLTexture; }
    bool GetAutoCleanup() const { return m_AutoCleanup; }
    void Query(Uint32* format, int* access, int* w, int* h);

    void SetAutoCleanup(bool auto_cleanup);
};

class ImageManager {
private:
    SDL_Renderer* m_Renderer;
    // std::vector<SDL_Surface*> m_Surfaces; don't see a use case for this yet

    Texture* m_FirstTexture;

public:
    explicit ImageManager(SDL_Renderer* renderer);
    ~ImageManager();

    SDL_Renderer* Renderer() const { return m_Renderer; }

    void AddTextureAutoCleanup(Texture* texture);
    void RemoveTextureAutoCleanup(Texture* texture);
    Texture* LoadTexture(const char* filepath, bool auto_cleanup = true);
    Texture* TextureFromSurface(SDL_Surface* sdl_surface, bool auto_cleanup = true);
};

#endif //TRIALANDERROR_IMAGEMANAGER_H
