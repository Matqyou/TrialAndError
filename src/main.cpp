/*
 *  Trial And Error
 *  This is a game.
 *
 *  Made by two and a half guys.
 *  Project started 16.03.23
 */

#define SDL_MAIN_HANDLED
#include "GameReference.h"
#include "GameWorld.h"
#include "technical stuff/GameControllers.h"
#include "game/entities/character/npc/CharacterNPC.h"
#include "game/entities/character/Character.h"
#include "game/entities/Projectile.h"
#include "game/indicators/TextSurface.h"
#include "game/entities/AmmoBox.h"
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

    World = new GameWorld(GameWindow, 50, 40);
    GameWindow->Render()->SetWorld(World);

    //Temp ammo spawn, had to generate random and set the value for each one, also changed it to also sending an int
    // to the Crates constructor, so its easier to work with and i dont need to have acess to DropTypes, which i do, but no,
    // it only works cuz this is main and i prefer to use the same typa fix on both ERRORS and crates, since
    // for ERRORS that fix wouldnt have worked, also it lowers the line count cuz i dont gotta set the ERROR/AMMO types
    // to each random number, can just send the number straight up
    new Crates(World, 200, 200, 20, rand() % 2);
    new Crates(World, 400, 200, 20, rand() % 2);
    new Crates(World, 600, 200, 20, rand() % 2);
    new Crates(World, 200, 400, 20, rand() % 2);
    new Crates(World, 400, 400, 20, rand() % 2);
    new Crates(World, 600, 400, 20, rand() % 2);
    new Crates(World, 200, 600, 20, rand() % 2);
    new Crates(World, 400, 600, 20, rand() % 2);
    new Crates(World, 600, 600, 20, rand() % 2);

    Controllers = new GameControllers();
    auto Player1 = new Player(World, "Keyboard");
    auto Char1 = new Character(World, Player1, 100.0,
                               32 * 17.5, 32 * 17.5, 10, 10);
    Char1->GiveWeapon(WEAPON_GLOCK);
    Char1->GiveWeapon(WEAPON_BURST);
    Char1->GiveWeapon(WEAPON_SHOTGUN);
    Char1->GiveWeapon(WEAPON_MINIGUN);

    return true;
}

int main() {
    if (!Initialize()) {
        std::printf("Terminating..");
        exit(1);
    }

    Clock* Timer = GameWindow->Timer();
    Drawing* Draw = GameWindow->Render();
    AssetsManager* AssetsHandler = GameWindow->Assets();
    SoundManager* SoundHandler = AssetsHandler->SoundHandler();
    ImageManager* ImageHandler = AssetsHandler->ImageHandler();

    // Load the PNG images
    Texture* TextureStart = ImageHandler->LoadTexture("assets/images/interface/Start.png", true);
    Texture* TextureSettings = ImageHandler->LoadTexture("assets/images/interface/Settings.png", true);
    Texture* Vignette = ImageHandler->LoadTexture("assets/images/backgrounds/vignette.png", true);
    Vignette->SetAlphaMod(200);

    Character::ms_Texture = ImageHandler->LoadTexture("assets/images/entities/Fist.png", true);
    Hands::ms_FistTexture = ImageHandler->LoadTexture("assets/images/entities/Fist.png", true);
    Projectile::ms_TextureGlock = ImageHandler->LoadTexture("assets/images/entities/projectiles/GlockBullet.png", true);
    Projectile::ms_TextureBurst = ImageHandler->LoadTexture("assets/images/entities/projectiles/BurstBullet.png", true);
    Projectile::ms_TextureShotgun = ImageHandler->LoadTexture("assets/images/entities/projectiles/ShotgunBullet.png", true);
    Projectile::ms_TextureMinigun = ImageHandler->LoadTexture("assets/images/entities/projectiles/MinigunBullet.png", true);
    AmmoBox::ms_TextureGlock = ImageHandler->LoadTexture("assets/images/entities/GlockAmmo.png", true);
    AmmoBox::ms_TextureShotgun = ImageHandler->LoadTexture("assets/images/entities/ShotgunAmmo.png", true);
    AmmoBox::ms_TextureBurst = ImageHandler->LoadTexture("assets/images/entities/BurstAmmo.png", true);
    AmmoBox::ms_TextureMinigun = ImageHandler->LoadTexture("assets/images/entities/MinigunAmmo.png", true);
    Crates::ms_TextureBox = ImageHandler->LoadTexture("assets/images/entities/RTS_Crate.png", true);
    Crates::ms_TextureBreakingBox1 = ImageHandler->LoadTexture("assets/images/entities/RTS_Crate_Breaking_1.png", true);
    Crates::ms_TextureBreakingBox2 = ImageHandler->LoadTexture("assets/images/entities/RTS_Crate_Breaking_2.png", true);
    Error::ms_TextureErrorInvincible = ImageHandler->LoadTexture("assets/images/entities/Invincible.png", true);
    Error::ms_TextureErrorSpiky = ImageHandler->LoadTexture("assets/images/entities/Cactus.png", true);
    Error::ms_TextureErrorSlowDown = ImageHandler->LoadTexture("assets/images/entities/Clock.png", true);
    Error::ms_TextureErrorDisorianted = ImageHandler->LoadTexture("assets/images/entities/Disorianted.png", true);
    Error::ms_TextureErrorHealersParadise = ImageHandler->LoadTexture("assets/images/entities/Healer.png", true);
    Error::ms_TextureErrorConfusingHP = ImageHandler->LoadTexture("assets/images/entities/Confusion.png", true);
    Error::ms_TextureErrorRanged = ImageHandler->LoadTexture("assets/images/entities/Ranged.png", true);
    Error::ms_TextureError = ImageHandler->LoadTexture("assets/images/entities/golden_apple.png", true);

    Character::ms_TextureErrorInvincible = ImageHandler->LoadTexture("assets/images/icons/Invincible.png", true);
    Character::ms_TextureErrorSpiky = ImageHandler->LoadTexture("assets/images/icons/Cactus.png", true);
    Character::ms_TextureErrorSlowDown = ImageHandler->LoadTexture("assets/images/icons/Clock.png", true);
    Character::ms_TextureErrorDisorianted = ImageHandler->LoadTexture("assets/images/icons/Disorianted.png", true);
    Character::ms_TextureErrorHealersParadise = ImageHandler->LoadTexture("assets/images/icons/Healer.png", true);
    Character::ms_TextureErrorConfusingHP = ImageHandler->LoadTexture("assets/images/icons/Confusion.png", true);
    Character::ms_TextureErrorRanged = ImageHandler->LoadTexture("assets/images/icons/Ranged.png", true);
    Character::ms_TextureError = ImageHandler->LoadTexture("assets/images/entities/golden_apple.png", true);

    Character::ms_TextureGlock = ImageHandler->LoadTexture("assets/images/weapons/Glock.png", true);
    Character::ms_TextureBurst = ImageHandler->LoadTexture("assets/images/weapons/Burst.png", true);
    Character::ms_TextureShotgun = ImageHandler->LoadTexture("assets/images/weapons/Shotgun.png", true);
    Character::ms_TexturesMinigun[0] = ImageHandler->LoadTexture("assets/images/weapons/Minigun1.png", true);
    Character::ms_TexturesMinigun[1] = ImageHandler->LoadTexture("assets/images/weapons/Minigun2.png", true);
    Character::ms_TexturesMinigun[2] = ImageHandler->LoadTexture("assets/images/weapons/Minigun3.png", true);
    Character::ms_TexturesMinigun[3] = ImageHandler->LoadTexture("assets/images/weapons/Minigun4.png", true);
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
    Character::ms_ItemSwitchSound = SoundHandler->LoadSound("assets/sounds/WeaponSwitch.wav", true);
    // AmmoBox::ms_PickupSounds[0] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick1.wav", true);
    // AmmoBox::ms_PickupSounds[1] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick2.wav", true);
    // AmmoBox::ms_PickupSounds[2] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick3.wav", true);
    // AmmoBox::ms_PickupSounds[3] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick4.wav", true);
    // AmmoBox::ms_PickupSounds[4] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick5.wav", true);
    // AmmoBox::ms_PickupSounds[5] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick6.wav", true);
    // AmmoBox::ms_PickupSounds[6] = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick7.wav", true);
    Crates::ms_BoxSound = SoundHandler->LoadSound("assets/sounds/BoxHit.wav", true);
    Crates::ms_HitSound = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt1.wav", true);

    Character::ms_BotNamePlate = new TextSurface(World->GameWindow()->Assets(),
                                                 World->GameWindow()->Assets()->TextHandler()->GetMainFont(),
                                                 "Bot User", { 255, 150, 150, 255 });

    SDL_Rect StartButtonRect = { int(GameWindow->GetWidth2()) - 150,
                                 int(GameWindow->GetHeight2()) - 200,
                                 300, 100 };
    SDL_Rect SettingsButtonRect = { int(GameWindow->GetWidth2()) - 150,
                                    int(GameWindow->GetHeight2()) - 50,
                                    300, 100 };

    bool Running = true;
    while (Running) {
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
                        bool Pause = !World->GetPaused();
                        World->SetPaused(Pause);

                        if (Pause) SoundHandler->PlaySound(MidUISound);
                        else SoundHandler->PlaySound(LowUISound);
                    } else if (ScancodeKey == SDL_SCANCODE_Z) {
                        new CharacterNPC(World, 20.0, 32 * 30, 30, 10, 10, NPC_TURRET, true);
                    }
                } break;
                case SDL_CONTROLLERDEVICEADDED: {
                    int DeviceID = CurrentEvent.cdevice.which;
                    GameController* CurrentController = Controllers->OpenController(DeviceID);
                    auto NewPlayer = new Player(World, "Controller");
                    auto NewChar = new Character(World, NewPlayer, 100.0,
                                                 32 * 17.5, 32 * 17.5, 10, 10);
                    NewChar->GiveWeapon(WEAPON_GLOCK);
                    NewChar->GiveWeapon(WEAPON_BURST);
                    NewChar->GiveWeapon(WEAPON_SHOTGUN);
                    NewChar->GiveWeapon(WEAPON_MINIGUN);
                    NewChar->SetGameController(CurrentController);
                    SoundHandler->PlaySound(HighSound);
                }
                    break;
                case SDL_CONTROLLERDEVICEREMOVED: {
                    int InstanceID = CurrentEvent.cdevice.which;
                    GameController* DeletedController = Controllers->CloseController(InstanceID);
                    World->DestroyPlayerByController(DeletedController);
                    World->DestroyCharacterByController(DeletedController);
                    SoundHandler->PlaySound(LowSound);
                } break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (World->GetPaused()) {
                        if (CurrentEvent.button.button == SDL_BUTTON_LEFT) {
                            int x = CurrentEvent.button.x;
                            int y = CurrentEvent.button.y;
                            if (x >= StartButtonRect.x && x < StartButtonRect.x + StartButtonRect.w &&
                                y >= StartButtonRect.y && y < StartButtonRect.y + StartButtonRect.h) {
                                SoundHandler->PlaySound(LowUISound);
                                World->SetPaused(false);
                            } else if (x >= SettingsButtonRect.x && x < SettingsButtonRect.x + SettingsButtonRect.w &&
                                y >= SettingsButtonRect.y && y < SettingsButtonRect.y + SettingsButtonRect.h) {
                                SoundHandler->PlaySound(MidUISound);
                            }
                        }
                    }
                } break;
            }
        }

        // Ticking
        World->Tick();
        Controllers->TickReset();

        // Drawing
        //Draw->SetColor(120, 120, 0, 255);
        //Draw->Clear();

        World->Draw();
        Draw->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

        if (World->GetPaused()) {
            Draw->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
            Draw->SetColor(0, 0, 0, 100);
            Draw->FillAll();
            Draw->SetDrawBlendMode(SDL_BLENDMODE_NONE);

            Draw->RenderTexture(TextureStart->SDLTexture(), nullptr, StartButtonRect);
            Draw->RenderTexture(TextureSettings->SDLTexture(), nullptr, SettingsButtonRect);
        }

        Draw->UpdateWindow();
        Timer->Tick();
    }

    SoundHandler->PlaySound(QuitSound);
    GameWindow->Deinitialize(true); // close everything except sound

    delete Controllers;
    delete World;
    while (Mix_Playing(-1)) {} // wait until last sound is done playing
    delete GameWindow;
    return 0;
}
