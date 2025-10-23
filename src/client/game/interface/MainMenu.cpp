// src/game/interface/MainMenu.cpp

#include "MainMenu.h"
#include "./GameModeMenu.h"
#include "../../game/OverworldGameWorld.h"
#include "../Player.h"
#include "../entities/characters/character/Character.h"

LoadedMusic MainMenu::sElevatorMusic("intro");
LoadedTexture MainMenu::sMenuTexture("interface.menu");
LoadedTexture MainMenu::sTextureTitle("ui.main.title2");
LoadedTexture MainMenu::sTexturePlay("ui.main.playbutton2");
LoadedTexture MainMenu::sTextureExit("ui.main.exit2");
LoadedTexture MainMenu::sTextureTraining("ui.main.trainingbutton");

MainMenu::MainMenu(GameData *game_window)
	: m_GameWindow(game_window)
{
	// Center title horizontally, and place title near top quarter
	int centerX = (int)m_GameWindow->GetWidth2();
	int centerY = (int)m_GameWindow->GetHeight2();
	m_TitleRect = {centerX - 300, centerY / 2 - 150, 600, 200};

	// Layout buttons vertically centered under title, spaced evenly
	int totalWidth = (int)m_GameWindow->GetWidth();
	int buttonH = 80; // slightly taller
	int gap = 24;
	int exitW = 360; // match exit width for Play/Training
	int playW = exitW;
	int trainW = exitW;
	int startY = centerY - 60; // main buttons slightly above center

	// Place Play and Training side-by-side centered
	int rowX = centerX - (playW + gap + trainW) / 2;
	m_PlayButtonRect = {rowX, startY, playW, buttonH};
	m_TrainingButtonRect = {rowX + playW + gap, startY, trainW, buttonH};

	// Exit below, centered with same width
	m_ExitButtonRect = {centerX - exitW / 2, startY + buttonH + 32, exitW, buttonH};

	m_Opened = std::chrono::steady_clock::now();
	m_Intro = true;
}

MainMenu::~MainMenu()
{
}

void MainMenu::Show()
{
	m_Intro = false;
	this->InitialShow();
}

void MainMenu::InitialShow()
{
	GameReference.ResetPlayerClassMenus();
	GameReference.ResetPendingClasses();
	bool running = true;
	bool menuOpen = true;
	if(m_Intro)
	{
		try {
			sElevatorMusic.GetMusic()->PlayMusic(-1);
		} catch (const std::exception &e) {
			std::cout << FStringColors("[MainMenu] Failed to play elevator music: %s", e.what()) << std::endl;
		}
	}
	while (menuOpen)
	{
		Tick();
		Render();
		SDL_ShowCursor(1);
		SDL_Event currentEvent;
		while (SDL_PollEvent(&currentEvent))
		{
			m_GameWindow->Event(currentEvent);
			HandleEvent(currentEvent, running, menuOpen);
		}
		m_GameWindow->Render()->UpdateWindow();
	}
	m_GameWindow->Render()->Clear();
}

void MainMenu::HandleEvent(const SDL_Event &event, bool &running, bool &menuOpen)
{
	switch (event.type)
	{
	case SDL_QUIT:
		m_GameWindow->Deinitialize(true); // close everything except sound
		while (Mix_Playing(-1))
		{
		} // wait until last sound is done playing
		delete m_GameWindow;
		exit(0);
	case SDL_MOUSEBUTTONDOWN:
		if (m_Intro)
		{
			m_Intro = false;
			Mix_SetMusicPosition(16);
			break;
		}

		SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
		if (event.button.button == SDL_BUTTON_LEFT)
		{
			int x = event.button.x;
			int y = event.button.y;
			if (x >= m_TrainingButtonRect.x && x < m_TrainingButtonRect.x + m_TrainingButtonRect.w &&
				y >= m_TrainingButtonRect.y && y < m_TrainingButtonRect.y + m_TrainingButtonRect.h)
			{
				// Training -> previous Play behavior (game select)
				menuOpen = false;
				Assets::PauseMusic();
				Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
				m_GameWindow->GameSelectMenu()->Show();
			}
			else if (x >= m_PlayButtonRect.x && x < m_PlayButtonRect.x + m_PlayButtonRect.w &&
			y >= m_PlayButtonRect.y && y < m_PlayButtonRect.y + m_PlayButtonRect.h)
		{
			// Play -> Overworld (new larger world)
			menuOpen = false;
			Assets::PauseMusic();
			Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
			GameWorld *world = new OverworldGameWorld(m_GameWindow, 100, 60); // larger overworld
			m_GameWindow->SetWorld(world);

			// Spawn a default player without assigning a class yet (they'll choose class in-game)
			Player *player = new Player(world, "Player1", nullptr, false);
			// Create a character for this player at the world's center
			Vec2d spawnPos(world->GetWidth() / 2.0, world->GetHeight() / 2.0);
			new Character(world, player, 100.0, spawnPos, Vec2d(0, 0), false);
		}
			if (x >= m_ExitButtonRect.x && x < m_ExitButtonRect.x + m_ExitButtonRect.w &&
				y >= m_ExitButtonRect.y && y < m_ExitButtonRect.y + m_ExitButtonRect.h)
			{
				m_GameWindow->Deinitialize(true);
				while (Mix_Playing(-1))
				{
				} // wait until last sound is done playing
				delete m_GameWindow;
			}
		}
		break;
	case SDL_MOUSEMOTION:
	{
		if (m_Intro)
			break;

		int x = event.motion.x;
		int y = event.motion.y;
	m_PlayHover = (x >= m_PlayButtonRect.x && x < m_PlayButtonRect.x + m_PlayButtonRect.w &&
		       y >= m_PlayButtonRect.y && y < m_PlayButtonRect.y + m_PlayButtonRect.h);
	m_TrainingHover = (x >= m_TrainingButtonRect.x && x < m_TrainingButtonRect.x + m_TrainingButtonRect.w &&
		       y >= m_TrainingButtonRect.y && y < m_TrainingButtonRect.y + m_TrainingButtonRect.h);
	m_ExitHover = (x >= m_ExitButtonRect.x && x < m_ExitButtonRect.x + m_ExitButtonRect.w &&
		       y >= m_ExitButtonRect.y && y < m_ExitButtonRect.y + m_ExitButtonRect.h);

	bool hoveringAny = m_PlayHover || m_ExitHover || m_TrainingHover;
	SDL_SetCursor(SDL_CreateSystemCursor(hoveringAny ? SDL_SYSTEM_CURSOR_HAND : SDL_SYSTEM_CURSOR_ARROW));
	}
	break;
	case SDL_WINDOWEVENT:
		if (event.window.event == SDL_WINDOWEVENT_RESIZED)
		{
			int centerX = (int)m_GameWindow->GetWidth2();
			int centerY = (int)m_GameWindow->GetHeight2();
			m_TitleRect = {centerX - 300, centerY / 2 - 150, 600, 200};

			int buttonH = 80; // slightly taller
			int gap = 24;
			int exitW = 360;
			int playW = exitW;
			int trainW = exitW;
			int startY = centerY - 60;
			int rowX = centerX - (playW + gap + trainW) / 2;
			m_PlayButtonRect = {rowX, startY, playW, buttonH};
			m_TrainingButtonRect = {rowX + playW + gap, startY, trainW, buttonH};

			m_ExitButtonRect = {centerX - exitW / 2, startY + buttonH + 32, exitW, buttonH};
			m_GameWindow->Render()->Clear();
			m_GameWindow->Render()->UpdateWindow();
			break;
		}
	}
}

void MainMenu::Tick()
{
	if (m_Intro)
	{
		if (std::chrono::duration_cast<
				std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_Opened)
				.count() >= 15500)
			m_Intro = false;

		return;
	}

	for (int i = 0; i < 3; i++)
	{
		auto random_position = Vec2d(rand() % m_GameWindow->GetWidth(), rand() % m_GameWindow->GetHeight());
		auto duration = 1500.0;
		m_Stars.emplace_back(random_position, Vec2d(0.0, 0.0), duration);
	}

	Vec2i MousePosition;
	SDL_GetMouseState(&MousePosition.x, &MousePosition.y);
	auto MousePositionD = Vec2d(MousePosition.x, MousePosition.y);

	for (int i = m_Stars.size() - 1; i >= 0; --i)
	{
		auto &[position, velocity, duration] = m_Stars[i];

		auto direction = position - MousePositionD;
		double distance = direction.Length();
		velocity += direction.Normalize() / distance * 0.15;
		velocity.x += (sin((position.x + position.y * 2) / 50)) * 0.0015;

		velocity *= 0.98;

		position.x += velocity.x;
		position.y += velocity.y;
		if (position.x > m_GameWindow->GetWidth())
			position.x -= m_GameWindow->GetWidth();
		if (position.y > m_GameWindow->GetHeight())
			position.y -= m_GameWindow->GetHeight();

		duration -= 1;
		if (duration <= 0.0)
			m_Stars.erase(m_Stars.begin() + i);
	}
}

void MainMenu::Render()
{
	Drawing *render = m_GameWindow->Render();
	SDL_Renderer *renderer = m_GameWindow->Renderer();

	//    SDL_RenderClear(renderer);
	//    render->RenderTextureFullscreen(m_MenuTexture->SDLTexture(), nullptr);
	//    render->RenderTexture(m_TexturePlay->SDLTexture(), nullptr, m_PlayButtonRect);
	//    render->RenderTexture(m_TextureExit->SDLTexture(), nullptr, m_ExitButtonRect);
	//    render->UpdateWindow();

	if (m_Intro)
	{
		render->SetColor(0, 0, 0, 255);
		render->Clear();

		int duration = (int)std::chrono::duration_cast<
						   std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_Opened)
						   .count();
		int centerX = (int)m_GameWindow->GetWidth2();
		int centerY = (int)m_GameWindow->GetHeight2();
		int radius = (int)((double)duration * 0.05);
		int color = (int)((double)duration / 15500 * 255);

		render->SetColor(color, color, color, 255);
		for (int y = -radius; y <= radius; ++y)
		{
			int dx = static_cast<int>(std::sqrt(radius * radius - y * y)); // Horizontal distance for this vertical offset
			int startX = centerX - dx;
			int endX = centerX + dx;

			// Draw a horizontal line for the current row
			SDL_RenderDrawLine(renderer, startX, centerY + y, endX, centerY + y);
		}

		if (duration >= 15000)
		{
			render->SetColor(0, 0, 0, 255);
			render->Clear();
		}
	}
	else
	{
		render->SetColor(0, 0, 50, 255);
		render->Clear();

		render->SetColor(200, 200, 200, 255);
		for (int i = m_Stars.size() - 1; i >= 0; --i)
		{
			auto &[position, velocity, duration] = m_Stars[i];

			auto size = (int)duration / 750.0;
			for (int j = 0; j < size; j++)
			{
				for (int k = 0; k < size; k++)
				{
					int draw_x = (int)(position.x - size / 2 + j);
					int draw_y = (int)(position.y - size / 2 + k);

					SDL_RenderDrawPoint(renderer, draw_x, draw_y);
				}
			}
		}

	render->RenderTexture(sTextureTitle.GetTexture()->SDLTexture(), nullptr, m_TitleRect);
	// Ensure blending
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	render->RenderButton(sTexturePlay.GetTexture()->SDLTexture(), m_PlayButtonRect, m_PlayHover);
	render->RenderButton(sTextureTraining.GetTexture()->SDLTexture(), m_TrainingButtonRect, m_TrainingHover);
	render->RenderButton(sTextureExit.GetTexture()->SDLTexture(), m_ExitButtonRect, m_ExitHover);
	}
}