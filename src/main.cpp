/*
 *  Trial And Error
 *  This is a game.
 *
 *  Made by two and a half guys.
 *  Project started 16.03.23
 */

#define SDL_MAIN_HANDLED
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
#include <vector>
#include <iostream>
#include <random>

GameReference *GameWindow;
GameWorld *World;
GameControllers *Controllers;

bool Initialize()
{
    GameWindow = new GameReference();
    if (!GameWindow->Initialize())
        return false;
    SDL_ShowCursor(0);
    AssetsManager *AssetsHandler = GameWindow->Assets();
    SoundManager *SoundHandler = AssetsHandler->SoundHandler();
    ImageManager *ImageHandler = AssetsHandler->ImageHandler();

    Character::ms_Texture = ImageHandler->LoadTexture("assets/images/entities/Fist.png", true);
    Hands::ms_FistTexture = ImageHandler->LoadTexture("assets/images/entities/Fist.png", true);
    ItemEntity::ms_TextureGlock = ImageHandler->LoadTexture("assets/images/entities/items/Glock.png", true);
    ItemEntity::ms_TextureShotgun = ImageHandler->LoadTexture("assets/images/entities/items/Shotgun.png", true);
    ItemEntity::ms_TextureBurst = ImageHandler->LoadTexture("assets/images/entities/items/Burst.png", true);
    ItemEntity::ms_TextureSniper = ImageHandler->LoadTexture("assets/images/entities/items/Sniper.png", true);
    ItemEntity::ms_TexturesMinigun[0] = ImageHandler->LoadTexture("assets/images/entities/items/Minigun1.png", true);
    ItemEntity::ms_TexturesMinigun[1] = ImageHandler->LoadTexture("assets/images/entities/items/Minigun2.png", true);
    ItemEntity::ms_TexturesMinigun[2] = ImageHandler->LoadTexture("assets/images/entities/items/Minigun3.png", true);
    ItemEntity::ms_TexturesMinigun[3] = ImageHandler->LoadTexture("assets/images/entities/items/Minigun4.png", true);
    Projectile::ms_TextureGlock = ImageHandler->LoadTexture("assets/images/entities/projectiles/GlockBullet.png", true);
    Projectile::ms_TextureBurst = ImageHandler->LoadTexture("assets/images/entities/projectiles/BurstBullet.png", true);
    Projectile::ms_TextureShotgun =
        ImageHandler->LoadTexture("assets/images/entities/projectiles/ShotgunBullet.png", true);
    Projectile::ms_TextureMinigun =
        ImageHandler->LoadTexture("assets/images/entities/projectiles/MinigunBullet.png", true);
    AmmoBox::ms_TextureGlock = ImageHandler->LoadTexture("assets/images/entities/GlockAmmo.png", true);
    AmmoBox::ms_TextureShotgun = ImageHandler->LoadTexture("assets/images/entities/ShotgunAmmo.png", true);
    AmmoBox::ms_TextureBurst = ImageHandler->LoadTexture("assets/images/entities/BurstAmmo.png", true);
    AmmoBox::ms_TextureMinigun = ImageHandler->LoadTexture("assets/images/entities/MinigunAmmo.png", true);
    Crate::ms_TextureBox = ImageHandler->LoadTexture("assets/images/entities/AmmoCrate.png", true);
    Crate::ms_TextureBreakingBox1 = ImageHandler->LoadTexture("assets/images/entities/RTS_Crate_Breaking_1.png", true);
    Crate::ms_TextureBreakingBox2 = ImageHandler->LoadTexture("assets/images/entities/RTS_Crate_Breaking_2.png", true);
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
    Character::ms_TextureShotgun = ImageHandler->LoadTexture("assets/images/weapons/Shotgun.png", true);
    Character::ms_TextureBurst = ImageHandler->LoadTexture("assets/images/weapons/Burst.png", true);
    Character::ms_TextureSniper = ImageHandler->LoadTexture("assets/images/weapons/Sniper.png", true);
    Character::ms_TexturesMinigun[0] = ImageHandler->LoadTexture("assets/images/weapons/Minigun1.png", true);
    Character::ms_TexturesMinigun[1] = ImageHandler->LoadTexture("assets/images/weapons/Minigun2.png", true);
    Character::ms_TexturesMinigun[2] = ImageHandler->LoadTexture("assets/images/weapons/Minigun3.png", true);
    Character::ms_TexturesMinigun[3] = ImageHandler->LoadTexture("assets/images/weapons/Minigun4.png", true);
    // Load sounds
    Sound *Basic_Death = SoundHandler->LoadSound("assets/sounds/basic_death.wav", true);
    Sound *FailReloadSound = SoundHandler->LoadSound("assets/sounds/FailReload.wav", true);
    Sound *GlockShootSound = SoundHandler->LoadSound("assets/sounds/GlockShoot.wav", true);
    GlockShootSound->SetVolume(64); // max 128
    Sound *GlockClickSound = SoundHandler->LoadSound("assets/sounds/GunClick.wav", true);
    GlockClickSound->SetVolume(32); // max 128
    Sound *ShotgunShootSound = SoundHandler->LoadSound("assets/sounds/ShotgunShoot.wav", true);
    Sound *BurstShootSound = SoundHandler->LoadSound("assets/sounds/ShootBurst.wav", true);
    Sound *ShotgunReloadSound = SoundHandler->LoadSound("assets/sounds/ShotgunReload.wav", true);

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
    WeaponSniper::ms_ShootSound = nullptr;
    WeaponSniper::ms_ClickSound = nullptr;
    WeaponSniper::ms_ReloadSound = nullptr;
    WeaponMinigun::ms_ShootSound = BurstShootSound;
    WeaponMinigun::ms_ClickSound = FailReloadSound;
    WeaponMinigun::ms_ReloadSound = ShotgunReloadSound;
    Character::ms_DeathSound = Basic_Death;
    Character::ms_HitSounds[0] = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt1.wav", true);
    Character::ms_HitSounds[1] = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt2.wav", true);
    Character::ms_HitSounds[2] = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt3.wav", true);
    Character::ms_AmmoPickupSound = SoundHandler->LoadSound("assets/sounds/entities/ammo/Pick6.wav", true);
    Character::ms_ItemSwitchSound = SoundHandler->LoadSound("assets/sounds/WeaponSwitch.wav", true);
    Crate::ms_BoxSound = SoundHandler->LoadSound("assets/sounds/BoxHit.wav", true);
    Crate::ms_HitSound = SoundHandler->LoadSound("assets/sounds/entities/character/Hurt1.wav", true);

    // Temp ammo spawn, had to generate random and set the value for each one, also changed it to also sending an int
    //  to the Crate constructor, so its easier to work with and i dont need to have acess to DropTypes, which i do, but no,
    //  it only works cuz this is main and i prefer to use the same typa fix on both ERRORS and crates, since
    //  for ERRORS that fix wouldnt have worked, also it lowers the line count cuz i dont gotta set the ERROR/AMMO types
    //  to each random number, can just send the number straight up
    return true;
}

bool StartUp()
{
    srand(time(nullptr));
    World = new GameWorld(GameWindow, 50, 30);
    GameWindow->Render()->SetWorld(World);
    Character::ms_BotNamePlate = new TextSurface(World->GameWindow()->Assets(),
                                                 World->GameWindow()->Assets()->TextHandler()->GetMainFont(),
                                                 "Bot User", {255, 150, 150, 255});
    new Crate(World, Vec2d(200, 200), 20, DropType(rand() % 2));
    new Crate(World, Vec2d(400, 200), 20, DropType(rand() % 2));
    new Crate(World, Vec2d(600, 200), 20, DropType(rand() % 2));
    new Crate(World, Vec2d(200, 400), 20, DropType(rand() % 2));
    new Crate(World, Vec2d(400, 400), 20, DropType(rand() % 2));
    new Crate(World, Vec2d(600, 400), 20, DropType(rand() % 2));
    new Crate(World, Vec2d(200, 600), 20, DropType(rand() % 2));
    new Crate(World, Vec2d(400, 600), 20, DropType(rand() % 2));
    new Crate(World, Vec2d(600, 600), 20, DropType(rand() % 2));

    new EntityGlock(World, nullptr, nullptr, Vec2d(800, 200));
    new EntityShotgun(World, nullptr,nullptr, Vec2d(900, 200));
    new EntityBurst(World, nullptr,nullptr, Vec2d(1000, 200));
    new EntityMinigun(World, nullptr,nullptr, Vec2d(1100, 200));
    new EntitySniper(World, nullptr,nullptr, Vec2d(1200, 200));

    Controllers = new GameControllers();
    auto Player1 = new Player(World, "Keyboard");
    auto Char1 = new Character(World,
                               Player1,
                               100.0,
                               Vec2d(32 * 17.5, 32 * 17.5),
                               Vec2d(10, 10));
    // Char1->GiveWeapon(new WeaponGlock(nullptr));

    return true;
}
int main()
{
    if (!Initialize())
    {
        std::printf("Terminating..");
        exit(1);
    }

    Clock *Timer = GameWindow->Timer();
    Drawing *Render = GameWindow->Render();
    AssetsManager *AssetsHandler = GameWindow->Assets();
    SoundManager *SoundHandler = AssetsHandler->SoundHandler();
    ImageManager *ImageHandler = AssetsHandler->ImageHandler();

    Texture *TextureResume = ImageHandler->LoadTexture("assets/images/interface/Resume.png", true);
    Texture *TexturePlay = ImageHandler->LoadTexture("assets/images/interface/PlayButton.png", true);
    Texture *TextureBack = ImageHandler->LoadTexture("assets/images/interface/Back.png", true);
    Texture *TextureExit = ImageHandler->LoadTexture("assets/images/interface/Exit.png", true);
    Texture *TextureSettings = ImageHandler->LoadTexture("assets/images/interface/Settings.png", true);
    Texture *MenuTexture = ImageHandler->LoadTexture("assets/images/interface/Menu.png", true);
    Texture *Vignette = ImageHandler->LoadTexture("assets/images/backgrounds/vignette.png", true);
    Vignette->SetAlphaMod(200);

    Sound *LowSound = SoundHandler->LoadSound("assets/sounds/Low.wav", true);
    Sound *HighSound = SoundHandler->LoadSound("assets/sounds/High.wav", true);
    Sound *QuitSound = SoundHandler->LoadSound("assets/sounds/Quit.wav", true);
    Sound *LowUISound = SoundHandler->LoadSound("assets/sounds/LowUI.wav", true);
    Sound *MidUISound = SoundHandler->LoadSound("assets/sounds/MidUI.wav", true);
    Sound *HighUISound = SoundHandler->LoadSound("assets/sounds/HighUI.wav", true);

    SDL_Rect PlayButtonRect = {int(GameWindow->GetWidth2()) - 180,
                               int(GameWindow->GetHeight2()) - 40,
                               360, 80};
    SDL_Rect ResumeButtonRect = {int(GameWindow->GetWidth2()) - 100,
                                 int(GameWindow->GetHeight2()) - 150,
                                 200, 70};
    SDL_Rect BackToMenuButtonRect = {int(GameWindow->GetWidth2()) - 100,
                                     int(GameWindow->GetHeight2()) + 50,
                                     200, 70};
    SDL_Rect ExitButtonRect = {int(GameWindow->GetWidth2()) - 180,
                               int(GameWindow->GetHeight2()) + 121,
                               360, 80};

    Vec2i RealMouse;
    bool MenuOpen = true;
    bool Running = true;
    while (MenuOpen)
    {
        // useless MainMenu classs yay
        Render->RenderTextureFullscreen(MenuTexture->SDLTexture(), nullptr);
        Render->RenderTexture(TexturePlay->SDLTexture(), nullptr, PlayButtonRect);
        Render->RenderTexture(TextureExit->SDLTexture(), nullptr, ExitButtonRect);
        SDL_ShowCursor(1);
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent))
        {
            GameWindow->Event(CurrentEvent);
            switch (CurrentEvent.type)
            {
            case SDL_QUIT:
            {
                MenuOpen = false;
                GameWindow->Deinitialize(true);
                Running = false;
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
                if (CurrentEvent.button.button == SDL_BUTTON_LEFT)
                {
                    int x = CurrentEvent.button.x;
                    int y = CurrentEvent.button.y;
                    if (x >= PlayButtonRect.x && x < PlayButtonRect.x + PlayButtonRect.w &&
                        y >= PlayButtonRect.y && y < PlayButtonRect.y + PlayButtonRect.h)
                    {
                        MenuOpen = false;
                        SoundHandler->PlaySound(LowUISound);
                    }
                    if (x >= ExitButtonRect.x && x < ExitButtonRect.x + ExitButtonRect.w &&
                        y >= ExitButtonRect.y && y < ExitButtonRect.y + ExitButtonRect.h)
                    {
                        // Not sure if making it delete things not initialised yet is an issue but its not
                        // yelling at me sooo, but if i dont make it do that it would be a memory leak, since
                        // i would be just doing running = false which doesnt delete things just stops the main loop

                        SoundHandler->PlaySound(QuitSound);
                        GameWindow->Deinitialize(true); // close everything except sound

                        delete Controllers;
                        delete World;
                        while (Mix_Playing(-1))
                        {
                        } // wait until last sound is done playing
                        delete GameWindow;
                        return 0;
                    }
                }
            }
            break;
            case SDL_WINDOWEVENT:
            {
                if (CurrentEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    PlayButtonRect = {int(GameWindow->GetWidth2()) - 180,
                                      int(GameWindow->GetHeight2()) - 40,
                                      360, 80};
                    ResumeButtonRect = {int(GameWindow->GetWidth2()) - 100,
                                        int(GameWindow->GetHeight2()) - 150,
                                        200, 70};
                    BackToMenuButtonRect = {int(GameWindow->GetWidth2()) - 100,
                                            int(GameWindow->GetHeight2()) + 50,
                                            200, 70};
                    ExitButtonRect = {int(GameWindow->GetWidth2()) - 180,
                                      int(GameWindow->GetHeight2()) + 121,
                                      360, 80};
                }
            }
            break;
            }
        }
        Render->UpdateWindow();
    }

    StartUp();
    while (!MenuOpen && Running)
    {
        // Input and events
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent))
        {
            GameWindow->Event(CurrentEvent);
            World->Event(CurrentEvent);
            Controllers->Event(CurrentEvent);

            switch (CurrentEvent.type)
            {
            case SDL_QUIT:
                SoundHandler->PlaySound(QuitSound);
                GameWindow->Deinitialize(true); // close everything except sound

                delete Controllers;
                delete World;
                while (Mix_Playing(-1))
                {
                } // wait until last sound is done playing
                delete GameWindow;
                return 0;

            case SDL_KEYDOWN:
            {
                SDL_Scancode ScancodeKey = CurrentEvent.key.keysym.scancode;
                if (ScancodeKey == SDL_SCANCODE_ESCAPE)
                {
                    bool Pause = !World->GetPaused();
                    World->SetPaused(Pause);

                    if (Pause)
                        SoundHandler->PlaySound(MidUISound);
                    else
                        SoundHandler->PlaySound(LowUISound);
                }
                else if (ScancodeKey == SDL_SCANCODE_Z)
                {
                    new CharacterNPC(World,
                                     20.0,
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
                GameController *CurrentController = Controllers->OpenController(DeviceID);
                auto NewPlayer = new Player(World, "Controller");
                auto NewChar = new Character(World,
                                             NewPlayer,
                                             100.0,
                                             Vec2d(32 * 17.5, 32 * 17.5),
                                             Vec2d(10, 10));
                NewChar->GiveWeapon(new WeaponGlock(nullptr));
                NewChar->SetGameController(CurrentController);
                SoundHandler->PlaySound(HighSound);
            }
            break;
            case SDL_CONTROLLERDEVICEREMOVED:
            {
                int InstanceID = CurrentEvent.cdevice.which;
                GameController *DeletedController = Controllers->CloseController(InstanceID);
                World->DestroyPlayerByController(DeletedController);
                World->DestroyCharacterByController(DeletedController);
                SoundHandler->PlaySound(LowSound);
            }
            break;
            case SDL_MOUSEBUTTONDOWN:
            {
                if (World->GetPaused())
                {
                    if (CurrentEvent.button.button == SDL_BUTTON_LEFT)
                    {
                        int x = CurrentEvent.button.x;
                        int y = CurrentEvent.button.y;
                        if (x >= ResumeButtonRect.x && x < ResumeButtonRect.x + ResumeButtonRect.w &&
                            y >= ResumeButtonRect.y && y < ResumeButtonRect.y + ResumeButtonRect.h)
                        {
                            SoundHandler->PlaySound(LowUISound);
                            World->SetPaused(false);
                        }
                        else if (x >= BackToMenuButtonRect.x && x < BackToMenuButtonRect.x + BackToMenuButtonRect.w &&
                                 y >= BackToMenuButtonRect.y && y < BackToMenuButtonRect.y + BackToMenuButtonRect.h)
                        {
                            MenuOpen = true;
                            SoundHandler->PlaySound(MidUISound);
                        }
                    }
                }
            }

            break;
            case SDL_WINDOWEVENT:
            {
                if (CurrentEvent.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    PlayButtonRect = {int(GameWindow->GetWidth2()) - 180,
                                      int(GameWindow->GetHeight2()) - 40,
                                      360, 80};
                    ResumeButtonRect = {int(GameWindow->GetWidth2()) - 100,
                                        int(GameWindow->GetHeight2()) - 150,
                                        200, 70};
                    BackToMenuButtonRect = {int(GameWindow->GetWidth2()) - 100,
                                            int(GameWindow->GetHeight2()) + 50,
                                            200, 70};
                    ExitButtonRect = {int(GameWindow->GetWidth2()) - 180,
                                      int(GameWindow->GetHeight2()) + 121,
                                      360, 80};
                }
            }
            break;
            }
        }

        // Ticking
        if (!World->GetPaused())
        {
            World->Tick();
        }

        Controllers->TickReset();

        // Drawing
        World->Draw();
        Render->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

        if (World->GetPaused())
        {
            Render->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
            Render->SetColor(0, 0, 0, 100);
            Render->FillAll();
            Render->SetDrawBlendMode(SDL_BLENDMODE_NONE);

            Render->RenderTexture(TextureResume->SDLTexture(), nullptr, ResumeButtonRect);
            Render->RenderTexture(TextureBack->SDLTexture(), nullptr, BackToMenuButtonRect);
        }

        Render->UpdateWindow();
        Timer->Tick();
    }
}
