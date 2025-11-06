//
// Created by Matq on 04/11/2025.
//

#include "VisualSurface.h"

VisualSurface::VisualSurface(SDL_Surface *surface, const Rect4f& hitbox)
	: Surface(surface)
{
	if (sdl_surface)
	{
		this->translation_ratios = {
			physical_hitbox.x / (float)surface->w,
			physical_hitbox.y / (float)surface->h,
			(float)surface->w / physical_hitbox.w,
			(float)surface->h / physical_hitbox.h,
		};
	}
	else
	{
		translation_ratios = { 1.0, 1.0, 1.0, 1.0 };
	}

	physical_hitbox = hitbox;
	uses_hitbox = true;
}

VisualSurface::~VisualSurface()
{

}

Rect4f VisualSurface::GetResultingPlacement(const Rect4f& physical_placement) const
{
	return {
		physical_placement.x - physical_placement.w * translation_ratios.x,
		physical_placement.y - physical_placement.h * translation_ratios.y,
		physical_placement.w * translation_ratios.w,
		physical_placement.h * translation_ratios.h
	};
}
