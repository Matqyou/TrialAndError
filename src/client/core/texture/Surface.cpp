//
// Created by Matq on 04/11/2025.
//

#include "Surface.h"
#include "client/core/Assets.h"

Surface::Surface(SDL_Surface *surface)
{
	sdl_surface = surface;
	uses_hitbox = false;
}

Surface::~Surface()
{
	delete sdl_surface;
	sdl_surface = nullptr;
}

Surface *Surface::FlagForAutomaticDeletion()
{
	if (!flagged_for_automatic_deletion)
	{
		flagged_for_automatic_deletion = true;
		AssetsClass::AutomaticallyDeleteSurface(this);
	}
	return this;
}