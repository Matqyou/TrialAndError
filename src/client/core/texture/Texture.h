//
// Created by Matq on 19/06/2025.
//

#pragma once

#include <shared/geometry/Rectangles.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>

struct TextureInfo
{
	Uint32 format;
	int access, w, h;
};

class Texture
{
protected:
	SDL_GPUTexture *gpu_texture;
	const Vec2i texture_size;

	bool uses_hitbox; // if is VisualTexture subclass

public:
	explicit Texture(const Vec2i& init_size, SDL_GPUTexture *init_gpu_texture);
	bool Destroy();
	virtual ~Texture();

	// Getting
	[[nodiscard]] SDL_GPUTexture *GPUTexture() const { return gpu_texture; }
	[[nodiscard]] const Vec2i& GetSize() const { return texture_size; }
	[[nodiscard]] float GetWidth() const { return static_cast<float>(texture_size.x); }
	[[nodiscard]] float GetHeight() const { return static_cast<float>(texture_size.y); }
	[[nodiscard]] bool UsesHitbox() const { return uses_hitbox; }

	// Generating
//	[[nodiscard]] Texture *CopyTexture(SDL_TextureAccess texture_access) const;

	// Manipulating
//	Texture *SetBlendMode(SDL_BlendMode blend_mode);
//	Texture *SetColorMod(Uint8 r, Uint8 g, Uint8 b);
//	Texture *SetColorMod(SDL_Color color);
//	Texture *SetColorModFloat(float r, float g, float b);
//	Texture *SetColorModFloat(SDL_FColor color);
//	Texture *SetAlphaMod(int alpha);
//	Texture *SetScaleMode(SDL_ScaleMode scale_mode);

};