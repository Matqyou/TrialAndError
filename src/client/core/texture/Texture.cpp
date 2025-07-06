//
// Created by Matq on 19/06/2025.
//

#include "client/core/Assets.h"
#include "client/core/texture/visual/VisualTexture.h"
#include "Texture.h"

#define CHECK_SDL_TEXTURE(sdl_texture, message) \
    if ((sdl_texture) == nullptr) { \
        std::wcout << Strings::FStringColorsW(L"[Texture] &c" message L"\n"); \
        return this; \
    }

Texture::Texture(SDL_Texture *sdl_texture)
{
	this->sdl_texture = sdl_texture;
	SDL_GetTextureSize(sdl_texture, &sdl_texture_size.x, &sdl_texture_size.y); // might error on nullptr
	this->flagged_for_automatic_deletion = false;
	this->uses_hitbox = false;
}

Texture::~Texture()
{
	if (sdl_texture != nullptr)
	{
		SDL_DestroyTexture(sdl_texture);
		sdl_texture = nullptr;
	}
}

Texture *Texture::CopyTexture(SDL_TextureAccess texture_access) const
{
	Texture* new_texture;
	if (uses_hitbox)
		new_texture = new VisualTexture(*(VisualTexture*)(this));
	else
		new_texture = new Texture(*this);

	new_texture->sdl_texture = Assets.CopySDLTexture(sdl_texture, texture_access);
	return new_texture;
}

Texture *Texture::FlagForAutomaticDeletion()
{
	if (!flagged_for_automatic_deletion)
	{
		flagged_for_automatic_deletion = true;
		AssetsClass::AutomaticallyDeleteTexture(this);
	}
	return this;
}

Texture *Texture::SetSDLTexture(SDL_Texture *sdl_texture)
{
	this->sdl_texture = sdl_texture;
	SDL_GetTextureSize(sdl_texture, &sdl_texture_size.x, &sdl_texture_size.y);
	return this;
}

Texture *Texture::SetBlendMode(SDL_BlendMode blend_mode)
{
#ifndef NDEBUG
	CHECK_SDL_TEXTURE(sdl_texture, "&cSetBlendMode(), sdl_texture was nullptr!")
#endif

	SDL_SetTextureBlendMode(sdl_texture, blend_mode);
	return this;
}

Texture *Texture::SetColorMod(Uint8 r, Uint8 g, Uint8 b)
{
#ifndef NDEBUG
	CHECK_SDL_TEXTURE(sdl_texture, "&cSetColorMod(), sdl_texture '%s' was nullptr!")
#endif

	SDL_SetTextureColorMod(sdl_texture, r, g, b);
	return this;
}

Texture *Texture::SetColorMod(SDL_Color color)
{
#ifndef NDEBUG
	CHECK_SDL_TEXTURE(sdl_texture, "&cSetColorMod(), sdl_texture '%s' was nullptr!")
#endif

	SDL_SetTextureColorMod(sdl_texture, color.r, color.g, color.b);
	return this;
}

Texture *Texture::SetColorModFloat(float r, float g, float b)
{
#ifndef NDEBUG
	CHECK_SDL_TEXTURE(sdl_texture, "&cSetColorModFloat(), sdl_texture '%s' was nullptr!")
#endif

	SDL_SetTextureColorModFloat(sdl_texture, r, g, b);
	return this;
}

Texture *Texture::SetColorModFloat(SDL_FColor color)
{
#ifndef NDEBUG
	CHECK_SDL_TEXTURE(sdl_texture, "&cSetColorModFloat(), sdl_texture '%s' was nullptr!")
#endif

	SDL_SetTextureColorModFloat(sdl_texture, color.r, color.g, color.b);
	return this;
}

Texture *Texture::SetAlphaMod(int alpha)
{
#ifndef NDEBUG
	CHECK_SDL_TEXTURE(sdl_texture, "&cSetAlphaMode(), sdl_texture '%s' was nullptr!")
#endif

	SDL_SetTextureAlphaMod(sdl_texture, alpha);
	return this;
}

Texture *Texture::SetScaleMode(SDL_ScaleMode scale_mode)
{
#ifndef NDEBUG
	CHECK_SDL_TEXTURE(sdl_texture, "&cSetScaleMode(), sdl_texture '%s' was nullptr!")
#endif

	SDL_SetTextureScaleMode(sdl_texture, scale_mode);
	return this;
}