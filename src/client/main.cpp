/*
 *  Trial And Error
 *  This is a game.
 *
 *  Made by two and a half guys.
 *  Project started 16.03.23
 */

#define SDL_MAIN_HANDLED

#ifdef _WIN32
#include <windows.h>
#undef interface
#undef PlaySound
#undef ERROR
#endif

#include "game/entities/characters/CharacterNPC.h"
#include "game/entities/Projectile.h"
#include "client/core/TextManager.h"
#include "client/game/GameReference.h"
#include "client/Protocol.h"
#include <SDL3/SDL_main.h>
#include <iostream>
#include <cstdlib>
#include <vector>

#include <random>

LinkSound sConnectedSound("ui.pitch.mid");
LinkSound sDisconnectedSound("ui.pitch.low");

void SetConsoleToUTF8()
{
	SetConsoleOutputCP(CP_UTF8);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hOut, &mode);
	SetConsoleMode(hOut, mode | ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);
}

void exit_application()
{
	GameReference.Deinitialize(true);
	Application.Destroy();
	dbg_msg("Graceful exit\n");
	exit(0);
}

int main()
{
#ifdef NDEBUG
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

	system("");
	SetConsoleToUTF8();

	srand(time(nullptr));
	GameReference.SetExitApplicationCallback([]() { exit_application(); });
	Application.Initialize(APPLICATION_NAME,
						   APPLICATION_VERSION,
						   "com.tae.trial_and_error",
						   Vec2i(1280, 720),
						   60.0, 10.0);
	GameReference.Initialize();

	auto clock = Application.GetClock();
	auto drawing = Application.GetDrawing();

	long long total_frame_duration = 1;
	SDL_Event sdl_event;
	EventContext event_context;
	do // Loading loop
	{
		// Events
		while (SDL_PollEvent(&sdl_event))
		{
			Application.Event(sdl_event);
			switch (sdl_event.type)
			{
				case SDL_EVENT_QUIT:
				{
					exit_application();
					break;
				}
			}
		}

		// Ticking
		Assets.ThreadedLoading();

		// Drawing
		if (clock->TimePassed())
		{
			drawing->SetRenderTarget(nullptr);
			drawing->SetColor(0, 0, 0, 255);
			drawing->Clear();

			// Draw progress bar
			SDL_FRect progress_rect = {
				((float)Application.GetWidth() - 800.0f) / 2.0f,
				((float)Application.GetHeight() - 50.0f) / 2.0f,
				800.0f * ((float)AssetsClass::GetWorkDone() / (float)AssetsClass::GetTotalWork()),
				50.0f
			};
			drawing->SetColor(104, 195, 235, 255);
			drawing->DrawRect(progress_rect, true);

			progress_rect.w = 800.0f;
			progress_rect.h = 50.0f;
			drawing->DrawRect(progress_rect, false);

			drawing->UpdateWindow();
		}
	} while (Assets.IsLoading());

	GameReference.InitUI();

	Texture *Vignette = Assets.GetTexture("backgrounds.vignette")
		->SetAlphaMod(200);

	GameReference.AddPlayerClassMenu();
	GameReference.Menu()->SwitchToThisMenu();
//	MainMenu mainMenu;
//	mainMenu.Show();

//	PauseMenu *PauseMenu;
	bool render_debug = false;
	while (true)
	{
		// This part of the loop runs with no delay
		event_context.ResetContext(); // Per tick context

		// Input and events
		while (SDL_PollEvent(&sdl_event))
		{
			event_context.ResetRapidContext(); // Per event context
			Application.Event(sdl_event);

			auto current_menu = GameReference.GetCurrentMenu();
			if (current_menu)
				current_menu->HandleEvent(sdl_event, event_context);
			if (GameReference.World())
				GameReference.World()->HandleEvent(sdl_event);

//			if (pauseMenuOpen)
//			{
//				PauseMenu->Event(sdl_event);
//				break;
//			}
//
//			if (GameReference.World()->LvlMenu() != nullptr)
//			{
//				GameReference.World()->LvlMenu()->Event(sdl_event);
//				break;
//			}

			switch (sdl_event.type)
			{
				case SDL_EVENT_QUIT:
				{
					exit_application();
					break;
				}
				case SDL_EVENT_MOUSE_MOTION:
				{
					event_context.had_mouse_motion = true;
					break;
				}
				case SDL_EVENT_WINDOW_MOUSE_LEAVE:
				{
					auto window_flags = SDL_GetWindowFlags(Application.GetWindow());
					if ((window_flags & SDL_WINDOW_INPUT_FOCUS) == 0)
						clock->SetIdleFramerate();

					break;
				}
				case SDL_EVENT_WINDOW_MOUSE_ENTER:
				{
					auto window_flags = SDL_GetWindowFlags(Application.GetWindow());
					if ((window_flags & SDL_WINDOW_INPUT_FOCUS) == 0)
						clock->ResetFramerate();

					break;
				}
				case SDL_EVENT_WINDOW_FOCUS_LOST:
				{
					clock->SetIdleFramerate();
					break;
				}
				case SDL_EVENT_WINDOW_FOCUS_GAINED:
				{
					clock->ResetFramerate();
					break;
				}
				case SDL_EVENT_KEY_DOWN:
				{
					SDL_Scancode scancode = sdl_event.key.scancode;
//					if (scancode == SDL_SCANCODE_ESCAPE)
//						GameReference.GetPauseMenu()->SwitchToThisMenu();
					if (sdl_event.key.scancode == SDL_SCANCODE_F2)
						render_debug = !render_debug;
					else if (sdl_event.key.scancode == SDL_SCANCODE_F3)
						GameReference.GetCurrentMenu()->DebugPrint();
//					else if (scancode == SDL_SCANCODE_Z)
//						new CharacterNPC(GameReference.World(), 50.0,
//										 Vec2d(32 * 30, 32), Vec2d(0, 10),
//										 NPC_TURRET, true);
					break;
				}
			}
			Element::DestroyElements();
		}
		if (event_context.had_mouse_motion && event_context.cursor_changed == CursorChange::NO_CHANGE)
			SDL_SetCursor(SDL_GetDefaultCursor());

		// This part of the loop runs at 60fps
		if (clock->TimePassed())
		{
			auto elapsed_seconds = (double)(total_frame_duration) / 1e9;

			// Ticking
			if (GameReference.World())
				GameReference.World()->Tick(elapsed_seconds);
			if (GameReference.GetCurrentMenu())
				GameReference.GetCurrentMenu()->Tick(elapsed_seconds);

			// Drawing
			if (GameReference.World())
				GameReference.World()->Draw();
			GameReference.GetInterface()->DrawBackground();
			drawing->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

			auto current_menu = GameReference.GetCurrentMenu();
			if (current_menu)
			{
				current_menu->Render();
				if (render_debug)
					current_menu->RenderDebug();
			}

			// Render one of the levelupmenus in queue if any
//			if (GameReference.World()->LvlMenu())
//				GameReference.World()->LvlMenu()->Render();
//			else
//				GameReference.World()->CheckLevelUps();

			GameReference.GetInterface()->DrawForeground();
			drawing->UpdateWindow();

//			if (GameReference.World()->GetDelay() && (GameReference.World()->LvlMenu() != nullptr))
//				GameReference.World()->SetDelay(false); // Reset the delay flag after the delay
		}

		total_frame_duration = clock->GetTimeElapsedNanoNow();
	}

	exit_application();
	return 0;
}
