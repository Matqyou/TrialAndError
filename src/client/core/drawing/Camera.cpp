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

Camera::Camera(const Vec2f& start_pos)
{
	pos = start_pos;
	zoom = 1.0;
}

Camera::Camera(const Vec2f& start_pos, float start_zoom)
{
	pos = start_pos;
	zoom = start_zoom;
}

Camera::~Camera()
{

}

// Camera → Screen (multiply by zoom, add offset)
float Camera::CameraToScreenX(float x) const
{
	return (x - pos.x) * zoom + static_cast<float>(Application.GetWidth2());
}

float Camera::CameraToScreenY(float y) const
{
	return (y - pos.y) * zoom + static_cast<float>(Application.GetHeight2());
}

// Screen → Camera (subtract offset, divide by zoom)
float Camera::ScreenToCameraX(float x) const
{
	return (x - static_cast<float>(Application.GetWidth2())) / zoom + pos.x;
}

float Camera::ScreenToCameraY(float y) const
{
	return (y - static_cast<float>(Application.GetHeight2())) / zoom + pos.y;
}

//template <class T>
//Vec2f Camera::CameraToScreenPoint(Vec2<T> point) const
//{
//	return {
//		CameraToScreenX(point.x),
//		CameraToScreenY(point.y)
//	};
//}
//
//template <class T>
//Vec2f Camera::ScreenToCameraPoint(Vec2<T> point) const
//{
//	return {
//		ScreenToCameraX(point.x),
//		ScreenToCameraY(point.y)
//	};
//}

SDL_FRect Camera::CameraToScreenRect(const SDL_FRect& rect) const
{
	return { float(CameraToScreenX(rect.x)),
			 float(CameraToScreenY(rect.y)),
			 float(rect.w * zoom),
			 float(rect.h * zoom) };
}

SDL_FRect Camera::ScreenToCameraRect(const SDL_FRect& rect) const
{
	return { float(ScreenToCameraX(rect.x)),
			 float(ScreenToCameraY(rect.y)),
			 float(rect.w / zoom),
			 float(rect.h / zoom) };
}
