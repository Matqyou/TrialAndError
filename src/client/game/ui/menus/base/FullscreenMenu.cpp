//
// Created by Matq on 12/01/2025.
//

#include "FullscreenMenu.h"
#include "client/core/Application.h"
#include "client/game/GameReference.h"
#include "client/game/ui/menus/Menus.h"

FullscreenMenu::FullscreenMenu()
	: Element()
{
	this->name = L"FullscreenMenu";

	this->SetDraw(DRAW_RECT);
}

void FullscreenMenu::FullscreenMenuEvent(const SDL_Event& sdl_event, EventContext& event_summary)
{
	// When multiple menus are stacked, the oldest one will prevent events from passing onto lower menus
	event_summary.rapid_context.event_captured = true;

	switch (sdl_event.type)
	{
		case SDL_EVENT_WINDOW_RESIZED:
		{
			this->RefreshMenu();
			break;
		}
	}
}

void FullscreenMenu::HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary)
{
	HandleEventChildren(sdl_event, event_summary);
	FullscreenMenuEvent(sdl_event, event_summary);
}

void FullscreenMenu::RefreshMenu()
{
	this->UpdateElement(Vec2i(0, 0), Application.GetResolution()); //
	this->UpdateElement(Vec2i(0, 0), Application.GetResolution()); // temporary fix for adaptive elements :/
}

void FullscreenMenu::SwitchToThisMenu()
{
	Menus.SetCurrentMenu(this);
	RefreshMenu();
}
