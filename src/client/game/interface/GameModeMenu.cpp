// src/game/interface/GameModeMenu.cpp

#include "GameModeMenu.h"
#include "../classes/PlayerClass.h"
#include "ClassSelectMenu.h"

LoadedTexture GameModeMenu::sMenuTexture("interface.menu");
LoadedTexture GameModeMenu::sTextureTitle("ui.gamemodemenu.title");
LoadedTexture GameModeMenu::sTextureClassButton1("ui.gamemodemenu.solobutton");
LoadedTexture GameModeMenu::sTextureClassButton2("ui.gamemodemenu.pvpbutton");
LoadedTexture GameModeMenu::sTextureClassButton3("ui.gamemodemenu.sandboxbutton");
LoadedTexture GameModeMenu::sTextureClassButton4("ui.gamemodemenu.co-opbutton");
LoadedTexture GameModeMenu::sTextureBack("ui.gamemodemenu.backbutton");

GameModeMenu::GameModeMenu(GameData *game_window)
	: m_GameWindow(game_window)
{
	int screenW = m_GameWindow->GetWidth();
	int screenH = m_GameWindow->GetHeight();

	int centerX = screenW / 2;
	int centerY = screenH / 2;

	int buttonWidth = screenW / 4;
	int buttonHeight = screenH / 10;
	int hSpacing = buttonWidth / 6;
	int vSpacing = buttonHeight;

	int totalWidth = buttonWidth * 2 + hSpacing;
	int totalHeight = buttonHeight * 2 + vSpacing;
	int originX = centerX - totalWidth / 2;
	int originY = centerY - totalHeight / 2;

	m_ClassButtonRect1 = { originX, originY, buttonWidth, buttonHeight };
	m_ClassButtonRect2 = { originX + buttonWidth + hSpacing, originY, buttonWidth, buttonHeight };
	m_ClassButtonRect3 = { originX, originY + buttonHeight + vSpacing, buttonWidth, buttonHeight };
	m_ClassButtonRect4 = { originX + buttonWidth + hSpacing, originY + buttonHeight + vSpacing, buttonWidth,
						   buttonHeight };

	m_BackButtonRect = {
		centerX - buttonHeight,
		originY + 3 * (buttonHeight) + vSpacing,
		buttonHeight * 2,
		buttonHeight };

	m_TitleRect = {
		centerX - (buttonWidth * 2 / 3),
		originY - 3 * buttonHeight,
		buttonWidth * 4 / 3,
		buttonHeight * 2 };
}

GameModeMenu::~GameModeMenu()
{
}

void GameModeMenu::Show()
{
	bool running = true;
	bool menuOpen = true;

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

void GameModeMenu::HandleEvent(const SDL_Event& event, bool& running, bool& menuOpen)
{
	switch (event.type)
	{
		case SDL_QUIT:m_GameWindow->Deinitialize(true);
			while (Mix_Playing(-1))
			{
			}
			delete m_GameWindow;
			exit(0);

		case SDL_MOUSEBUTTONDOWN:SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));

			if (event.button.button == SDL_BUTTON_LEFT)
			{
				int x = event.button.x;
				int y = event.button.y;

				if (x >= m_ClassButtonRect1.x && x < m_ClassButtonRect1.x + m_ClassButtonRect1.w &&
					y >= m_ClassButtonRect1.y && y < m_ClassButtonRect1.y + m_ClassButtonRect1.h)
				{
					menuOpen = false;
					Assets::PauseMusic();
					Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
					m_GameWindow->GetClassMenus().front()->Show(GameMode::Solo);
				}
				else if (x >= m_ClassButtonRect2.x && x < m_ClassButtonRect2.x + m_ClassButtonRect2.w &&
					y >= m_ClassButtonRect2.y && y < m_ClassButtonRect2.y + m_ClassButtonRect2.h)
				{
					menuOpen = false;
					Assets::PauseMusic();
					Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
					m_GameWindow->GetClassMenus().front()->Show(GameMode::Sandbox); 	
					m_GameWindow->RemovePlayerClassMenu();
					// PVP mode
				}
				else if (x >= m_ClassButtonRect3.x && x < m_ClassButtonRect3.x + m_ClassButtonRect3.w &&
					y >= m_ClassButtonRect3.y && y < m_ClassButtonRect3.y + m_ClassButtonRect3.h)
				{
					menuOpen = false;
					Assets::PauseMusic();
					Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
					m_GameWindow->GetClassMenus().front()->Show(GameMode::Sandbox);
					m_GameWindow->RemovePlayerClassMenu();
				}
				else if (x >= m_ClassButtonRect4.x && x < m_ClassButtonRect4.x + m_ClassButtonRect4.w &&
					y >= m_ClassButtonRect4.y && y < m_ClassButtonRect4.y + m_ClassButtonRect4.h)
				{
					menuOpen = false;
					Assets::PauseMusic();
					Assets::Get()->GetSound("ui.pitch.low")->PlaySound();

					for (ClassSelectMenu *menu : m_GameWindow->GetClassMenus())
					{
						menu->Show(GameMode::Coop);
					}
				}
				else if (x >= m_BackButtonRect.x && x < m_BackButtonRect.x + m_BackButtonRect.w &&
					y >= m_BackButtonRect.y && y < m_BackButtonRect.y + m_BackButtonRect.h)
				{
					menuOpen = false;
					m_GameWindow->Menu()->Show();
				}
			}
			break;

		case SDL_MOUSEMOTION:
		{
			int x = event.motion.x;
			int y = event.motion.y;
			bool hovering = false;

			hovering |= (x >= m_ClassButtonRect1.x && x < m_ClassButtonRect1.x + m_ClassButtonRect1.w &&
				y >= m_ClassButtonRect1.y && y < m_ClassButtonRect1.y + m_ClassButtonRect1.h);
			hovering |= (x >= m_ClassButtonRect2.x && x < m_ClassButtonRect2.x + m_ClassButtonRect2.w &&
				y >= m_ClassButtonRect2.y && y < m_ClassButtonRect2.y + m_ClassButtonRect2.h);
			hovering |= (x >= m_ClassButtonRect3.x && x < m_ClassButtonRect3.x + m_ClassButtonRect3.w &&
				y >= m_ClassButtonRect3.y && y < m_ClassButtonRect3.y + m_ClassButtonRect3.h);
			hovering |= (x >= m_ClassButtonRect4.x && x < m_ClassButtonRect4.x + m_ClassButtonRect4.w &&
				y >= m_ClassButtonRect4.y && y < m_ClassButtonRect4.y + m_ClassButtonRect4.h);
			hovering |= (x >= m_BackButtonRect.x && x < m_BackButtonRect.x + m_BackButtonRect.w &&
				y >= m_BackButtonRect.y && y < m_BackButtonRect.y + m_BackButtonRect.h);

		m_ClassHover1 = (x >= m_ClassButtonRect1.x && x < m_ClassButtonRect1.x + m_ClassButtonRect1.w &&
			       y >= m_ClassButtonRect1.y && y < m_ClassButtonRect1.y + m_ClassButtonRect1.h);
		m_ClassHover2 = (x >= m_ClassButtonRect2.x && x < m_ClassButtonRect2.x + m_ClassButtonRect2.w &&
			       y >= m_ClassButtonRect2.y && y < m_ClassButtonRect2.y + m_ClassButtonRect2.h);
		m_ClassHover3 = (x >= m_ClassButtonRect3.x && x < m_ClassButtonRect3.x + m_ClassButtonRect3.w &&
			       y >= m_ClassButtonRect3.y && y < m_ClassButtonRect3.y + m_ClassButtonRect3.h);
		m_ClassHover4 = (x >= m_ClassButtonRect4.x && x < m_ClassButtonRect4.x + m_ClassButtonRect4.w &&
			       y >= m_ClassButtonRect4.y && y < m_ClassButtonRect4.y + m_ClassButtonRect4.h);
		m_BackHover = (x >= m_BackButtonRect.x && x < m_BackButtonRect.x + m_BackButtonRect.w &&
			     y >= m_BackButtonRect.y && y < m_BackButtonRect.y + m_BackButtonRect.h);

		bool hoveringAny = m_ClassHover1 || m_ClassHover2 || m_ClassHover3 || m_ClassHover4 || m_BackHover;
		SDL_SetCursor(SDL_CreateSystemCursor(hoveringAny ? SDL_SYSTEM_CURSOR_HAND : SDL_SYSTEM_CURSOR_ARROW));
		}
			break;
		case SDL_CONTROLLERDEVICEADDED:
		{
			int DeviceID = event.cdevice.which;
			GameController *CurrentController = m_GameWindow->Controllers()->OpenController(DeviceID);
			m_GameWindow->AddPlayerClassMenu();
		}
			break;

		case SDL_CONTROLLERDEVICEREMOVED:
		{
			int DeviceID = event.cdevice.which;
			if (m_GameWindow->Controllers()->GetConnectedControllers().empty())
			{
				break;
			}
			GameController *DeletedController = m_GameWindow->Controllers()->CloseController(DeviceID);
			m_GameWindow->RemovePlayerClassMenu();
		}
			break;

		case SDL_WINDOWEVENT:
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				int screenW = m_GameWindow->GetWidth();
				int screenH = m_GameWindow->GetHeight();

				int centerX = screenW / 2;
				int centerY = screenH / 2;

				int buttonWidth = screenW / 4;
				int buttonHeight = screenH / 10;
				int hSpacing = buttonWidth / 6;
				int vSpacing = buttonHeight;

				int totalWidth = buttonWidth * 2 + hSpacing;
				int totalHeight = buttonHeight * 2 + vSpacing;
				int originX = centerX - totalWidth / 2;
				int originY = centerY - totalHeight / 2;

				m_ClassButtonRect1 = { originX, originY, buttonWidth, buttonHeight };
				m_ClassButtonRect2 = { originX + buttonWidth + hSpacing, originY, buttonWidth, buttonHeight };
				m_ClassButtonRect3 = { originX, originY + buttonHeight + vSpacing, buttonWidth, buttonHeight };
				m_ClassButtonRect4 = { originX + buttonWidth + hSpacing, originY + buttonHeight + vSpacing, buttonWidth,
									   buttonHeight };

				m_BackButtonRect = {
					centerX - buttonHeight,
					originY + 3 * (buttonHeight) + vSpacing,
					buttonHeight * 2,
					buttonHeight };

				m_TitleRect = {
					centerX - (buttonWidth * 2 / 3),
					originY - 3 * buttonHeight,
					buttonWidth * 4 / 3,
					buttonHeight * 2 };

				m_GameWindow->Render()->Clear();
				m_GameWindow->Render()->UpdateWindow();
			}
			break;
	}
}

void GameModeMenu::Tick()
{
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
		auto& [position, velocity, duration] = m_Stars[i];

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

void GameModeMenu::Render()
{
	Drawing *render = m_GameWindow->Render();
	SDL_Renderer *renderer = m_GameWindow->Renderer();

	render->SetColor(0, 0, 50, 255);
	render->Clear();

	render->SetColor(200, 200, 200, 255);
	for (int i = m_Stars.size() - 1; i >= 0; --i)
	{
		auto& [position, velocity, duration] = m_Stars[i];

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

	render->RenderButton(sTextureClassButton1.GetTexture()->SDLTexture(), m_ClassButtonRect1, m_ClassHover1);
	render->RenderButton(sTextureClassButton2.GetTexture()->SDLTexture(), m_ClassButtonRect2, m_ClassHover2);
	render->RenderButton(sTextureClassButton3.GetTexture()->SDLTexture(), m_ClassButtonRect3, m_ClassHover3);
	render->RenderButton(sTextureClassButton4.GetTexture()->SDLTexture(), m_ClassButtonRect4, m_ClassHover4);
	render->RenderButton(sTextureBack.GetTexture()->SDLTexture(), m_BackButtonRect, m_BackHover);
}