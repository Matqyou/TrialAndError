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
#include "game/indicators/TextSurface.h"
#include "game/entities/Projectile.h"
#include "game/interface/PauseMenu.h"
#include "game/interface/MainMenu.h"
#include "client/core/TextManager.h"
#include "game/entities/AmmoBox.h"
#include "GameData.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <random>

//GameReference *GameWindow;
LoadedSound sConnectedSound("ui.pitch.mid");
LoadedSound sDisconnectedSound("ui.pitch.low");

bool Initialize()
{
	srand(time(nullptr));

	if (!GameReference.Initialize())
		return false;

	SDL_ShowCursor(0);
	return true;
}

int main()
{
#ifdef NDEBUG
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

	// Enable UTF-8 processing in the console
	SetConsoleOutputCP(CP_UTF8);
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hOut, &mode);
	SetConsoleMode(hOut, mode | ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

	system("");
	if (!Initialize())
	{
		std::printf("Terminating..");
		exit(1);
	}
	Clock *Timer = GameReference.Timer();
	Drawing *Render = GameReference.Render();
	Assets *assets = Assets::Get();

	Texture *Vignette = assets->GetTexture("backgrounds.vignette")
		->SetAlphaMod(200);

	MainMenu mainMenu(&GameReference);
	mainMenu.InitialShow();

	PauseMenu *PauseMenu;
	while (true)
	{
		PauseMenu = GameReference.World()->Menu(); // 😱😱😱
		bool pauseMenuOpen = PauseMenu->Paused();

		// Input and events
		SDL_Event CurrentEvent;
		while (SDL_PollEvent(&CurrentEvent))
		{
			GameReference.Event(CurrentEvent);
			GameReference.World()->Event(CurrentEvent);
			GameReference.Controllers()->Event(CurrentEvent);

			if (pauseMenuOpen)
			{
				PauseMenu->HandleEvent(CurrentEvent);
				break;
			}

			if (GameReference.World()->LvlMenu() != nullptr)
			{
				GameReference.World()->LvlMenu()->HandleEvent(CurrentEvent);
				break;
			}

			switch (CurrentEvent.type)
			{
				case SDL_QUIT:GameReference.Deinitialize(true);
					return 0; // This should happen in every quit scenario, but menus didn't think about that

				case SDL_KEYDOWN:
				{
					SDL_Scancode ScancodeKey = CurrentEvent.key.keysym.scancode;
					if (ScancodeKey == SDL_SCANCODE_ESCAPE)
					{
						PauseMenu->Show();
					}
					else if (ScancodeKey == SDL_SCANCODE_Z)
					{
						new CharacterNPC(GameReference.World(),
										 50.0,
										 Vec2d(32 * 30, 32),
										 Vec2d(0, 10),
										 NPC_TURRET,
										 true);
					}
				}
					break;
			}
		}

		// Ticking
		GameReference.World()->Tick();
		GameReference.Controllers()->TickReset();

		// Drawing
		GameReference.World()->Draw();
		GameReference.GetInterface()->DrawBackground();
		Render->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

		// Render the pause menu if open
		if (pauseMenuOpen)
		{
			PauseMenu->Render();
		}

		// Render one of the levelupmenus in queue if any
		if (GameReference.World()->LvlMenu())
			GameReference.World()->LvlMenu()->Render();
		else
			GameReference.World()->CheckLevelUps();

		GameReference.GetInterface()->DrawForeground();
		Render->UpdateWindow();

		if (GameReference.World()->GetDelay() && (GameReference.World()->LvlMenu() != nullptr))
		{
			////            SDL_Delay(1000); // Delay for 1000 milliseconds (1 second)
			//            SDL_Event event;
			//            while (SDL_PollEvent(&event)) {
			//                // Discard events
			//            }
			//
			GameReference.World()->SetDelay(false); // Reset the delay flag after the delay
		}

		Timer->Tick();
	}

	return 0;
}
