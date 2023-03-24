//
// Created by T450s on 24/03/2023.
//

#ifndef TRIALANDERROR_IMAGEMANAGER_H
#define TRIALANDERROR_IMAGEMANAGER_H

#include <SDL_image.h>
#include <vector>

class ImageManager {
private:
    SDL_Renderer* m_Renderer;
    // std::vector<SDL_Surface*> m_Surfaces; don't see a use case for this yet
    std::vector<SDL_Texture*> m_Textures;

public:
    ImageManager(SDL_Renderer* renderer);
    ~ImageManager();

    SDL_Texture* LoadTexture(const char* filepath);

    void UnloadTexture(SDL_Texture* texture);
};

#endif //TRIALANDERROR_IMAGEMANAGER_H
