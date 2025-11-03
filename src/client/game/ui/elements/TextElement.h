//
// Created by Matq on 23/01/2025.
//

#pragma once

#include "Element.h"

class TextElement : public Element
{
private:
	Texture *generated_text;

public:
	TextElement();
	~TextElement() override;

	// Manipulation
	TextElement *UpdateText(TTF_Font *ttf_font, const char *new_text, SDL_Color sdl_color);
	TextElement *UpdateTextOutline(TTF_Font *ttf_font, const char *new_text, int outline_thickness,
								   SDL_Color text_color, SDL_Color outline_color);

};