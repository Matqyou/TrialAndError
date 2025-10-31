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
	float zoom;

public:
	Camera();
	explicit Camera(const Vec2f& start_pos);
	Camera(const Vec2f& start_pos, float start_zoom);
	~Camera();

	// Getting
	[[nodiscard]] Vec2f GetPos() const { return pos; }
	[[nodiscard]] float GetZoom() const { return zoom; }

	// Calculations
	[[nodiscard]] float CameraToScreenX(float x) const;
	[[nodiscard]] float CameraToScreenY(float y) const;
	[[nodiscard]] float ScreenToCameraX(float x) const;
	[[nodiscard]] float ScreenToCameraY(float y) const;

	template <class T>
	[[nodiscard]] Vec2f CameraToScreenPoint(Vec2<T> point) const
	{
		return {
			CameraToScreenX(point.x),
			CameraToScreenY(point.y)
		};
	}

	template <class T>
	[[nodiscard]] Vec2f ScreenToCameraPoint(Vec2<T> point) const
	{
		return {
			ScreenToCameraX(point.x),
			ScreenToCameraY(point.y)
		};
	}
	[[nodiscard]] SDL_FRect ScreenToCameraRect(const SDL_FRect& rect) const;

	// Manipulating
	void SetPos(const Vec2f& new_pos) { pos = new_pos; }
	void SetZoom(float new_zoom) { zoom = new_zoom; }

};
