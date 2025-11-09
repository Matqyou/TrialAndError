//
// Created by Matq on 19/06/2025.
//

#include "client/core/texture/visual/VisualTexture.h"
#include "client/core/drawing/Drawing.h"
#include "client/core/Assets.h"
#include "Texture.h"

Texture::Texture(const Vec2i& init_size, SDL_GPUTexture *init_gpu_texture)
	: texture_size(init_size)
{
	gpu_texture = init_gpu_texture;
	uses_hitbox = false;

	Assets.NewTexture(this);
}

bool Texture::Destroy()
{
	Assets.RemoveTexture(this);
	bool destroy = (gpu_texture != nullptr);

	SDL_ReleaseGPUTexture(Drawing.Device(), gpu_texture);
	gpu_texture = nullptr;

	return destroy;
}

Texture::~Texture()
{
	Destroy();
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