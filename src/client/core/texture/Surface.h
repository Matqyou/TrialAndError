//
// Created by Matq on 04/11/2025.
//

#pragma once

#include <shared/geometry/Rectangles.h>
#include <SDL3/SDL.h>

class Surface
{
protected:
	SDL_Surface *sdl_surface;
	bool uses_hitbox;
	bool flagged_for_automatic_deletion;

public:
	explicit Surface(SDL_Surface *surface);
	virtual ~Surface();

	// Getting
	[[nodiscard]] SDL_Surface *SDLSurface() const { return sdl_surface; }
	[[nodiscard]] Vec2i GetSize() const { return { sdl_surface->w, sdl_surface->h }; }
	[[nodiscard]] float GetWidth() const { return static_cast<float>(sdl_surface->w); }
	[[nodiscard]] float GetHeight() const { return static_cast<float>(sdl_surface->h); }
	[[nodiscard]] bool UsesHitbox() const { return uses_hitbox; }

	Surface *FlagForAutomaticDeletion();

};