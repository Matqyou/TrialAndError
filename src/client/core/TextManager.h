//
// Created by Matq on 19/03/2023.
//

#pragma once

#include <SDL3_ttf/SDL_ttf.h>
#include "Assets.h"
#include <vector>
#include <string>

std::string FStringColors(const char *format, ...);
std::string FString(const char *format, ...);
std::string ErasePrefix(std::string string, const std::string& prefix);
std::string EraseSuffix(std::string string, const std::string& suffix);

class TextManager
{
private:
	TTF_Font *m_MainFont;

	std::vector<TTF_Font *> m_Fonts;

public:
	explicit TextManager();
	~TextManager();

	// Getting
	[[nodiscard]] TTF_Font *GetFont(int font_index) const { return m_Fonts[font_index]; }
	[[nodiscard]] TTF_Font *GetMainFont() const { return m_MainFont; }

	// Generating
	[[nodiscard]] Texture *Render(TTF_Font *font, const char *text, SDL_Color color);
	TTF_Font *LoadFont(const char *filepath, int ptsize);
};
