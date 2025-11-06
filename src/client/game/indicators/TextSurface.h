//
// Created by Matq on 15/04/2023.
//

#pragma once

#include <client/core/texture/visual/VisualTexture.h>
#include "SDL3_ttf/SDL_ttf.h"

class TextSurface
{
private:
	Texture *m_Texture;
	std::string m_Text;
	TTF_Font *m_Font;
	SDL_Color m_Color;
	bool m_Update;

	// Generating
	Texture *UpdateTexture();

public:
	TextSurface(TTF_Font *font, const std::string& text, SDL_Color color);
	~TextSurface();

	// Getting
	[[nodiscard]] Texture *GetTexture() const { return m_Texture; }
	[[nodiscard]] std::string GetText() const { return m_Text; }
	[[nodiscard]] TTF_Font *GetFont() const { return m_Font; }
	[[nodiscard]] SDL_Color GetColor() const { return m_Color; }
	[[nodiscard]] bool GetFlaggedForUpdate() const { return m_Update; }

	// Setting
	void SetText(const std::string& text);
	void SetFont(TTF_Font *font);
	void SetColor(SDL_Color color);
	void FlagForUpdate() { m_Update = true; }

	// Generate
	Texture *RequestUpdate();
};
