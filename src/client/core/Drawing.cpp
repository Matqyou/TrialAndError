//
// Created by Matq on 26/03/2023.
//

#include "Drawing.h"
#include "client/game/GameWorld.h"

Drawing::Drawing(GameData *game_window)
{
	m_GameWindow = game_window;
	m_World = nullptr;
	m_Renderer = m_GameWindow->Renderer();
	m_CameraX = 0.0;
	m_CameraY = 0.0;
	m_Zoom = 0.5;
}

Drawing::~Drawing() = default;

double Drawing::DetranslateX(double x)
{
	return (x - m_World->GameWindow()->GetWidth2()) / m_Zoom + m_CameraX;
}

double Drawing::DetranslateY(double y)
{
	return (y - m_World->GameWindow()->GetHeight2()) / m_Zoom + m_CameraY;
}

double Drawing::TranslateX(double x)
{
	return (x - m_CameraX) * m_Zoom + m_World->GameWindow()->GetWidth2();
}

double Drawing::TranslateY(double y)
{
	return (y - m_CameraY) * m_Zoom + m_World->GameWindow()->GetHeight2();
}

SDL_Rect Drawing::TranslateRect(const SDL_Rect& rect)
{
	return { int(TranslateX(rect.x)),
			 int(TranslateY(rect.y)),
			 int(rect.w * m_Zoom),
			 int(rect.h * m_Zoom) };
}

SDL_FRect Drawing::TranslateFRect(const SDL_FRect& rect)
{
	return { float(TranslateX(rect.x)),
			 float(TranslateY(rect.y)),
			 float(rect.w * m_Zoom),
			 float(rect.h * m_Zoom) };
}

void Drawing::SetRenderTarget(Texture *target)
{
	if (target) SDL_SetRenderTarget(m_Renderer, target->SDLTexture());
	else SDL_SetRenderTarget(m_Renderer, nullptr);
}

void Drawing::SetCameraPos(double camera_x, double camera_y)
{
	m_CameraX = camera_x;
	m_CameraY = camera_y;
}

void Drawing::LineCamera(double x1, double y1, double x2, double y2)
{
	SDL_RenderDrawLine(m_Renderer, int(TranslateX(x1)), int(TranslateY(y1)),
					   int(TranslateX(x2)), int(TranslateY(y2)));
}

void Drawing::DrawRectCamera(const SDL_Rect& rect)
{
	SDL_Rect MovedRect = TranslateRect(rect);
	SDL_RenderDrawRect(m_Renderer, &MovedRect);
}

void Drawing::FillRect(const SDL_Rect& rect)
{
	SDL_RenderFillRect(m_Renderer, &rect);
}

void Drawing::FillRectCamera(const SDL_Rect& rect)
{
	SDL_Rect MovedRect = TranslateRect(rect);
	SDL_RenderFillRect(m_Renderer, &MovedRect);
}

void Drawing::FillRectFCamera(const SDL_FRect& rect)
{
	SDL_FRect MovedRect = TranslateFRect(rect);
	SDL_RenderFillRectF(m_Renderer, &MovedRect);
}

void Drawing::RenderTexture(SDL_Texture *texture, SDL_Rect *srcrect, const SDL_Rect& dstrect)
{
	SDL_RenderCopy(m_Renderer, texture, srcrect, &dstrect);
}

void Drawing::RenderButton(SDL_Texture *texture, const SDL_Rect &dstrect, bool hover)
{
	// Ensure blending
	SDL_SetRenderDrawBlendMode(m_Renderer, SDL_BLENDMODE_BLEND);

	if (hover)
	{
		SDL_Rect shadow = dstrect;
		shadow.x += 4; shadow.y += 4; shadow.w += 6; shadow.h += 6;
		SDL_SetRenderDrawColor(m_Renderer, 0, 0, 0, 110);
		SDL_RenderFillRect(m_Renderer, &shadow);

		SDL_Rect hoveredRect = dstrect;
		hoveredRect.x -= 2; hoveredRect.y -= 2; hoveredRect.w += 4; hoveredRect.h += 4;
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

		// draw a slightly brightened/scaled version for the hover pop
		SDL_SetTextureColorMod(texture, 220, 220, 220);
		SDL_RenderCopy(m_Renderer, texture, nullptr, &hoveredRect);

		// restore
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}
	else
	{
		SDL_RenderCopy(m_Renderer, texture, nullptr, &dstrect);
	}
}

void Drawing::RenderTextureCamera(SDL_Texture *texture, SDL_Rect *srcrect, const SDL_Rect& dstrect)
{
	SDL_Rect MovedRect = TranslateRect(dstrect);
	SDL_RenderCopy(m_Renderer, texture, srcrect, &MovedRect);
}

void Drawing::RenderTextureExCamera(SDL_Texture *texture,
									SDL_Rect *srcrect,
									const SDL_Rect& dstrect,
									double angle,
									SDL_Point *center,
									SDL_RendererFlip flip)
{
	SDL_Rect MovedRect = TranslateRect(dstrect);
	SDL_RenderCopyEx(m_Renderer, texture, srcrect, &MovedRect, angle, center, flip);
}

void Drawing::RenderTextureF(SDL_Texture *texture, SDL_Rect *srcrect, const SDL_FRect& dstrect)
{
	SDL_RenderCopyF(m_Renderer, texture, srcrect, &dstrect);
}

void Drawing::RenderTextureFCamera(SDL_Texture *texture, SDL_Rect *srcrect, const SDL_FRect& dstrect)
{
	SDL_FRect MovedRect = TranslateFRect(dstrect);
	SDL_RenderCopyF(m_Renderer, texture, srcrect, &MovedRect);
}

void Drawing::RenderTextureExFCamera(SDL_Texture *texture,
									 SDL_Rect *srcrect,
									 const SDL_FRect& dstrect,
									 double angle,
									 SDL_FPoint *center,
									 SDL_RendererFlip flip)
{
	SDL_FRect MovedRect = TranslateFRect(dstrect);
	SDL_RenderCopyExF(m_Renderer, texture, srcrect, &MovedRect, angle, center, flip);
}

void Drawing::RenderDebugPointCamera(double x, double y)
{
	int centerX = int(TranslateX(x));
	int centerY = int(TranslateY(y));
	int radius = 3;

	for (int y = -radius; y <= radius; ++y)
	{
		int dx = static_cast<int>(std::sqrt(radius * radius - y * y)); // Horizontal distance for this vertical offset
		int startX = centerX - dx;
		int endX = centerX + dx;

		// Draw a horizontal line for the current row
		SDL_RenderDrawLine(m_Renderer, startX, centerY + y, endX, centerY + y);
	}
}

void Drawing::RenderTextureFullscreen(SDL_Texture *texture, SDL_Rect *srcrect)
{
	SDL_RenderCopy(m_Renderer, texture, srcrect, nullptr);
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