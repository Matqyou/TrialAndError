// src/game/interface/PauseMenu.h

#pragma once

#include "client/game/ui/menus/base/FullscreenMenu.h"
#include "client/core/texture/Texture.h"
#include "shared/utility/Functions.h"
#include "SDL3/SDL.h"

class PauseMenu : public FullscreenMenu
{
public:
	PauseMenu();
	~PauseMenu();

	// Manipulating
	void SwitchToThisMenu() override;

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary) override;
	void Tick(double elapsed_seconds) override;
	void Render() override;

};