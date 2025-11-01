//
// Created by Matq on 26/03/2023.
//

#include "Drawing.h"

Drawing::Drawing(SDL_Renderer *renderer)
{
	m_Renderer = renderer;
}

Drawing::~Drawing() = default;

void Drawing::SetRenderTarget(Texture *target)
{
	if (target) SetRenderTargetSDL(target->SDLTexture());
	else SetRenderTargetSDL(nullptr);
}

void Drawing::SetRenderTargetSDL(SDL_Texture *target)
{
	SDL_SetRenderTarget(m_Renderer, target);
}

void Drawing::DrawRect(const SDL_FRect& rect, bool fill)
{
	if (fill)
		SDL_RenderFillRect(m_Renderer, &rect);
	else
		SDL_RenderRect(m_Renderer, &rect);
}

void Drawing::DrawRect(const SDL_FRect& rect, bool fill, const Camera& camera)
{
	SDL_FRect MovedRect = camera.CameraToScreenRect(rect);
	if (fill)
		SDL_RenderFillRect(m_Renderer, &MovedRect);
	else
		SDL_RenderRect(m_Renderer, &MovedRect);
}

void Drawing::FillCircle(const Vec2f& center, float radius, SDL_FColor color)
{
	const int segments = 20; // Number of segments for the circle
	float angle_step = 2.0f * M_PI / segments;

	for (int i = 0; i < segments; i++)
	{
		float theta1 = (float)i * angle_step;
		float theta2 = ((float)i + 1.0f) * angle_step;

		// Points on the circle
		float x1 = center.x + radius * cosf(theta1);
		float y1 = center.y + radius * sinf(theta1);
		float x2 = center.x + radius * cosf(theta2);
		float y2 = center.y + radius * sinf(theta2);

		// Draw triangles for the circle
		SDL_Vertex vertices[3] = {
			{{ (float)center.x, (float)center.y }, color, { 0, 0 }},
			{{ x1, y1 }, color, { 0, 0 }},
			{{ x2, y2 }, color, { 0, 0 }}
		};

		SDL_RenderGeometry(m_Renderer, nullptr, vertices, 3, nullptr, 0);
	}
}

void Drawing::DrawLine(const Vec2f& start, const Vec2f& end)
{
	SDL_RenderLine(m_Renderer, start.x, start.y, end.x, end.y);
}

void Drawing::DrawLine(const Vec2f& start, const Vec2f& end, const Camera& camera)
{
	SDL_RenderLine(m_Renderer,
				   camera.CameraToScreenX(start.x), camera.CameraToScreenY(start.y),
				   camera.CameraToScreenX(end.x), camera.CameraToScreenY(end.y));
}

void Drawing::DrawLine(const Vec2f& start, const Vec2f& end, float size, SDL_FColor color)
{
	float half_thickness = size / 2.0f;

	// Vector between start and end
	Vec2f direction = (end - start).Normalize();

	// Perpendicular vector to the direction
	Vec2f perp_dir(-direction.y, direction.x);

	// Calculate the vertices of the thick line rectangle
	Vec2f p1 = start + perp_dir * half_thickness;
	Vec2f p2 = start - perp_dir * half_thickness;
	Vec2f p3 = end - perp_dir * half_thickness;
	Vec2f p4 = end + perp_dir * half_thickness;

	SDL_Vertex vertices[4] = {
		{{ p1.x, p1.y }, color, { 0, 0 }},
		{{ p2.x, p2.y }, color, { 0, 0 }},
		{{ p3.x, p3.y }, color, { 0, 0 }},
		{{ p4.x, p4.y }, color, { 0, 0 }}
	};

	int indices[6] = {
		0, 1, 2,  // First triangle (p1 -> p2 -> p3)
		0, 2, 3   // Second triangle (p1 -> p3 -> p4)
	};

	SDL_RenderGeometry(m_Renderer, nullptr, vertices, 4, indices, 6);

	// Draw the rounded caps
	FillCircle(start, half_thickness, color);
	FillCircle(end, half_thickness, color);
}

void Drawing::DrawLine(const Vec2f& start, const Vec2f& end, float size, SDL_FColor color, const Camera& camera)
{
	Vec2f screen_start = camera.CameraToScreenPoint(start);
	Vec2f screen_end = camera.CameraToScreenPoint(end);

	auto screen_size = size * camera.GetZoom();
	DrawLine(screen_start, screen_end, screen_size, color);
}

void Drawing::RenderTexture(SDL_Texture *texture, SDL_FRect *srcrect, const SDL_FRect& dstrect)
{
	SDL_RenderTexture(m_Renderer, texture, srcrect, &dstrect);
}

void Drawing::RenderTexture(SDL_Texture *texture, SDL_FRect *srcrect, const SDL_FRect& dstrect, const Camera& camera)
{
	SDL_FRect MovedRect = camera.CameraToScreenRect(dstrect);
	SDL_RenderTexture(m_Renderer, texture, srcrect, &MovedRect);
}

void Drawing::RenderTextureRotated(SDL_Texture *texture,
								   SDL_FRect *srcrect, const SDL_FRect& dstrect,
								   double angle, SDL_FPoint *center,
								   SDL_FlipMode flip)
{
	SDL_RenderTextureRotated(m_Renderer, texture, srcrect, &dstrect, angle, center, flip);
}

void Drawing::RenderTextureRotated(SDL_Texture *texture,
								   SDL_FRect *srcrect, const SDL_FRect& dstrect,
								   double angle, SDL_FPoint *center,
								   SDL_FlipMode flip, const Camera& camera)
{
	SDL_FRect MovedRect = camera.CameraToScreenRect(dstrect);
	SDL_RenderTextureRotated(m_Renderer, texture, srcrect, &MovedRect, angle, center, flip);
}

void Drawing::RenderTextureFullscreen(SDL_Texture *texture, SDL_FRect *srcrect)
{
	SDL_RenderTexture(m_Renderer, texture, srcrect, nullptr);
}

void Drawing::Clear()
{
	SDL_RenderClear(m_Renderer);
}

void Drawing::FillAll()
{
	SDL_RenderFillRect(m_Renderer, nullptr);
}

void Drawing::UpdateWindow()
{
	SDL_RenderPresent(m_Renderer);
}
