#define SDL_MAIN_HANDLED
#include "GameReference.h"
#include "GameWorld.h"
#include "technical stuff/GameControllers.h"
#include "game/entities/character/Character.h"
#include "game/entities/Bullets.h"
#include "game/indicators/TextSurface.h"
#include "game/entities/Ammo.h"
#include "game/entities/Crates.h"
#include "game/entities/Error.h"
#include <vector>
#include <iostream>
#include <random>

GameReference* GameWindow;
GameWorld* World;
GameControllers* Controllers;

bool Initialize() {
    srand(time(nullptr));
    GameWindow = new GameReference();
    if (!GameWindow->Initialize())
        return false;

    TextManager* TextHandler = GameWindow->Assets()->TextHandler();
    TextHandler->LoadFont("Minecraft.ttf", 16);

    World = new GameWorld(GameWindow, 50, 25);
    World->SetCameraPos(30, 30);
    GameWindow->RenderClass()->SetWorld(World);

    //Temp ammo spawn

    new Crates(World,200, 200, 20);
    new Crates(World,400, 200, 20);
    new Crates(World,600, 200, 20);
    new Crates(World,200, 400, 20);
    new Crates(World,400, 400, 20);
    new Crates(World,600, 400, 20);
    new Crates(World,200, 600, 20);
    new Crates(World,400, 600, 20);
    new Crates(World,600, 600, 20);

    Controllers = new GameControllers();
    auto Player1 = new Player(World, "Crazy speedrunner #1");
    // new Player(World, "Speedrunner crazy #2");
    new Character(World, Player1, 100.0,
                  32*30, 30, 10, 10,
                  false);

    for (int i = 0; i < 2; i++)
        new Character(World, nullptr, 100.0,
                      32*30, 300, 0, 0,
                      true);

    return true;
}

int main() {
    if (!Initialize()) {
        std::printf("Terminating..");
        exit(1);
    }

    Clock* Timer = GameWindow->Timer();
    Drawing* Draw = GameWindow->RenderClass();
    AssetsManager* AssetsHandler = GameWindow->Assets();
    SoundManager* SoundHandler = AssetsHandler->SoundHandler();
    ImageManager* ImageHandler = AssetsHandler->ImageHandler();
    TextManager* TextHandler = AssetsHandler->TextHandler();

    // Load the PNG images
    Texture* TextureStart = ImageHandler->LoadTexture("assets/images/UI/Start.png", true);
    Texture* TextureSettings = ImageHandler->LoadTexture("assets/images/UI/Settings.png", true);
    Texture* Vignette = ImageHandler->LoadTexture("assets/images/backgrounds/vignette.png", true);
    Vignette->SetAlphaMod(200);

    Character::ms_Texture = ImageHandler->LoadTexture("assets/images/character/Most_Epic_Cactus_W_Hat.png", true);
    Hands::ms_FistTexture = ImageHandler->LoadTexture("assets/images/entities/Fist.png", true);
    Bullets::ms_Texture = ImageHandler->LoadTexture("assets/images/entities/Pellet.png", true);
    Ammo::ms_TextureGlock = ImageHandler->LoadTexture("assets/images/entities/GlockAmmo.png", true);
    Ammo::ms_TextureShotgun = ImageHandler->LoadTexture("assets/images/entities/ShotgunAmmo.png", true);
    Ammo::ms_TextureBurst = ImageHandler->LoadTexture("assets/images/entities/BurstAmmo.png", true);
    Ammo::ms_TextureMinigun = ImageHandler->LoadTexture("assets/images/entities/MinigunAmmo.png", true);
    Crates::ms_TextureBox = ImageHandler->LoadTexture("assets/images/entities/RTS_Crate.png", true);
    Error::ms_TextureError = ImageHandler->LoadTexture("assets/images/character/golden_apple.png", true);
    // Load sounds
    Sound* Background = SoundHandler->LoadSound("assets/sounds/background_theme.mp3", true);
    Sound* Basic_Death = SoundHandler->LoadSound("assets/sounds/basic_death.wav", true);
    Sound* Epic_Death = SoundHandler->LoadSound("assets/sounds/epic_death.wav", true);
    Sound* LowSound = SoundHandler->LoadSound("assets/sounds/Low.wav", true);
    Sound* HighSound = SoundHandler->LoadSound("assets/sounds/High.wav", true);
    Sound* QuitSound = SoundHandler->LoadSound("assets/sounds/Quit.wav", true);
    Sound* LowUISound = SoundHandler->LoadSound("assets/sounds/LowUI.wav", true);
    Sound* MidUISound = SoundHandler->LoadSound("assets/sounds/MidUI.wav", true);
    Sound* HighUISound = SoundHandler->LoadSound("assets/sounds/HighUI.wav", true);
    Sound* FailReloadSound = SoundHandler->LoadSound("assets/sounds/FailReload.wav", true);
    Sound* GlockShootSound = SoundHandler->LoadSound("assets/sounds/GlockShoot.wav", true);
    GlockShootSound->SetVolume(64); // max 128
    Sound* GlockClickSound = SoundHandler->LoadSound("assets/sounds/GunClick.wav", true);
    GlockClickSound->SetVolume(32); // max 128
    Sound* ShotgunShootSound = SoundHandler->LoadSound("assets/sounds/ShotgunShoot.wav", true);
    Sound* BurstShootSound = SoundHandler->LoadSound("assets/sounds/ShootBurst.wav", true);
    Sound* ShotgunReloadSound = SoundHandler->LoadSound("assets/sounds/ShotgunReload.wav", true);

    WeaponGlock::ms_ShootSound = GlockShootSound;
    WeaponGlock::ms_ClickSound = FailReloadSound;
    WeaponGlock::ms_ReloadSound = ShotgunReloadSound;
    ProjectileWeapon::ms_NoAmmo = GlockClickSound;
    WeaponShotgun::ms_ShootSound = ShotgunShootSound;
    WeaponShotgun::ms_ClickSound = FailReloadSound;
    WeaponShotgun::ms_ReloadSound = ShotgunReloadSound;
    WeaponBurst::ms_ShootSound = BurstShootSound;
    WeaponBurst::ms_ClickSound = FailReloadSound;
    WeaponBurst::ms_ReloadSound = ShotgunReloadSound;
    WeaponMinigun::ms_ShootSound = BurstShootSound;
    WeaponMinigun::ms_ClickSound = FailReloadSound;
    WeaponMinigun::ms_ReloadSound = ShotgunReloadSound;
    Character::ms_DeathSound = Basic_Death;
    Character::ms_HitSounds[0] = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt1.wav", true);
    Character::ms_HitSounds[1] = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt2.wav", true);
    Character::ms_HitSounds[2] = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt3.wav", true);
    Character::ms_AmmoPickupSound = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick6.wav", true);
    // Ammo::ms_PickupSounds[0] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick1.wav", true);
    // Ammo::ms_PickupSounds[1] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick2.wav", true);
    // Ammo::ms_PickupSounds[2] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick3.wav", true);
    // Ammo::ms_PickupSounds[3] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick4.wav", true);
    // Ammo::ms_PickupSounds[4] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick5.wav", true);
    // Ammo::ms_PickupSounds[5] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick6.wav", true);
    // Ammo::ms_PickupSounds[6] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick7.wav", true);
    Crates::ms_HitSound = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt1.wav", true);

    Character::ms_BotNamePlate = new TextSurface(World->GameWindow()->Assets(),
                                                 World->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                                 "Bot User", { 255, 150, 150, 255 });

    TextSurface TestText = TextSurface(AssetsHandler, TextHandler->FirstFont(), "Jesse -.. .. .", {255, 255, 255, 255 });

    bool Running = true;
    bool Config = true;
    while (Running) {
        // Render the Start button
        SDL_Rect startButtonRect = { GameWindow->Width()/2-150, GameWindow->Height()/2-200, 300, 100 };
        // Render the Settings button
        SDL_Rect settingsButtonRect = { GameWindow->Width()/2-150, GameWindow->Height()/2-50, 300, 100 };
        // Input and events
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent)) {
            GameWindow->Event(CurrentEvent);
            World->Event(CurrentEvent);
            Controllers->Event(CurrentEvent);

            switch (CurrentEvent.type) {
                case SDL_QUIT: {
                    Running = false;
                } break;
                case SDL_KEYDOWN: {
                    SDL_Scancode ScancodeKey = CurrentEvent.key.keysym.scancode;
                    if (ScancodeKey == SDL_SCANCODE_ESCAPE) {
                        bool Pause = !World->Paused();
                        World->SetPaused(Pause);
                        if (Pause) SoundHandler->PlaySound(MidUISound);
                        else SoundHandler->PlaySound(LowUISound);
                    } else if (ScancodeKey == SDL_SCANCODE_Z) {
                        new Character(World, nullptr, 100.0,
                                      32*30, 30, 10, 10,
                                      true);
                    }
                    // else if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_F11)
                    //     SDL_SetWindowFullscreen(Window, !(SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN));
                } break;
                case SDL_CONTROLLERDEVICEADDED: {
                    int DeviceID = CurrentEvent.cdevice.which;
                    GameController* CurrentController = Controllers->OpenController(DeviceID);
                    auto* NewPlayer = new Character(World, nullptr, 100.0,
                                                    32*30, 30, 10, 10,
                                                    false); // TODO fix ;)
                    NewPlayer->SetGameController(CurrentController);
                    SoundHandler->PlaySound(HighSound);
                } break;
                case SDL_CONTROLLERDEVICEREMOVED: {
                    int InstanceID = CurrentEvent.cdevice.which;
                    GameController* DeletedController = Controllers->CloseController(InstanceID);
                    World->DestroyPlayerByController(DeletedController);
                    SoundHandler->PlaySound(LowSound);
                } break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (CurrentEvent.button.button == SDL_BUTTON_LEFT)
                    {
                        int x = CurrentEvent.button.x;
                        int y = CurrentEvent.button.y;
                        if (x >= startButtonRect.x && x < startButtonRect.x + startButtonRect.w &&
                            y >= startButtonRect.y && y < startButtonRect.y + startButtonRect.h)
                        {
                            if(World->Paused()) {
                                SoundHandler->PlaySound(LowUISound);
                                World->SetPaused(false);
                            }
                        }
                        else if (x >= settingsButtonRect.x && x < settingsButtonRect.x + settingsButtonRect.w &&
                            y >= settingsButtonRect.y && y < settingsButtonRect.y + settingsButtonRect.h)
                        {
                            Config = !Config;
                            if(World->Paused())SoundHandler->PlaySound(MidUISound);
                        }
                    }
                } break;
            }
        }
    
        // Ticking
        World->Tick();
        Controllers->TickLast();

        // Drawing
        Draw->SetColor(120, 0, 120, 255);
        Draw->Clear();

        World->Draw();

        Draw->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

        Texture* TextTexture = TestText.RequestUpdate();
        SDL_Rect DestinationRect;
        TextTexture->Query(nullptr, nullptr, &DestinationRect.w, &DestinationRect.h);
        DestinationRect.x = 0;
        DestinationRect.y = GameWindow->Height() - DestinationRect.h;
        Draw->RenderTexture(TextTexture->SDLTexture(), nullptr, DestinationRect);

        if (World->Paused()) {
            Draw->SetBlendingMode(SDL_BLENDMODE_BLEND);
            Draw->SetColor(0, 0, 0, 100);
            Draw->FillAll();
            Draw->SetBlendingMode(SDL_BLENDMODE_NONE);

            // start
            Draw->SetColor(90, 20, 20, 255);
            Draw->FillRect({ GameWindow->Width() / 2 - 150, GameWindow->Height()/2-200, 300, 100 });
            //   setting
            Draw->SetColor(20, 20, 90, 255);
            Draw->FillRect({ GameWindow->Width() / 2 - 150, GameWindow->Height()/2-50, 300, 100 });

            Draw->RenderTexture(TextureStart->SDLTexture(), nullptr, startButtonRect);
            Draw->RenderTexture(TextureSettings->SDLTexture(), nullptr, settingsButtonRect);

            if (!Config) {
                //   setting
                Draw->SetColor(20, 20, 90, 255);
                Draw->FillRect(settingsButtonRect);

                Draw->RenderTexture(TextureSettings->SDLTexture(), nullptr, settingsButtonRect);
            }
        }

        Draw->UpdateWindow();
        Timer->Tick();
    }

    SoundHandler->PlaySound(QuitSound);
    GameWindow->Deinitialize(true); // close everything except sound

    delete Controllers;
    delete World;
    while(Mix_Playing(-1)) { }  // wait until last sound is done playing
    delete GameWindow;
    return 0;
}
