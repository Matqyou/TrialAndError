//
// Created by Matq on 06/07/2025.
//

#pragma once

#include "client/game/ui/EventContext.h"
#include "SDL3/SDL_events.h"
#include "shared/math/Vec2.h"

class MainMenu;
class PauseMenu;
class LevelupMenu;
class GamemodeMenu;
class ClassSelectMenu;
class FullscreenMenu;
class MenusClass
{
private:
	FullscreenMenu *current_menu;

public:
	MainMenu *main_menu;
	PauseMenu *pause_menu;
	LevelupMenu *levelup_menu;
	GamemodeMenu *gamemode_menu;
	ClassSelectMenu *classselect_menu;

	Vec2i drag_from;
	bool dragging;
	bool render_debug;

public:
	MenusClass();
	void InitMenus();
	~MenusClass();

	// Sense
	[[nodiscard]] FullscreenMenu *GetCurrentMenu() const { return current_menu; };

	// Manipulating
	void SetCurrentMenu(FullscreenMenu *new_menu);

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_context);
	void Tick(double elapsed_seconds);
	void Render();

};

extern MenusClass Menus;
