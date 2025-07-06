//
// Created by Matq on 22/06/2025.
//

#include "StarBackground.h"
#include "client/core/Application.h"

StarBackground::StarBackground()
	: Element()
{
	star_accumulator = 0;

	SetColor(0, 0, 50, 255);
	SetDraw(DRAW_CUSTOM);
	SetFullyOccupy(true, true);
	SetFlexInvolved(false, false);
	SetName("StarBackground");
}

StarBackground::~StarBackground()
{

}

void StarBackground::Tick(double elapsed_seconds)
{
	star_accumulator += elapsed_seconds * 600.0; // Stars/sec
	int add_stars = static_cast<int>(star_accumulator);
	star_accumulator -= add_stars;
	
	for (int i = 0; i < add_stars; i++)
	{
		auto random_position = Vec2d(rand() % Application.GetWidth(), rand() % Application.GetHeight());
		m_Stars.emplace_back(random_position, Vec2d(0.0, 0.0), 5.0);
	}

	Vec2f MousePosition;
	SDL_GetMouseState(&MousePosition.x, &MousePosition.y);
	auto MousePositionD = Vec2d(MousePosition.x, MousePosition.y);
	for (int i = (int)m_Stars.size() - 1; i >= 0; --i)
	{
		auto& [position, velocity, duration] = m_Stars[i];

		if (SDL_GetMouseFocus()) // If mouse is in any window
		{ // Push around with the mouse
			auto direction = position - MousePositionD;
			double distance = direction.Length();
			velocity += direction.Normalize() / distance * elapsed_seconds * 5000;
		}
		velocity.x += (sin((position.x + position.y * 2) / 50)) * elapsed_seconds * 50;
		velocity *= pow(0.98, elapsed_seconds / (1.0 / Application.GetClock()->GetInitialFramerate()));

		position.x += velocity.x * elapsed_seconds;
		position.y += velocity.y * elapsed_seconds;

		if (position.x < 0)
			position.x += Application.GetWidth();
		else if (position.x > Application.GetWidth())
			position.x -= Application.GetWidth();

		if (position.y < 0)
			position.y += Application.GetHeight();
		else if (position.y > Application.GetHeight())
			position.y -= Application.GetHeight();

		duration -= elapsed_seconds;
		if (duration <= 0.0)
			m_Stars.erase(m_Stars.begin() + i);
	}
}

void StarBackground::Render()
{
	auto drawing = Application.GetDrawing();
	drawing->SetColor(color);
	drawing->FillAll();

	drawing->SetColor(200, 200, 200, 100);
	for (int i = m_Stars.size() - 1; i >= 0; --i)
	{
		auto& [position, velocity, duration] = m_Stars[i];

		auto size = (int)duration;
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < size; k++)
			{
				auto draw_x = (float)(position.x - size / 2 + j);
				auto draw_y = (float)(position.y - size / 2 + k);
				SDL_RenderPoint(drawing->Renderer(), draw_x, draw_y);
			}
		}
	}
}