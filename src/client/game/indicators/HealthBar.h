//
// Created by Matq on 13/04/2023.
//

#pragma once

#include "client/GameData.h"
#include "../entities/Entity.h"

class HealthBar
{
private:
	GameData *m_GameWindow;
	HasHealth *m_HealthComponent; // Might stay unused outside of constructor
	double *m_HealthReference, *m_MaxHealthReference;
	int m_Width, m_Height, m_SpacingW, m_SpacingH;
	double m_Ratio;

	Uint8 m_r, m_g, m_b, m_a;
	int m_InnerWidth, m_InnerHeight;
	Texture *m_Texture;

public:
	HealthBar(GameData *game_window,
			  HasHealth *health_component,
			  int width,
			  int height,
			  int spacing_w,
			  int spacing_h);
	~HealthBar();

	// Getting
	[[nodiscard]] Texture *GetTexture() const { return m_Texture; }

	// Setting
	void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

	// Generating
	Texture *UpdateTexture();
};
