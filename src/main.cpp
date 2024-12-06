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
    AssetsManager* AssetsHandler = GameWindow->Assets();
    SoundManager* SoundHandler = AssetsHandler->SoundHandler();
    Decals* decals = Decals::Get();

//    Character::ms_Texture = ImageHandler->LoadTexture("assets/images/entities/Fist.png", true);
    Hands::ms_FistTexture = decals->GetTexture("entities.fist");
    ItemEntity::ms_TextureGlock = decals->GetTexture("entities.items.glock");
    ItemEntity::ms_TextureShotgun = decals->GetTexture("entities.items.shotgun");
    ItemEntity::ms_TextureBurst = decals->GetTexture("entities.items.burst");
    ItemEntity::ms_TextureSniper = decals->GetTexture("entities.items.sniper");
    ItemEntity::ms_TexturesMinigun[0] = decals->GetTexture("entities.items.minigun1");
    ItemEntity::ms_TexturesMinigun[1] = decals->GetTexture("entities.items.minigun2");
    ItemEntity::ms_TexturesMinigun[2] = decals->GetTexture("entities.items.minigun3");
    ItemEntity::ms_TexturesMinigun[3] = decals->GetTexture("entities.items.minigun4");
    Projectile::ms_TextureGlock = decals->GetTexture("entities.projectiles.glockbullet");
    Projectile::ms_TextureBurst = decals->GetTexture("entities.projectiles.burstbullet");
    Projectile::ms_TextureShotgun = decals->GetTexture("entities.projectiles.shotgunbullet");
    Projectile::ms_TextureSniper = decals->GetTexture("entities.projectiles.shotgunbullet");
    Projectile::ms_TextureMinigun = decals->GetTexture("entities.projectiles.minigunbullet");
    AmmoBox::ms_TextureGlock = decals->GetTexture("entities.glockammo");
    AmmoBox::ms_TextureShotgun = decals->GetTexture("entities.shotgunammo");
    AmmoBox::ms_TextureBurst = decals->GetTexture("entities.burstammo");
    AmmoBox::ms_TextureMinigun = decals->GetTexture("entities.minigunammo");
    Crate::ms_TextureBox = decals->GetTexture("entities.ammocrate");
    Crate::ms_TextureBreakingBox1 = decals->GetTexture("entities.rts_crate_breaking_1");
    Crate::ms_TextureBreakingBox2 = decals->GetTexture("entities.rts_crate_breaking_2");
    Error::ms_TextureErrorInvincible = decals->GetTexture("entities.invincible");
    Error::ms_TextureErrorSpiky = decals->GetTexture("entities.cactus");
    Error::ms_TextureErrorSlowDown = decals->GetTexture("entities.clock");
    Error::ms_TextureErrorDisorianted = decals->GetTexture("entities.disorianted");
    Error::ms_TextureErrorHealersParadise = decals->GetTexture("entities.healer");
    Error::ms_TextureErrorConfusingHP = decals->GetTexture("entities.confusion");
    Error::ms_TextureErrorRanged = decals->GetTexture("entities.ranged");
    Error::ms_TextureError = decals->GetTexture("entities.golden_apple");

    Character::ms_TextureErrorInvincible = decals->GetTexture("icons.invincible");
    Character::ms_TextureErrorSpiky = decals->GetTexture("icons.cactus");
    Character::ms_TextureErrorSlowDown = decals->GetTexture("icons.clock");
    Character::ms_TextureErrorDisorianted = decals->GetTexture("icons.disorianted");
    Character::ms_TextureErrorHealersParadise = decals->GetTexture("icons.healer");
    Character::ms_TextureErrorConfusingHP = decals->GetTexture("icons.confusion");
    Character::ms_TextureErrorRanged = decals->GetTexture("icons.ranged");
    Character::ms_TextureError = decals->GetTexture("entities.golden_apple");

    Character::ms_TextureGlock = decals->GetTexture("weapons.glock");
    Character::ms_TextureShotgun = decals->GetTexture("weapons.shotgun");
    Character::ms_TextureBurst = decals->GetTexture("weapons.burst");
    Character::ms_TextureSniper = decals->GetTexture("weapons.sniper");
    Character::ms_TexturesMinigun[0] = decals->GetTexture("weapons.minigun1");
    Character::ms_TexturesMinigun[1] = decals->GetTexture("weapons.minigun2");
    Character::ms_TexturesMinigun[2] = decals->GetTexture("weapons.minigun3");
    Character::ms_TexturesMinigun[3] = decals->GetTexture("weapons.minigun4");

    // Load sounds
    Sound* Basic_Death = SoundHandler->LoadSound("assets/sounds/basic_death.wav", true);
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
    Character::ms_InvincibleHitSound = SoundHandler->LoadSound("assets/sounds/entities/character/InvincibleHit.wav", true);
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
    AssetsManager* AssetsHandler = GameWindow->Assets();
    SoundManager* SoundHandler = AssetsHandler->SoundHandler();

    Decals* decals = Decals::Get();
    Texture* TextureResume = decals->GetTexture("interface.resume");
    Texture* TexturePlay = decals->GetTexture("interface.playbutton");
    Texture* TextureBack = decals->GetTexture("interface.back");
    Texture* TextureExit = decals->GetTexture("interface.exit");
    Texture* TextureSettings = decals->GetTexture("interface.settings");
    Texture* MenuTexture = decals->GetTexture("interface.menu");
    Texture* Vignette = decals->GetTexture("backgrounds.vignette");
    Vignette->SetAlphaMod(200);

    Sound* LowSound = SoundHandler->LoadSound("assets/sounds/Low.wav", true);
    Sound* HighSound = SoundHandler->LoadSound("assets/sounds/High.wav", true);
    Sound* QuitSound = SoundHandler->LoadSound("assets/sounds/Quit.wav", true);
    Sound* LowUISound = SoundHandler->LoadSound("assets/sounds/LowUI.wav", true);
    Sound* MidUISound = SoundHandler->LoadSound("assets/sounds/MidUI.wav", true);
    Sound* HighUISound = SoundHandler->LoadSound("assets/sounds/HighUI.wav", true);

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
                    SoundHandler->PlaySound(QuitSound);
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
                    SoundHandler->PlaySound(HighSound);
                }
                    break;
                case SDL_CONTROLLERDEVICEREMOVED: {
                    int InstanceID = CurrentEvent.cdevice.which;
                    GameController* DeletedController = GameWindow->Controllers()->CloseController(InstanceID);
                    GameWindow->World()->DestroyPlayerByController(DeletedController);
                    GameWindow->World()->DestroyCharacterByController(DeletedController);
                    SoundHandler->PlaySound(LowSound);
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
