// src/game/interface/PauseMenu.cpp

#include "PauseMenu.h"

PauseMenu::PauseMenu(GameWorld *GameWorld) : m_GameWorld(GameWorld)
{
	AssetsManager *assetsHandler = m_GameWorld->GameWindow()->Assetz();
	Assets *decals = Assets::Get();

	m_GameWindow = m_GameWorld->GameWindow();
	m_TextureResume = decals->GetTexture("interface.resume");
	m_TextureBack = decals->GetTexture("interface.back");
	m_Paused = false;
	m_ResumeButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) - 150, 200, 70};
	m_BackToMenuButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) + 50, 200, 70};
}

PauseMenu::~PauseMenu()
{
	// Clean up textures if necessary
}

void PauseMenu::Show()
{
	m_GameWorld->SetPaused(true);
	m_Paused = true;
}

void PauseMenu::HandleEvent(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_QUIT:
		m_GameWindow->Deinitialize(true);
		while (Mix_Playing(-1))
		{
		} // wait until last sound is done playing
		delete m_GameWindow;
		break;
	case SDL_MOUSEBUTTONDOWN:
	{
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			int x = event.button.x;
			int y = event.button.y;

			// If the player clicked the resume button -> unpause
			if (x >= m_ResumeButtonRect.x && x < m_ResumeButtonRect.x + m_ResumeButtonRect.w &&
				y >= m_ResumeButtonRect.y && y < m_ResumeButtonRect.y + m_ResumeButtonRect.h)
			{
				Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
				m_Paused = false;
				m_GameWorld->SetPaused(false);
				return;
			}

			// If the player clicked the back-to-menu button -> go back to main menu
			if (x >= m_BackToMenuButtonRect.x && x < m_BackToMenuButtonRect.x + m_BackToMenuButtonRect.w &&
				y >= m_BackToMenuButtonRect.y && y < m_BackToMenuButtonRect.y + m_BackToMenuButtonRect.h)
			{
				Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
				m_Paused = false;
				m_GameWorld->SetPaused(false);
				m_GameWindow->Menu()->Show();
				return;
			}
		}
	}
	break;
	case SDL_MOUSEMOTION:
	{
		int x = event.motion.x;
		int y = event.motion.y;

		// Update hover flags based on current mouse position
		m_ResumeHover = (x >= m_ResumeButtonRect.x && x < m_ResumeButtonRect.x + m_ResumeButtonRect.w &&
						 y >= m_ResumeButtonRect.y && y < m_ResumeButtonRect.y + m_ResumeButtonRect.h);
		m_BackToMenuHover = (x >= m_BackToMenuButtonRect.x && x < m_BackToMenuButtonRect.x + m_BackToMenuButtonRect.w &&
							 y >= m_BackToMenuButtonRect.y && y < m_BackToMenuButtonRect.y + m_BackToMenuButtonRect.h);

		bool hovering = m_ResumeHover || m_BackToMenuHover;
		SDL_SetCursor(SDL_CreateSystemCursor(hovering ? SDL_SYSTEM_CURSOR_HAND : SDL_SYSTEM_CURSOR_ARROW));
	}
	break;
	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			m_ResumeButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) - 150, 200,
								  70};
			m_BackToMenuButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) + 50,
									  200, 70};
			m_GameWindow->Render()->UpdateWindow();
		}
		break;
	case SDL_KEYDOWN:
		SDL_Scancode ScancodeKey = event.key.keysym.scancode;
		if (ScancodeKey == SDL_SCANCODE_ESCAPE)
		{
			Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
			m_Paused = false;
			m_GameWorld->SetPaused(false);
			return;
		}
		break;
	}
}

void PauseMenu::Render()
{
	Drawing *render = m_GameWindow->Render();
	SDL_Renderer *renderer = m_GameWindow->Renderer();
	// Ensure blending is enabled for overlays and texture alpha/mod
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	render->RenderButton(m_TextureResume->SDLTexture(), m_ResumeButtonRect, m_ResumeHover);

	// Draw back button (texture)
	render->RenderButton(m_TextureBack->SDLTexture(), m_BackToMenuButtonRect, m_BackToMenuHover);
	render->UpdateWindow();
}