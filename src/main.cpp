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
#include "game/interface/MainMenu.h"
#include "game/interface/PauseMenu.h"
#include <vector>
#include <iostream>
#include <random>

GameReference *GameWindow;
GameWorld *World;
GameControllers *Controllers;

bool Initialize()
{
    srand(time(nullptr));

    GameWindow = new GameReference();
    if (!GameWindow->Initialize())
        return false;

    SDL_ShowCursor(0);
    AssetsManager *AssetsHandler = GameWindow->Assets();
    SoundManager *SoundHandler = AssetsHandler->SoundHandler();
    ImageManager *ImageHandler = AssetsHandler->ImageHandler();

//    Character::ms_Texture = ImageHandler->LoadTexture("assets/images/entities/Fist.png", true);
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
    Projectile::ms_TextureSniper =
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
    World = new GameWorld(GameWindow, 50, 30);
    GameWindow->Render()->SetWorld(World);
    Character::ms_BotNamePlate = new TextSurface(World->GameWindow()->Assets(),
                                                 World->GameWindow()->Assets()->TextHandler()->GetMainFont(),
                                                 "Bot User", {255, 150, 150, 255});
    new Crate(World, Vec2d(200, 200), DropType(rand() % 2));
    new Crate(World, Vec2d(400, 200), DropType(rand() % 2));
    new Crate(World, Vec2d(600, 200), DropType(rand() % 2));
    new Crate(World, Vec2d(200, 400), DropType(rand() % 2));
    new Crate(World, Vec2d(400, 400), DropType(rand() % 2));
    new Crate(World, Vec2d(600, 400), DropType(rand() % 2));
    new Crate(World, Vec2d(200, 600), DropType(rand() % 2));
    new Crate(World, Vec2d(400, 600), DropType(rand() % 2));
    new Crate(World, Vec2d(600, 600), DropType(rand() % 2));

    new EntityGlock(World, nullptr, nullptr, Vec2d(800, 200));
    new EntityShotgun(World, nullptr, nullptr, Vec2d(900, 200));
    new EntityBurst(World, nullptr, nullptr, Vec2d(1000, 200));
    new EntityMinigun(World, nullptr, nullptr, Vec2d(1100, 200));
    new EntitySniper(World, nullptr, nullptr, Vec2d(1200, 200));

    Controllers = new GameControllers();
    auto Player1 = new Player(World, "Keyboard");
    auto Char1 = new Character(World,
                               Player1,
                               100.0,
                               Vec2d(32 * 17.5, 32 * 17.5),
                               Vec2d(10, 10));
    Player1->GainXP(300);
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

    MainMenu mainMenu(GameWindow);
    mainMenu.Show();
    Vec2i RealMouse;
    bool Running = true;
    StartUp();

    PauseMenu pauseMenu(World, &mainMenu);
    LevelUpMenu *activeLevelUpMenu = nullptr;
    std::queue<LevelUpMenu *> levelUpMenuQueue;
    bool pauseMenuOpen = false;
    bool levelUpMenuOpen = false;
    bool delayActive = false;
    while (Running)
    {
        pauseMenuOpen = pauseMenu.Paused();

        if(!levelUpMenuOpen){
        for (auto player = World->FirstPlayer(); player != nullptr; player = player->Next())
        {
            std::queue<LevelUpMenu *> playerQueue = player->GetLevelUpMenuQueue();
            while (!playerQueue.empty())
            {
                levelUpMenuQueue.push(playerQueue.front());
                playerQueue.pop();
                player->SetLevelUpMenuQueue(playerQueue);
            }
            playerQueue = std::queue<LevelUpMenu *>();
        }

        if (!levelUpMenuQueue.empty())
        {
            activeLevelUpMenu = levelUpMenuQueue.front();
            activeLevelUpMenu->Show();
            levelUpMenuOpen = activeLevelUpMenu->Paused();
        }
        else
        {
            activeLevelUpMenu = nullptr;
            levelUpMenuOpen = false;
        }
        }

        levelUpMenuOpen = (activeLevelUpMenu != nullptr) && activeLevelUpMenu->Paused();
        // Input and events
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent))
        {
            GameWindow->Event(CurrentEvent);
            World->Event(CurrentEvent);
            Controllers->Event(CurrentEvent);

            if (pauseMenuOpen)
                pauseMenu.HandleEvent(CurrentEvent);

            if (levelUpMenuOpen)
                activeLevelUpMenu->HandleEvent(CurrentEvent);

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
                    pauseMenu.Show();
                }
                else if (ScancodeKey == SDL_SCANCODE_Z)
                {
                    new CharacterNPC(World,
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
        }

        if (!World->GetPaused())
        {
            // Update game logic
            World->Tick();
            Controllers->TickReset();
        }
        World->Draw();
        Render->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

        // Render the pause menu if open
        if (pauseMenuOpen)
        {
            pauseMenu.Render();
        }

        // Render one of the levelupmenus in queue if any
        if (levelUpMenuOpen)
        {
            activeLevelUpMenu->Render();
            if (!activeLevelUpMenu->Paused())
            {
                levelUpMenuQueue.pop();
                if(levelUpMenuQueue.empty())
                {
                    World->SetPaused(false);
                }
            }
        }

        Render->UpdateWindow();

        if (World->GetDelay() && (levelUpMenuOpen))
        {
            SDL_Delay(1000); // Delay for 1000 milliseconds (1 second)
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                // Discard events
            }
            World->SetDelay(false); // Reset the delay flag after the delay
        }

        Timer->Tick();
    }

    return 0;
}
