//
// Created by Matq on 13/04/2023.
//

#include "HealthBar.h"

HealthBar::HealthBar(HasHealth *health_component,
					 int width,
					 int height,
					 int spacing_w,
					 int spacing_h)
{
	m_HealthComponent = health_component;
	m_Width = width;
	m_Height = height;
	m_SpacingW = spacing_w;
	m_SpacingH = spacing_h;
	m_Ratio = 0.0;

	m_r = 0;
	m_g = 255;
	m_b = 0;
	m_a = 0;
	m_InnerWidth = m_Width - m_SpacingW * 2;
	m_InnerHeight = m_Height - m_SpacingH * 2;
	m_Texture = Assets.CreateTexture(AssetsClass::TexturePurpose::RENDER_TARGET, Vec2i(width, height));

	// HealthComponent validation (might not be the prettiest)
#ifndef NDEBUG
	if (m_HealthComponent == nullptr)
	{ throw std::runtime_error("HealthComponent cannot be nullptr"); }
#endif

	m_HealthReference = &m_HealthComponent->m_Health;
	m_MaxHealthReference = &m_HealthComponent->m_MaxHealth;
}

HealthBar::~HealthBar()
{
	delete m_Texture;
}

void HealthBar::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
}

Texture *HealthBar::UpdateTexture()
{
//	m_Ratio = *m_HealthReference / *m_MaxHealthReference;
//	int InnerWidth = (int)((double)(m_InnerWidth) * m_Ratio);
//
//	auto drawing = Application.GetDrawing();
//	drawing->SetRenderTarget(m_Texture);
//	drawing->SetColor(255, 255, 255, 255);
//	drawing->Clear();
//
//	drawing->SetColor(m_r, m_g, m_b, m_a);
//	SDL_FRect FillRect = { (float)m_SpacingW, (float)m_SpacingH, (float)InnerWidth, (float)m_InnerHeight };
//	drawing->DrawRect(FillRect, true);
//	drawing->SetRenderTarget(nullptr);

	return m_Texture;
}
