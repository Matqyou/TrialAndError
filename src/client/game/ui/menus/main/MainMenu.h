// src/game/interface/MainMenu.h

#pragma once

#include "client/game/ui/menus/base/FullscreenMenu.h"
#include "shared/math/Vec2.h"
#include "SDL3/SDL.h"
#include <chrono>

class MainMenu : public FullscreenMenu
{
private:
	std::chrono::steady_clock::time_point m_Opened;
	bool m_Intro;

public:
	MainMenu();
	~MainMenu() override;

	// Manipulating
	void SwitchToThisMenu() override;

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary) override;
	void Tick(double elapsed_seconds) override;
	void Render() override;

};