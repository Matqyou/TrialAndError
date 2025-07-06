//
// Created by Matq on 20/06/2025.
//

#include "Camera.h"
#include "client/game/GameReference.h"

Camera::Camera()
{
	pos = Vec2f(0.0f, 0.0f);
	zoom = 1.0;
}

Camera::Camera(Vec2f start_pos)
{
	pos = start_pos;
	zoom = 1.0;
}

Camera::Camera(Vec2f start_pos, double start_zoom)
{
	pos = start_pos;
	zoom = start_zoom;
}

Camera::~Camera()
{

}

float Camera::DetranslateX(float x) const
{
	return (x - Application.GetWidth2()) / zoom + pos.x;
}

float Camera::DetranslateY(float y) const
{
	return (y - Application.GetHeight2()) / zoom + pos.y;
}

float Camera::TranslateX(float x) const
{
	return (x - pos.x) * zoom + Application.GetWidth2();
}

float Camera::TranslateY(float y) const
{
	return (y - pos.y) * zoom + Application.GetHeight2();
}

SDL_FRect Camera::TranslateRect(const SDL_FRect& rect) const
{
	return { float(TranslateX(rect.x)),
			 float(TranslateY(rect.y)),
			 float(rect.w * zoom),
			 float(rect.h * zoom) };
}