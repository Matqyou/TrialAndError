// src/game/interface/MainMenu.h

#pragma once

#include "SDL.h"
#include "client/GameData.h"
#include "shared/utility/Vec2.h"
#include <chrono>

class MainMenu
{
private:
	GameData *m_GameWindow;
	static LoadedMusic sElevatorMusic;
	static LoadedTexture sMenuTexture;
	static LoadedTexture sTextureTitle;
	static LoadedTexture sTexturePlay;
	static LoadedTexture sTextureExit;
	SDL_Rect m_TitleRect;
	SDL_Rect m_PlayButtonRect;
	SDL_Rect m_ExitButtonRect;

	std::vector<std::tuple<Vec2d, Vec2d, double>> m_Stars;
	std::chrono::steady_clock::time_point m_Opened;
	bool m_Intro;

public:
	MainMenu(GameData *game_window);
	~MainMenu();

	void Show();
	void HandleEvent(const SDL_Event& event, bool& running, bool& menuOpen);
	void Tick();
	void Render();
};