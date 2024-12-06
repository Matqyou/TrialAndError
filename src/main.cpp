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
#include <vector>
#include <iostream>
#include <random>

GameReference *GameWindow;

bool Initialize() {
    srand(time(nullptr));

    GameWindow = new GameReference();
    if (!GameWindow->Initialize())
        return false;

    SDL_ShowCursor(0);

//    Character::ms_Texture = ImageHandler->LoadTexture("assets/images/entities/Fist.png", true);

    // Temp ammo spawn, had to generate random and set the value for each one, also changed it to also sending an int
    //  to the Crate constructor, so its easier to work with and i dont need to have acess to DropTypes, which i do, but no,
    //  it only works cuz this is main and i prefer to use the same typa fix on both ERRORS and crates, since
    //  for ERRORS that fix wouldnt have worked, also it lowers the line count cuz i dont gotta set the ERROR/AMMO types
    //  to each random number, can just send the number straight up
    return true;
}

int main() {
#ifdef _WIN32
#ifdef NDEBUG
    FreeConsole();
#endif
#endif
    if (!Initialize()) {
        std::printf("Terminating..");
        exit(1);
    }

    Clock* Timer = GameWindow->Timer();
    Drawing* Render = GameWindow->Render();
    AssetsManager* AssetsHandler = GameWindow->Assetz();
    Assets* decals = Assets::Get();

    Texture* TextureResume = decals->GetTexture("interface.resume");
    Texture* TexturePlay = decals->GetTexture("interface.playbutton");
    Texture* TextureBack = decals->GetTexture("interface.back");
    Texture* TextureExit = decals->GetTexture("interface.exit");
    Texture* TextureSettings = decals->GetTexture("interface.settings");
    Texture* MenuTexture = decals->GetTexture("interface.menu");
    Texture* Vignette = decals->GetTexture("backgrounds.vignette");
    Vignette->SetAlphaMod(200);

    Sound* LowSound = decals->GetSound("low");
    Sound* HighSound = decals->GetSound("high");
    Sound* QuitSound = decals->GetSound("quit");
    Sound* LowUISound = decals->GetSound("lowui");
    Sound* MidUISound = decals->GetSound("midui");
    Sound* HighUISound = decals->GetSound("highui");

    // Decals::Get()->GetSound("quit")->PlaySound();

    MainMenu mainMenu(GameWindow);
    mainMenu.Show();
    bool Running = true;

    PauseMenu pauseMenu(GameWindow->World(), &mainMenu);
    LevelUpMenu* activeLevelUpMenu = nullptr;
    std::queue<LevelUpMenu*> levelUpMenuQueue;
    bool pauseMenuOpen = false;
    bool levelUpMenuOpen = false;
    bool delayActive = false;
    while (Running) {
        pauseMenuOpen = pauseMenu.Paused();

        if (!levelUpMenuOpen) {
            for (auto player = GameWindow->World()->FirstPlayer(); player != nullptr; player = player->Next()) {
                std::queue<LevelUpMenu*> playerQueue = player->GetLevelUpMenuQueue();
                while (!playerQueue.empty()) {
                    levelUpMenuQueue.push(playerQueue.front());
                    playerQueue.pop();
                    player->SetLevelUpMenuQueue(playerQueue);
                }
                playerQueue = std::queue<LevelUpMenu*>();
            }

            if (!levelUpMenuQueue.empty()) {
                activeLevelUpMenu = levelUpMenuQueue.front();
                activeLevelUpMenu->Show();
                levelUpMenuOpen = activeLevelUpMenu->Paused();
            } else {
                activeLevelUpMenu = nullptr;
                levelUpMenuOpen = false;
            }
        }

        levelUpMenuOpen = (activeLevelUpMenu != nullptr) && activeLevelUpMenu->Paused();
        // Input and events
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent)) {
            GameWindow->Event(CurrentEvent);
            GameWindow->World()->Event(CurrentEvent);
            GameWindow->Controllers()->Event(CurrentEvent);

            if (pauseMenuOpen)
                pauseMenu.HandleEvent(CurrentEvent);

            if (levelUpMenuOpen)
                activeLevelUpMenu->HandleEvent(CurrentEvent);

            switch (CurrentEvent.type) {
                case SDL_QUIT:
                    QuitSound->PlaySound();
                    GameWindow->Deinitialize(true); // close everything except sound

                    while (Mix_Playing(-1)) {} // wait until last sound is done playing
                    delete GameWindow;
                    return 0;

                case SDL_KEYDOWN: {
                    SDL_Scancode ScancodeKey = CurrentEvent.key.keysym.scancode;
                    if (ScancodeKey == SDL_SCANCODE_ESCAPE) {
                        pauseMenu.Show();
                    } else if (ScancodeKey == SDL_SCANCODE_Z) {
                        new CharacterNPC(GameWindow->World(),
                                         50.0,
                                         Vec2d(32 * 30, 32),
                                         Vec2d(0, 10),
                                         NPC_TURRET,
                                         true);
                    }
                }
                    break;
                case SDL_CONTROLLERDEVICEADDED: {
                    int DeviceID = CurrentEvent.cdevice.which;
                    GameController* CurrentController = GameWindow->Controllers()->OpenController(DeviceID);
                    auto NewPlayer = new Player(GameWindow->World(), "Controller");
                    auto NewChar = new Character(GameWindow->World(),
                                                 NewPlayer,
                                                 100.0,
                                                 Vec2d(32 * 17.5, 32 * 17.5),
                                                 Vec2d(10, 10));

                    NewChar->GiveWeapon(new WeaponGlock(nullptr));
                    NewChar->SetGameController(CurrentController);
                    HighSound->PlaySound();
                }
                    break;
                case SDL_CONTROLLERDEVICEREMOVED: {
                    int InstanceID = CurrentEvent.cdevice.which;
                    GameController* DeletedController = GameWindow->Controllers()->CloseController(InstanceID);
                    GameWindow->World()->DestroyPlayerByController(DeletedController);
                    GameWindow->World()->DestroyCharacterByController(DeletedController);
                    LowSound->PlaySound();
                }
                    break;
            }
        }

        // Ticking
        GameWindow->World()->Tick();
        GameWindow->Controllers()->TickReset();

        // Drawing
        GameWindow->World()->Draw();
        Render->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

        // Render the pause menu if open
        if (pauseMenuOpen) {
            pauseMenu.Render();
        }

        // Render one of the levelupmenus in queue if any
        if (levelUpMenuOpen) {
            activeLevelUpMenu->Render();
            if (!activeLevelUpMenu->Paused()) {
                levelUpMenuQueue.pop();
                if (levelUpMenuQueue.empty()) {
                    GameWindow->World()->SetPaused(false);
                }
            }
        }

        Render->UpdateWindow();

        if (GameWindow->World()->GetDelay() && (levelUpMenuOpen)) {
            SDL_Delay(1000); // Delay for 1000 milliseconds (1 second)
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                // Discard events
            }

            GameWindow->World()->SetDelay(false); // Reset the delay flag after the delay
        }

        Timer->Tick();
    }

    return 0;
}
