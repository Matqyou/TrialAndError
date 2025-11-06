//
// Created by Matq on 06/07/2025.
//

#include "Menus.h"

#include "client/game/ui/menus/class_select/ClassSelectMenu.h"
#include "client/game/ui/menus/class_select/GamemodeMenu.h"
#include "client/game/ui/menus/levelup/LevelupMenu.h"
#include "client/game/ui/menus/pause/PauseMenu.h"
#include "client/game/ui/menus/main/MainMenu.h"
#include "client/game/ui/CommonUI.h"

MenusClass::MenusClass()
{
	main_menu = nullptr;
	pause_menu = nullptr;
	levelup_menu = nullptr;
	gamemode_menu = nullptr;
	current_menu = nullptr;
	classselect_menu = nullptr;

	drag_from = Vec2i(0, 0);
	dragging = false;
	render_debug = false;
}

void MenusClass::InitMenus()
{
	main_menu = new MainMenu();
	pause_menu = new PauseMenu();
	levelup_menu = new LevelupMenu();
	gamemode_menu = new GamemodeMenu();
	classselect_menu = new ClassSelectMenu();
}

MenusClass::~MenusClass()
{
	delete main_menu;
	delete pause_menu;
	delete levelup_menu;
	delete gamemode_menu;
	delete classselect_menu;
}

void MenusClass::SetCurrentMenu(FullscreenMenu *new_menu)
{
	current_menu = new_menu;
}

void MenusClass::HandleEvent(const SDL_Event& sdl_event, EventContext& event_context)
{
	switch (sdl_event.type)
	{
		case SDL_EVENT_KEY_DOWN:
		{
			SDL_Scancode scancode = sdl_event.key.scancode;
			if (scancode == SDL_SCANCODE_F2)
				render_debug = !render_debug;
			else if (scancode == SDL_SCANCODE_F3)
				Menus.GetCurrentMenu()->DebugPrint();

			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
		{
			if (sdl_event.button.button == SDL_BUTTON_RIGHT)
			{
				float mouse_x, mouse_y;
				SDL_GetMouseState(&mouse_x, &mouse_y);
				drag_from = { (int)mouse_x, (int)mouse_y };
				dragging = true;
			}

			break;
		}
		case SDL_EVENT_MOUSE_BUTTON_UP:
		{
			if (sdl_event.button.button == SDL_BUTTON_RIGHT)
				dragging = false;

			break;
		}
	}

	if (current_menu)
		current_menu->HandleEvent(sdl_event, event_context);
}

void MenusClass::Tick(double elapsed_seconds)
{
	if (current_menu)
		current_menu->Tick(elapsed_seconds);
}

void MenusClass::Render()
{
	if (current_menu)
	{
		current_menu->Render();
//		if (render_debug)
//		{
//			auto drawing = Application.GetDrawing();
//			Menus.GetCurrentMenu()->RenderDebug();
//
//			float mouse_x, mouse_y;
//			SDL_GetMouseState(&mouse_x, &mouse_y);
//
//			if (dragging)
//			{
//				int debug_width = (int)mouse_x - drag_from.x;
//				int debug_height = (int)mouse_y - drag_from.y;
//				SDL_FRect debug_rect = {
//					(float)drag_from.x,
//					(float)drag_from.y,
//					(float)debug_width,
//					(float)debug_height,
//				};
//				drawing->SetColor(255, 0, 255, 100);
//				drawing->DrawRect(debug_rect, true);
//
//				drawing->SetColor(255, 0, 255, 255);
//				drawing->DrawLine(Vec2f((float)drag_from.x, 0),
//								  Vec2f((float)drag_from.x, (float)Application.GetHeight()));
//				drawing->DrawLine(Vec2f(0, (float)drag_from.y),
//								  Vec2f((float)Application.GetWidth(), (float)drag_from.y));
//
//				auto debug_size = Strings::FString("%i, %i", abs(debug_width), abs(debug_height));
//				Texture *debug_render = Assets.RenderTextBlended(CommonUI::fontBig, debug_size, { 255, 255, 255, 255 });
//				Vec2f debug_render_size = debug_render->GetSize();
//				SDL_FRect text_rect = {
//					(float)drag_from.x + ((float)debug_width - debug_render_size.x) / 2.0f,
//					(float)drag_from.y + ((float)debug_height - debug_render_size.y) / 2.0f,
//					debug_render_size.x,
//					debug_render_size.y,
//				};
//				drawing->RenderTexture(debug_render->SDLTexture(), nullptr, text_rect);
//				delete debug_render;
//			}
//
//			drawing->SetColor(255, 0, 255, 255);
//			drawing->DrawLine(Vec2f(mouse_x, 0), Vec2f(mouse_x, (float)Application.GetHeight()));;
//			drawing->DrawLine(Vec2f(0, mouse_y), Vec2f((float)Application.GetWidth(), mouse_y));
//
//			auto coordinates = Strings::FString("%i, %i", (int)mouse_x, (int)mouse_y);
//			Texture *debug_render = Assets.RenderTextBlended(CommonUI::fontBig, coordinates, { 255, 255, 255, 255 });
//			Vec2f debug_render_size = debug_render->GetSize();
//			SDL_FRect text_rect = {
//				mouse_x - debug_render_size.x,
//				mouse_y - debug_render_size.y,
//				debug_render_size.x,
//				debug_render_size.y,
//			};
//			drawing->RenderTexture(debug_render->SDLTexture(), nullptr, text_rect);
//			delete debug_render;
//		}
	}
}

MenusClass Menus;