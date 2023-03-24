//
// Created by T450s on 24/03/2023.
//

#include "ImageManager.h"

ImageManager::ImageManager(SDL_Renderer* renderer) {
    m_Renderer = renderer;
}

ImageManager::~ImageManager() {

}

SDL_Texture* ImageManager::LoadTexture(const char *filepath) {
    SDL_Surface* TempSurface = IMG_Load(filepath);
    SDL_Texture* NewTexture = SDL_CreateTextureFromSurface(m_Renderer, TempSurface);
    SDL_FreeSurface(TempSurface);
    return NewTexture;
}

void ImageManager::UnloadTexture(SDL_Texture *texture) {
    SDL_DestroyTexture(texture);
}
