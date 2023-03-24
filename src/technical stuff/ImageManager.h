//
// Created by T450s on 24/03/2023.
//

#ifndef TRIALANDERROR_IMAGEMANAGER_H
#define TRIALANDERROR_IMAGEMANAGER_H

#include <SDL_image.h>
#include <vector>
#include <string>
#include <iostream>

class Texture {
private:
    friend class ImageManager;
    SDL_Texture* m_SDLTexture;
    Texture* m_NextTexture, *m_PrevTexture;

public:
    explicit Texture(SDL_Texture* sdl_texture);
    ~Texture();

    SDL_Texture* SDLTexture() const { return m_SDLTexture; }
    void Query(Uint32* format, int* access, int* w, int* h);
};

class ImageManager {
private:
    SDL_Renderer* m_Renderer;
    // std::vector<SDL_Surface*> m_Surfaces; don't see a use case for this yet

    Texture* m_FirstTexture;

public:
    explicit ImageManager(SDL_Renderer* renderer);
    ~ImageManager();

    Texture* LoadTexture(const char* filepath);

    void UnloadTexture(Texture* texture);
};

#endif //TRIALANDERROR_IMAGEMANAGER_H
