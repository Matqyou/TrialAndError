// src/game/interface/PauseMenu.h

#pragma once

#include "client/game/ui/menus/FullscreenMenu.h"
#include "client/core/texture/Texture.h"
#include "shared/utility/Functions.h"
#include "SDL3/SDL.h"

class PauseMenu : public FullscreenMenu
{
private:
	bool m_Paused;

public:
	PauseMenu();
	~PauseMenu();

	// Sense
	[[nodiscard]] bool Paused() const { return m_Paused; }

	// Manipulating
//	void Show() override;

	// Ticking
//	void Event(const SDL_Event& sdl_event) override;
//	void Render() override;

};