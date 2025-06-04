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

#include "GameReference.h"
#include "game/GameWorld.h"
#include "technical stuff/GameControllers.h"
#include "game/indicators/TextSurface.h"
#include "game/entities/characters/CharacterNPC.h"
#include "game/entities/characters/character/Character.h"
#include "game/entities/Projectile.h"
#include "game/entities/item/weapons/EntityGuns.h"
#include "game/entities/AmmoBox.h"
#include "game/entities/Crate.h"
#include "game/entities/Error.h"
#include "game/interface/MainMenu.h"
#include "game/interface/PauseMenu.h"
#include "technical stuff/TextManager.h"
#include <vector>
#include <iostream>
#include <random>
#include <cstdlib>

GameReference *GameWindow;
LoadedSound sConnectedSound("ui.pitch.mid");
LoadedSound sDisconnectedSound("ui.pitch.low");

bool Initialize()
{
    srand(time(nullptr));

    GameWindow = new GameReference();
    if (!GameWindow->Initialize())
        return false;

    SDL_ShowCursor(0);
    return true;
}

int main()
{
#ifdef _WIN32
#ifdef NDEBUG
    FreeConsole();
#endif
#endif

    system("");
    if (!Initialize())
    {
        std::printf("Terminating..");
        exit(1);
    }

    // GameWindow->TestEnvironment();

    Clock *Timer = GameWindow->Timer();
    Drawing *Render = GameWindow->Render();
    Assets *assets = Assets::Get();

    Texture *Vignette = assets->GetTexture("backgrounds.vignette");
    Vignette->SetAlphaMod(200);

    MainMenu mainMenu(GameWindow);
    mainMenu.Show();

    PauseMenu *PauseMenu;

    while (true)
    {
        PauseMenu = GameWindow->World()->Menu();
        bool pauseMenuOpen = PauseMenu->Paused();
            
        // Input and events
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent))
        {
            GameWindow->Event(CurrentEvent);
            GameWindow->World()->Event(CurrentEvent);
            GameWindow->Controllers()->Event(CurrentEvent);

            if (pauseMenuOpen)
            {
                PauseMenu->HandleEvent(CurrentEvent);
                break;
            }

            if (GameWindow->World()->LvlMenu()!= nullptr)
            {
                GameWindow->World()->LvlMenu()->HandleEvent(CurrentEvent);
                break;
            }

            switch (CurrentEvent.type)
            {
            case SDL_QUIT:
                GameWindow->Deinitialize(true);
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
                    new CharacterNPC(GameWindow->World(),
                                     50.0,
                                     Vec2d(32 * 30, 32),
                                     Vec2d(0, 10),
                                     NPC_TURRET,
                                     true);
                }
            }
            break;
            case SDL_CONTROLLERDEVICEADDED:
            {
                int DeviceID = CurrentEvent.cdevice.which;
                GameController *CurrentController = GameWindow->Controllers()->OpenController(DeviceID);
                auto NewPlayer = new Player(GameWindow->World(), "Controller");
                auto NewChar = new Character(GameWindow->World(),
                                             NewPlayer,
                                             100.0,
                                             Vec2d(32 * 17.5, 32 * 17.5),
                                             Vec2d(10, 10),
                                             false);

                NewChar->GiveWeapon(new WeaponGlock(nullptr));
                NewChar->SetGameController(CurrentController);
                sConnectedSound.GetSound()->PlaySound();
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED:
            {
                int InstanceID = CurrentEvent.cdevice.which;
                GameController *DeletedController = GameWindow->Controllers()->CloseController(InstanceID);
                GameWindow->World()->DestroyPlayerByController(DeletedController);
                GameWindow->World()->DestroyCharacterByController(DeletedController);
                sDisconnectedSound.GetSound()->PlaySound();
                break;
            }
            }
        }

        // Ticking
        GameWindow->World()->Tick();
        GameWindow->Controllers()->TickReset();

        // Drawing
        GameWindow->World()->Draw();
        GameWindow->GetInterface()->DrawBackground();
        Render->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

        // Render the pause menu if open
        if (pauseMenuOpen)
        {
            PauseMenu->Render();
        }

        // Render one of the levelupmenus in queue if any
        if (GameWindow->World()->LvlMenu()!= nullptr)
        {
            GameWindow->World()->LvlMenu()->Render();
        }
        else{
            GameWindow->World()->CheckLevelUps();
        }

        GameWindow->GetInterface()->DrawForeground();
        Render->UpdateWindow();

        if (GameWindow->World()->GetDelay() && (GameWindow->World()->LvlMenu()!=nullptr))
        {
            ////            SDL_Delay(1000); // Delay for 1000 milliseconds (1 second)
            //            SDL_Event event;
            //            while (SDL_PollEvent(&event)) {
            //                // Discard events
            //            }
            //
            GameWindow->World()->SetDelay(false); // Reset the delay flag after the delay
        }

        Timer->Tick();
    }

    return 0;
}
