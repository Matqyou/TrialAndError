//
// Created by Matq on 19/06/2025.
//

#pragma once

#include <SDL3/SDL.h>
#include "SDL3_image/SDL_image.h"
#include "shared/geometry/Rectangles.h"

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
	bool flagged_for_automatic_deletion;

	bool uses_hitbox; // if is VisualTexture subclass

public:
	explicit Texture(const Vec2i& size, SDL_GPUTexture *texture);
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
	Texture *FlagForAutomaticDeletion();
//	Texture *SetBlendMode(SDL_BlendMode blend_mode);
//	Texture *SetColorMod(Uint8 r, Uint8 g, Uint8 b);
//	Texture *SetColorMod(SDL_Color color);
//	Texture *SetColorModFloat(float r, float g, float b);
//	Texture *SetColorModFloat(SDL_FColor color);
//	Texture *SetAlphaMod(int alpha);
//	Texture *SetScaleMode(SDL_ScaleMode scale_mode);

};