//
// Created by T450s on 24/03/2023.
//

#include "ImageManager.h"

Texture::Texture(SDL_Texture* sdl_texture, std::string filepath) {
    m_SDLTexture = sdl_texture;
    m_Filepath = filepath;
    m_NextTexture = nullptr;
    m_PrevTexture = nullptr;
}

Texture::~Texture() {
    SDL_DestroyTexture(m_SDLTexture);
    std::printf("Deconstructor for '%s'\n", m_Filepath.c_str());
}

ImageManager::ImageManager(SDL_Renderer* renderer) {
    m_Renderer = renderer;
    m_FirstTexture = nullptr;
}

ImageManager::~ImageManager() {
    Texture* Next;
    for (auto Current = m_FirstTexture; Current != nullptr; Current = Next) {
        Next = Current->m_NextTexture;
        delete Current;
    }
}

Texture* ImageManager::LoadTexture(const char *filepath) {
    SDL_Surface* TempSurface = IMG_Load(filepath);
    SDL_Texture* NewSDLTexture = SDL_CreateTextureFromSurface(m_Renderer, TempSurface);
    SDL_FreeSurface(TempSurface);
    auto NewTexture = new Texture(NewSDLTexture, filepath);

    if (m_FirstTexture) {
        m_FirstTexture->m_PrevTexture = NewTexture;
        NewTexture->m_NextTexture = m_FirstTexture;
        m_FirstTexture = NewTexture;
    } else {
        m_FirstTexture = NewTexture;
    }

    return NewTexture;
}

void ImageManager::UnloadTexture(Texture* texture) {
    if (texture == nullptr)
        return;

    if (m_FirstTexture == texture) {
        m_FirstTexture = texture->m_PrevTexture;
        m_FirstTexture->m_NextTexture = nullptr;
    } else {
        auto Next = texture->m_NextTexture;
        auto Prev = texture->m_PrevTexture;

        if (Next) Next->m_PrevTexture = Prev;
        if (Prev) Prev->m_NextTexture = Next;
    }

    delete texture;
}
