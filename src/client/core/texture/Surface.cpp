//
// Created by Matq on 04/11/2025.
//

#include "Surface.h"
#include "client/core/Assets.h"

Surface::Surface(SDL_Surface *surface)
{
	sdl_surface = surface;
	uses_hitbox = false;

	Assets.NewSurface(this);
}

bool Surface::Destroy()
{
	Assets.RemoveSurface(this);
	bool destroy = (sdl_surface != nullptr);

	SDL_DestroySurface(sdl_surface);
	sdl_surface = nullptr;

	return destroy;
}

Surface::~Surface()
{
	Destroy();
}