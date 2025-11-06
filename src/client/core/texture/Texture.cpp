//
// Created by Matq on 19/06/2025.
//

#include "client/core/texture/visual/VisualTexture.h"
#include "client/core/drawing/Drawing.h"
#include "client/core/Assets.h"
#include "Texture.h"

//#define CHECK_SDL_TEXTURE(sdl_texture, message) \
//    if ((sdl_texture) == nullptr) { \
//        std::cout << Strings::FStringColors("[Texture] &c" message "\n"); \
//        return this; \
//    }

Texture::Texture(const Vec2i& size, SDL_GPUTexture *texture)
	: texture_size(size)
{
	this->gpu_texture = texture;
	this->flagged_for_automatic_deletion = false;
	this->uses_hitbox = false;
}

Texture::~Texture()
{
	if (gpu_texture != nullptr)
	{
		SDL_ReleaseGPUTexture(Drawing.Device(), gpu_texture);
//		SDL_DestroyTexture(gpu_texture);
		gpu_texture = nullptr;
	}
}

//Texture *Texture::CopyTexture(SDL_TextureAccess texture_access) const
//{
//	Texture *new_texture;
//	if (uses_hitbox)
//		new_texture = new VisualTexture(*(VisualTexture *)(this));
//	else
//		new_texture = new Texture(*this);
//
//	new_texture->gpu_texture = Assets.CopySDLTexture(gpu_texture, texture_access);
//	return new_texture;
//}

Texture *Texture::FlagForAutomaticDeletion()
{
	if (!flagged_for_automatic_deletion)
	{
		flagged_for_automatic_deletion = true;
		AssetsClass::AutomaticallyDeleteTexture(this);
	}
	return this;
}

//Texture *Texture::SetSDLTexture(SDL_Texture *new_gpu_texture)
//{
//	this->gpu_texture = new_gpu_texture;
//	SDL_GetTextureSize(new_gpu_texture, &texture_size.x, &texture_size.y);
//	return this;
//}
//
//Texture *Texture::SetBlendMode(SDL_BlendMode blend_mode)
//{
//#ifndef NDEBUG
//	CHECK_SDL_TEXTURE(gpu_texture, "&cSetBlendMode(), sdl_texture was nullptr!")
//#endif
//
//	SDL_SetTextureBlendMode(gpu_texture, blend_mode);
//	return this;
//}
//
//Texture *Texture::SetColorMod(Uint8 r, Uint8 g, Uint8 b)
//{
//#ifndef NDEBUG
//	CHECK_SDL_TEXTURE(gpu_texture, "&cSetColorMod(), sdl_texture '%s' was nullptr!")
//#endif
//
//	SDL_SetTextureColorMod(gpu_texture, r, g, b);
//	return this;
//}
//
//Texture *Texture::SetColorMod(SDL_Color color)
//{
//#ifndef NDEBUG
//	CHECK_SDL_TEXTURE(gpu_texture, "&cSetColorMod(), sdl_texture '%s' was nullptr!")
//#endif
//
//	SDL_SetTextureColorMod(gpu_texture, color.r, color.g, color.b);
//	return this;
//}
//
//Texture *Texture::SetColorModFloat(float r, float g, float b)
//{
//#ifndef NDEBUG
//	CHECK_SDL_TEXTURE(gpu_texture, "&cSetColorModFloat(), sdl_texture '%s' was nullptr!")
//#endif
//
//	SDL_SetTextureColorModFloat(gpu_texture, r, g, b);
//	return this;
//}
//
//Texture *Texture::SetColorModFloat(SDL_FColor color)
//{
//#ifndef NDEBUG
//	CHECK_SDL_TEXTURE(gpu_texture, "&cSetColorModFloat(), sdl_texture '%s' was nullptr!")
//#endif
//
//	SDL_SetTextureColorModFloat(gpu_texture, color.r, color.g, color.b);
//	return this;
//}
//
//Texture *Texture::SetAlphaMod(int alpha)
//{
//#ifndef NDEBUG
//	CHECK_SDL_TEXTURE(gpu_texture, "&cSetAlphaMode(), sdl_texture '%s' was nullptr!")
//#endif
//
//	SDL_SetTextureAlphaMod(gpu_texture, alpha);
//	return this;
//}
//
//Texture *Texture::SetScaleMode(SDL_ScaleMode scale_mode)
//{
//#ifndef NDEBUG
//	CHECK_SDL_TEXTURE(gpu_texture, "&cSetScaleMode(), sdl_texture '%s' was nullptr!")
//#endif
//
//	SDL_SetTextureScaleMode(gpu_texture, scale_mode);
//	return this;
//}