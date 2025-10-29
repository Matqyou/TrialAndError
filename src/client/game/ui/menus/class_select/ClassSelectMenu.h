// src/game/interface/ClassSelectmenu.h

#pragma once

#include "client/game/GameReference.h"
#include "client/game/ui/menus/base/FullscreenMenu.h"
#include "SDL3/SDL.h"
#include <chrono>

class ClassSelectMenu : public FullscreenMenu
{
private:
	Gamemode gamemode;

//	SDL_FRect m_TitleRect;
//	SDL_FRect m_ClassButtonRect1;
//	SDL_FRect m_ClassButtonRect2;
//	SDL_FRect m_ClassButtonRect3;
//	SDL_FRect m_ClassButtonRect4;
//	SDL_FRect m_BackButtonRect;

//	std::vector<std::tuple<Vec2d, Vec2d, double>> m_Stars;

public:
	ClassSelectMenu();
	~ClassSelectMenu();

	// Manipulating
	void SetGamemode(Gamemode new_gamemode) { gamemode = new_gamemode; }
//	void Show() override;

	// Ticking
//	void Event(const SDL_Event& sdl_event) override;
//	void Tick() override;
//	void Render() override;

};