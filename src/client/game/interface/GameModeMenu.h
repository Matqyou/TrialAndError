// src/game/interface/GameSelectMenu.h

#pragma once

#include <client/GameData.h>
#include "SDL.h"
#include <chrono>

class GameModeMenu
{
private:
	GameData *m_GameWindow;
	static LoadedTexture sMenuTexture;
	static LoadedTexture sTextureTitle;
	static LoadedTexture sTextureClassButton1;
	static LoadedTexture sTextureClassButton2;
	static LoadedTexture sTextureClassButton3;
	static LoadedTexture sTextureClassButton4;
	static LoadedTexture sTextureBack;
	SDL_Rect m_TitleRect;
	SDL_Rect m_ClassButtonRect1;
	SDL_Rect m_ClassButtonRect2;
	SDL_Rect m_ClassButtonRect3;
	SDL_Rect m_ClassButtonRect4;
	bool m_ClassHover1{false};
	bool m_ClassHover2{false};
	bool m_ClassHover3{false};
	bool m_ClassHover4{false};
	SDL_Rect m_BackButtonRect;
	bool m_BackHover{false};

	std::vector<std::tuple<Vec2d, Vec2d, double>> m_Stars;

public:
	GameModeMenu(GameData *game_window);
	~GameModeMenu();

	void Show();
	void HandleEvent(const SDL_Event& event, bool& running, bool& menuOpen);
	void Tick();
	void Render();
};