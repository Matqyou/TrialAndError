//
// Created by T450s on 24/03/2023.
//

#include "ImageManager.h"

Texture::Texture(ImageManager* image_handler, SDL_Texture* sdl_texture, bool auto_cleanup) {
    m_ImageHandler = image_handler;
    m_SDLTexture = sdl_texture;
    m_NextTexture = nullptr;
    m_PrevTexture = nullptr;
    m_AutoCleanup = false;

    this->SetAutoCleanup(auto_cleanup);
}

Texture::~Texture() {
    m_ImageHandler->RemoveTextureAutoCleanup(this);
    SDL_DestroyTexture(m_SDLTexture);
}

void Texture::Query(Uint32* format, int* access, int* w, int* h) {
    SDL_QueryTexture(m_SDLTexture, format, access, w, h);
}

void Texture::SetAlpha(int alpha) {
    SDL_SetTextureAlphaMod(m_SDLTexture, alpha);
}

void Texture::SetAutoCleanup(bool auto_cleanup) {
    if (auto_cleanup) m_ImageHandler->AddTextureAutoCleanup(this);
    else m_ImageHandler->RemoveTextureAutoCleanup(this);
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

void ImageManager::AddTextureAutoCleanup(Texture* texture) {
    if (texture->m_AutoCleanup)
        return;

    texture->m_AutoCleanup = true;
    if (m_FirstTexture) {
        m_FirstTexture->m_PrevTexture = texture;
        texture->m_NextTexture = m_FirstTexture;
        m_FirstTexture = texture;
    } else {
        m_FirstTexture = texture;
    }
}

void ImageManager::RemoveTextureAutoCleanup(Texture* texture) {
    if (texture == nullptr || !texture->m_AutoCleanup)
        return;

    texture->m_AutoCleanup = false;
    if (m_FirstTexture == texture) {
        auto Next = texture->m_NextTexture;

        m_FirstTexture = Next;
        if (Next) Next->m_PrevTexture = nullptr;
    } else {
        auto Next = texture->m_NextTexture;
        auto Prev = texture->m_PrevTexture;

        if (Next) Next->m_PrevTexture = Prev;
        if (Prev) Prev->m_NextTexture = Next;
    }
}

Texture* ImageManager::LoadTexture(const char *filepath, bool auto_cleanup) {
    SDL_Surface* TempSurface = IMG_Load(filepath);
    if (!TempSurface) {
        std::printf("Failed to load texture '%s'\n", filepath);
        return nullptr;
    }

    SDL_Texture* NewSDLTexture = SDL_CreateTextureFromSurface(m_Renderer, TempSurface);
    SDL_FreeSurface(TempSurface);

    return new Texture(this, NewSDLTexture, auto_cleanup);
}

Texture* ImageManager::TextureFromSurface(SDL_Surface* sdl_surface, bool auto_cleanup) {
    SDL_Texture* NewSDLTexture = SDL_CreateTextureFromSurface(m_Renderer, sdl_surface);

    return new Texture(this, NewSDLTexture, auto_cleanup);
}