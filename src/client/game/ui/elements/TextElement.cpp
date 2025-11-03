//
// Created by Matq on 23/01/2025.
//

#include "TextElement.h"
#include <client/core/Assets.h>

TextElement::TextElement()
	: Element()
{
	this->name = "TextElement";
	this->generated_text = nullptr;

	this->SetSize(Vec2i(60, 40));
	this->SetDraw(ElementDraw::DRAW_TEXTURE);
}

TextElement::~TextElement()
{
	delete generated_text;
}

TextElement *TextElement::UpdateText(TTF_Font *ttf_font, const char *new_text, SDL_Color sdl_color)
{
	delete generated_text;

	this->generated_text = Assets.RenderTextBlended(ttf_font, new_text, sdl_color);     // TODO: very hacky, extra resources used for no reason
	Vec2f text_size = generated_text->GetSize();
	this->texture_instance.ChangeTexture(generated_text);
	this->size = Vec2i(text_size);
	return this;
}

TextElement *TextElement::UpdateTextOutline(TTF_Font *ttf_font, const char *new_text, int outline_thickness,
											SDL_Color text_color, SDL_Color outline_color)
{
	delete generated_text;

	this->generated_text = Assets.RenderTextBlendedOutline(ttf_font, new_text, outline_thickness,
															text_color, outline_color);     // TODO: very hacky, extra resources used for no reason
	auto generated_text_visual = (VisualTexture *)generated_text;
	auto text_hitbox = generated_text_visual->GetHitbox();
	this->texture_instance.ChangeTexture(generated_text);
	this->size = Vec2i((int)text_hitbox.w, (int)text_hitbox.h);
	return this;
}