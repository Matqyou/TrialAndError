//
// Created by Matq on 24/01/2025.
//

#include "VisualTexture.h"
#include "client/core/Assets.h"

VisualTexture::VisualTexture(const Vec2i& init_size, SDL_GPUTexture *init_gpu_texture, const Rect4f& init_physical_hitbox)
	: Texture(init_size, init_gpu_texture)
{
	if (this->gpu_texture)
	{
		this->translation_ratios = {
			init_physical_hitbox.x / (float)texture_size.x,
			init_physical_hitbox.y / (float)texture_size.y,
			(float)texture_size.x / init_physical_hitbox.w,
			(float)texture_size.y / init_physical_hitbox.h,
		};
	}
	else
	{ this->translation_ratios = { 1.0, 1.0, 1.0, 1.0 }; }

	this->physical_hitbox = init_physical_hitbox;
	this->uses_hitbox = true;
}

VisualTexture::~VisualTexture()
{

}

Rect4f VisualTexture::GetResultingPlacement(const Rect4f& physical_placement) const
{
	return { physical_placement.x - physical_placement.w * translation_ratios.x,
			 physical_placement.y - physical_placement.h * translation_ratios.y,
			 physical_placement.w * translation_ratios.w,
			 physical_placement.h * translation_ratios.h };
};

VisualTextureInstance::VisualTextureInstance(Texture *init_texture)
{
	texture = init_texture;
	physical_placement = Rect4f(0.0f, 0.0f, 64.0f, 64.0f);
	resulting_placement = Rect4f(0.0f, 0.0f, 64.0f, 64.0f);
}

VisualTextureInstance::VisualTextureInstance(Texture *init_texture, const Rect4f& initial_placement)
{
	texture = init_texture;
	physical_placement = initial_placement;
	resulting_placement = initial_placement; //
}

VisualTextureInstance::~VisualTextureInstance()
{

}

void VisualTextureInstance::ChangeTexture(Texture *new_texture)
{
	this->texture = new_texture;

	UpdateResultingPlacement();
}

void VisualTextureInstance::UpdateResultingPlacement()
{
	if (texture == nullptr) return;

	if (texture->UsesHitbox())
	{
		auto *visual_texture = (VisualTexture *)texture;
		this->resulting_placement = visual_texture->GetResultingPlacement(physical_placement);
	}
	else
	{ this->resulting_placement = physical_placement; } // little sussy and maybe expensive
}

void VisualTextureInstance::UpdateWithNewPlacement(const SDL_FRect& new_placement)
{
	this->physical_placement = Rect4f(new_placement.x, new_placement.y, new_placement.w, new_placement.h);

	UpdateResultingPlacement();
}
