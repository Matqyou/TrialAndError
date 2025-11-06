//
// Created by Matq on 15/04/2023.
//

#include "TextSurface.h"
#include <client/core/Assets.h>
#include <utility>

Texture *TextSurface::UpdateTexture()
{
	delete m_Texture;
	m_Texture = Assets.RenderTextBlended(m_Font, m_Text.c_str(), m_Color);

	m_Update = false;
	return m_Texture;
}

TextSurface::TextSurface(TTF_Font *font, const std::string& text, SDL_Color color)
{
	m_Texture = nullptr;
	m_Text = text;
	m_Font = font;
	m_Color = color;
	m_Update = true;

	UpdateTexture();
}

TextSurface::~TextSurface()
{
	delete m_Texture;
}

void TextSurface::SetText(const std::string& text)
{
	m_Text = text;
}

void TextSurface::SetFont(TTF_Font *font)
{
	m_Font = font;
}

void TextSurface::SetColor(const SDL_Color color)
{
	m_Color = color;
}

Texture *TextSurface::RequestUpdate()
{
	if (!m_Update) return m_Texture;

	UpdateTexture();
	return m_Texture;
}