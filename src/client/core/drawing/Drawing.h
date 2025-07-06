//
// Created by Matq on 26/03/2023.
//

#pragma once

#include "SDL3_image/SDL_image.h"
#include "client/core/Assets.h"
#include "Camera.h"

class GameData;
class GameWorld;
class Drawing
{
private:
	SDL_Renderer *m_Renderer;

public:
	explicit Drawing(SDL_Renderer *renderer);
	~Drawing();

	// Getting
	[[nodiscard]] SDL_Renderer *Renderer() const { return m_Renderer; }

	// Options
	void SetRenderTarget(Texture *target);
	void SetRenderTargetSDL(SDL_Texture *target);
	void SetDrawBlendMode(SDL_BlendMode blend_mode) { SDL_SetRenderDrawBlendMode(m_Renderer, blend_mode); }
	void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { SDL_SetRenderDrawColor(m_Renderer, r, g, b, a); }
	void SetColor(SDL_Color color) { SetColor(color.r, color.g, color.b, color.a); }

	// Drawing
	void DrawRect(const SDL_FRect& rect, bool fill);
	void DrawRect(const SDL_FRect& rect, bool fill, const Camera& camera);
	void FillCircle(const Vec2f& center, float radius, SDL_FColor color);
	void DrawLine(const Vec2f& start, const Vec2f& end);
	void DrawLine(const Vec2f& start, const Vec2f& end, const Camera& camera);
	void DrawLine(const Vec2f& start, const Vec2f& end, float size, SDL_FColor color);
	void DrawLine(const Vec2f& start, const Vec2f& end, float size, SDL_FColor color, const Camera& camera);
	void RenderTexture(SDL_Texture *texture, SDL_FRect *srcrect, const SDL_FRect& dstrect);
	void RenderTexture(SDL_Texture *texture, SDL_FRect *srcrect, const SDL_FRect& dstrect, const Camera& camera);
	void RenderTextureRotated(SDL_Texture *texture,
							  SDL_FRect *srcrect, const SDL_FRect& dstrect,
							  double angle, SDL_FPoint *center,
							  SDL_FlipMode flip);
	void RenderTextureRotated(SDL_Texture *texture,
							  SDL_FRect *srcrect, const SDL_FRect& dstrect,
							  double angle, SDL_FPoint *center,
							  SDL_FlipMode flip, const Camera& camera);
	void RenderTextureFullscreen(SDL_Texture *texture, SDL_FRect *srcrect);
	void Clear();  // Resets all pixels to a color value
	void FillAll();  // If color blending is enabled, fill the window with a semi-transparent tone
	void UpdateWindow();  // Pushes the latest state of the pixels to window

};
