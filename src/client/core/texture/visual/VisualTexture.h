//
// Created by Matq on 24/01/2025.
//

#pragma once

#include "client/core/texture/visual/HitboxFile.h"
#include "client/core/texture/Texture.h"

class VisualTexture : public Texture
{
private:
	Rect4f physical_hitbox;
	Rect4f translation_ratios;

public:
	VisualTexture(const Vec2i& init_size, SDL_GPUTexture *init_gpu_texture, const Rect4f& init_physical_hitbox);
	~VisualTexture() override;

	// Getting
	[[nodiscard]] const Rect4f& GetHitbox() const { return physical_hitbox; }
	[[nodiscard]] Rect4f GetResultingPlacement(const Rect4f& physical_placement) const;

};

class VisualTextureInstance
{
private:
	Texture *texture;
	Rect4f physical_placement;
	Rect4f resulting_placement;

public:
	explicit VisualTextureInstance(Texture *init_texture);
	VisualTextureInstance(Texture *init_texture, const Rect4f& initial_placement);
	~VisualTextureInstance();

	// Getting
	[[nodiscard]] Texture *GetTexture() const { return texture; }
	[[nodiscard]] const Rect4f& GetResultingRect() const { return resulting_placement; }
	[[nodiscard]] SDL_FRect GetResultingFRect() const
	{
		return { resulting_placement.x, resulting_placement.y, resulting_placement.w, resulting_placement.h };
	}

	// Manipulating
	void ChangeTexture(Texture *new_texture);
	void UpdateResultingPlacement();
	void UpdateWithNewPlacement(const SDL_FRect& new_placement);

};
