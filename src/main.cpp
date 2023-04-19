#define SDL_MAIN_HANDLED
#include "GameReference.h"
#include "GameWorld.h"
#include "technical stuff/GameControllers.h"
#include "game/entities/character/Character.h"
#include "game/entities/Bullets.h"
#include "game/indicators/TextSurface.h"
#include "game/entities/Ammo.h"
#include <vector>
#include <iostream>

GameReference* GameWindow;
GameWorld* World;
GameControllers* Controllers;
Ammo* AmmoCrates;
bool Initialize() {
    srand(time(nullptr));
    GameWindow = new GameReference();
    if (!GameWindow->Initialize())
        return false;

    TextManager* TextHandler = GameWindow->Assets()->TextHandler();
    TextHandler->LoadFont("GROBOLD.ttf", 16);

    World = new GameWorld(GameWindow, 200, 200);
    World->SetCameraPos(30, 30);
    GameWindow->RenderClass()->SetWorld(World);

    //Temp ammo spawn
    AmmoCrates = new Ammo(World, GLOCK_AMMO, 200,200);
    AmmoCrates = new Ammo(World, GLOCK_AMMO, 400,200);
    AmmoCrates = new Ammo(World, GLOCK_AMMO, 600,200);
    Controllers = new GameControllers();
    new Character(World, 30, 30, 10, 10);
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
    Texture* TextureConnected = ImageHandler->LoadTexture("assets/images/UI/chain.png", true);
    Texture* TextureDisconnected = ImageHandler->LoadTexture("assets/images/UI/dis_chain.png", true);
    Texture* TextureIcon = ImageHandler->LoadTexture("assets/images/UI/PS4_Controller_Icon.png", true);
    Texture* Vignette = ImageHandler->LoadTexture("assets/images/backgrounds/vignette.png", true);
    Vignette->SetAlphaMod(200);
    Texture* Pellet = ImageHandler->LoadTexture("assets/images/Bullets/Pellet.png", true);
    Texture*  Chad = ImageHandler->LoadTexture("assets/images/Character/Chad.png", true);
    Texture* DefaultText = TextHandler->Render(GameWindow->Assets()->TextHandler()->FirstFont(),"Undefined", { 190, 100, 100, 180 }, true);
    Texture* Fist = ImageHandler->CreateTexture(SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1, 1, true);
    Draw->SetColor(255, 255, 255, 255);
    Draw->SetRenderTarget(Fist);
    Draw->Clear();
    Draw->SetRenderTarget(nullptr);

    Character::ms_Texture = Chad;
    Character::ms_FistTexture = Fist;
    Bullets::ms_Texture = Pellet;

    // Load sounds
    Sound* Background = SoundHandler->LoadSound("assets/sounds/background_theme.mp3", true);
    Sound* Fail_Death = SoundHandler->LoadSound("assets/sounds/fail_death.mp3", true);
    Sound* Basic_Death = SoundHandler->LoadSound("assets/sounds/basic_death.wav", true);
    Sound* Epic_Death = SoundHandler->LoadSound("assets/sounds/epic_death.wav", true);
    Sound* LowSound = SoundHandler->LoadSound("assets/sounds/Low.wav", true);
    Sound* HighSound = SoundHandler->LoadSound("assets/sounds/High.wav", true);
    Sound* QuitSound = SoundHandler->LoadSound("assets/sounds/Quit.wav", true);
    Sound* LowUISound = SoundHandler->LoadSound("assets/sounds/LowUI.wav", true);
    Sound* MidUISound = SoundHandler->LoadSound("assets/sounds/MidUI.wav", true);
    Sound* HighUISound = SoundHandler->LoadSound("assets/sounds/HighUI.wav", true);
    Sound* GlockShootSound = SoundHandler->LoadSound("assets/sounds/GlockShoot.wav", true);
    GlockShootSound->SetVolume(64); // max 128
    Sound* GlockClickSound = SoundHandler->LoadSound("assets/sounds/GunClick.wav", true);
    GlockClickSound->SetVolume(32); // max 128
    Sound* ShotgunShootSound = SoundHandler->LoadSound("assets/sounds/ShotgunShoot.wav", true);
    Sound* BurstShootSound = SoundHandler->LoadSound("assets/sounds/ShootBurst.wav", true);
    Sound* ShotgunReloadSound = SoundHandler->LoadSound("assets/sounds/ShotgunReload.wav", true);
    Sound* Hurt1 = SoundHandler->LoadSound("assets/sounds/Character/Hurt1.wav", true);
    Sound* Hurt2 = SoundHandler->LoadSound("assets/sounds/Character/Hurt2.wav", true);
    Sound* Hurt3 = SoundHandler->LoadSound("assets/sounds/Character/Hurt3.wav", true);

    WeaponGlock::ms_ShootSound = GlockShootSound;
    WeaponGlock::ms_ClickSound = GlockClickSound;
    WeaponGlock::ms_ReloadSound = ShotgunReloadSound;
    ProjectileWeapon::ms_NoAmmo = Fail_Death;
    WeaponShotgun::ms_ShootSound = ShotgunShootSound;
    WeaponShotgun::ms_ClickSound = GlockClickSound;
    WeaponShotgun::ms_ReloadSound = ShotgunReloadSound;
    WeaponBurst::ms_ShootSound = BurstShootSound;
    WeaponBurst::ms_ClickSound = GlockClickSound;
    WeaponBurst::ms_ReloadSound = ShotgunReloadSound;
    WeaponMinigun::ms_ShootSound = BurstShootSound;
    WeaponMinigun::ms_ClickSound = GlockClickSound;
    WeaponMinigun::ms_ReloadSound = ShotgunReloadSound;
    Character::ms_DeathSound = Basic_Death;
    Character::ms_HitSounds[0] = Hurt1;
    Character::ms_HitSounds[1] = Hurt2;
    Character::ms_HitSounds[2] = Hurt3;

    TextSurface TestText = TextSurface(AssetsHandler, TextHandler->FirstFont(), "Jesse -.. .. .", {255, 255, 255, 255 });

    int ignore_ticks = 5;
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
                    if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                        bool Pause = !World->Paused();
                        World->SetPaused(Pause);
                        if (Pause) SoundHandler->PlaySound(MidUISound);
                        else SoundHandler->PlaySound(LowUISound);
                    }
                    // else if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_F11)
                    //     SDL_SetWindowFullscreen(Window, !(SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN));
                } break;
                case SDL_CONTROLLERDEVICEADDED: {
                    int DeviceID = CurrentEvent.cdevice.which;
                    GameController* CurrentController = Controllers->OpenController(DeviceID);
                    auto* NewPlayer = new Character(World, 30, 30, 10, 10); // Add new player
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
                                ignore_ticks = 5; //Minimum ticks it has to skip to not shoot on resume
                            }
                            World->SetPaused(false);
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
        if(!ignore_ticks)World->Tick();
        else ignore_ticks -=1;

        // Drawing
        Draw->SetColor(120, 0, 120, 255);
        Draw->Clear();

        World->Draw();

        Draw->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

        Texture* TextTexture = TestText.Update();
        SDL_Rect DestinationRect;
        TextTexture->Query(nullptr, nullptr, &DestinationRect.w, &DestinationRect.h);
        DestinationRect.x = 0;
        DestinationRect.y = GameWindow->Height() - DestinationRect.h;
        Draw->RenderTexture(TextTexture->SDLTexture(), nullptr, DestinationRect);

        if (World->Paused()) {
            Draw->SetBlendingMode(SDL_BLENDMODE_BLEND);
            Draw->SetColor(0, 0, 0, 100);
            Draw->BlendFullscreen();
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
