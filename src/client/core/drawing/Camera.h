//
// Created by Matq on 20/06/2025.
//

#pragma once

#include "shared/utility/Vec2.h"
#include "SDL3/SDL_rect.h"

class Camera
{
private:
	Vec2f pos;
	double zoom;

public:
	Camera();
	explicit Camera(Vec2f start_pos);
	Camera(Vec2f start_pos, double start_zoom);
	~Camera();

	// Getting
	[[nodiscard]] Vec2f GetPos() const { return pos; }
	[[nodiscard]] double GetZoom() const { return zoom; }

	// Generating
	float DetranslateX(float x) const;
	float DetranslateY(float y) const;
	float TranslateX(float x) const;
	float TranslateY(float y) const;
	SDL_FRect TranslateRect(const SDL_FRect& rect) const;

	// Manipulating
	void SetPos(const Vec2f& new_pos) { pos = new_pos; }
	void SetZoom(double new_zoom) { zoom = new_zoom; }

};
