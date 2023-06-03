//
// Created by T450s on 24/03/2023.
//

#include "ImageManager.h"

Texture::Texture(ImageManager* image_handler, SDL_Texture* sdl_texture, bool auto_cleanup) {
    m_ImageHandler = image_handler;
    m_SDLTexture = sdl_texture;
    SDL_QueryTexture(m_SDLTexture,
                     &m_Information.format,
                     &m_Information.access,
                     &m_Information.w,
                     &m_Information.h);
    m_NextTexture = nullptr;
    m_PrevTexture = nullptr;
    m_AutoCleanup = false;

    SetAutoCleanup(auto_cleanup);
}

Texture::~Texture() {
    m_ImageHandler->RemoveTextureAutoCleanup(this);
    SDL_DestroyTexture(const_cast<SDL_Texture*>(m_SDLTexture));
}

Texture* Texture::CopyTexture(bool auto_cleanup) {
    return m_ImageHandler->CopyTexture(this, auto_cleanup);
}

void Texture::SetBlendMode(SDL_BlendMode blend_mode) {
    SDL_SetTextureBlendMode(m_SDLTexture, blend_mode);
}

void Texture::SetColorMod(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(m_SDLTexture, r, g, b);
}

void Texture::SetAlphaMod(int alpha) {
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

Texture* ImageManager::LoadTexture(const char* filepath, bool auto_cleanup) {
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

Texture* ImageManager::CreateTexture(Uint32 format, int access, int w, int h, bool auto_cleanup) {
    SDL_Texture* NewSDLTexture = SDL_CreateTexture(m_Renderer, format, access, w, h);

    return new Texture(this, NewSDLTexture, auto_cleanup);
}

Texture* ImageManager::CopyTexture(Texture* original, bool auto_cleanup) {
    Texture* NewTexture = CreateTexture(original->GetFormat(),
                                        SDL_TEXTUREACCESS_TARGET,
                                        original->GetWidth(),
                                        original->GetHeight(),
                                        auto_cleanup);
    SDL_Texture* TempTarget = SDL_GetRenderTarget(m_Renderer);
    SDL_SetRenderTarget(m_Renderer, NewTexture->SDLTexture());
    SDL_RenderCopy(m_Renderer, original->SDLTexture(), nullptr, nullptr);
    SDL_SetRenderTarget(m_Renderer, TempTarget);
    return NewTexture;
}