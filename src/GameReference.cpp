//
// Created by Matq on 22/03/2023.
//

#include "GameReference.h"
#include "client/Assets.h"
#include "game/entities/Crate.h"
#include "game/entities/item/weapons/EntityGuns.h"
#include "game/entities/Projectile.h"

GameReference::GameReference() {
    m_Window = nullptr;
    m_Renderer = nullptr;
    m_GLContext = nullptr;
    m_Timer = nullptr;
    m_Random = nullptr;
    m_Draw = nullptr;
    m_AssetsHandler = nullptr;
    m_Width = 0;
    m_Height = 0;
    m_Width2 = 0.0;
    m_Height2 = 0.0;
    m_InitializedSDL = false;
    m_InitializedMix = false;
    m_InitializedAudio = false;
    m_InitializedImages = false;
    m_InitializedTTF = false;
}

GameReference::~GameReference() {
    Deinitialize(false);
    delete m_AssetsHandler;
}

void GameReference::UpdateDimensions(int width, int height) {
    m_Width = width;
    m_Height = height;
    m_Width2 = width / 2.0;
    m_Height2 = height / 2.0;

    
}

bool GameReference::InitializeSDL() {
    if (!m_InitializedSDL) {
        m_InitializedSDL = !SDL_Init(SDL_INIT_EVERYTHING);
        if (!m_InitializedSDL) {
            std::printf("Error while initializing main %s\n", SDL_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeMix() {
    if (!m_InitializedMix) {
        int MixFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
        m_InitializedMix = Mix_Init(MixFlags) == MixFlags;
        if (!m_InitializedMix) {
            std::printf("Error while initializing Mix %s\n", Mix_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeAudio() {
    if (!m_InitializedAudio) {
        m_InitializedAudio = !Mix_OpenAudio(44100 * 2, MIX_DEFAULT_FORMAT, 2, 1024 * 4);
        Mix_AllocateChannels(16);
        if (!m_InitializedAudio) std::printf("Warning while opening audio %s\n", Mix_GetError());
    }

    return true;
}

bool GameReference::InitializeImages() {
    if (!m_InitializedImages) {
        int ImageFlags = IMG_INIT_PNG | IMG_INIT_JPG;
        m_InitializedImages = IMG_Init(ImageFlags) == ImageFlags;
        if (!m_InitializedImages) {
            std::printf("Error while initializing Image %s\n", IMG_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeTTF() {
    if (!m_InitializedTTF) {
        m_InitializedTTF = !TTF_Init();
        if (!m_InitializedTTF) {
            std::printf("Error while initializing TTF %s\n", TTF_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::Initialize() {
    SDL_version Version;
    SDL_GetVersion(&Version);
    std::cout << "Using SDL " << (int)Version.major << "." << (int)Version.minor << "." << (int)Version.patch << std::endl;

    if (!InitializeSDL() ||
        !InitializeMix() ||
        !InitializeAudio() ||
        !InitializeImages() ||
        !InitializeTTF())
        return false;

    if (!m_Window) {
        UpdateDimensions(1280, 720);
        m_Window = SDL_CreateWindow("TrialAndError",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    m_Width,
                                    m_Height,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if (!m_Window) {
            std::printf("Error while creating the window %s\n", SDL_GetError());
            return false;
        }
    }

    if (!m_Renderer) {
        m_Renderer = SDL_CreateRenderer(m_Window, -1, 0);
        if (!m_Renderer) {
            std::printf("Error while creating the renderer %s\n", SDL_GetError());
            return false;
        }
    }

    m_GLContext = SDL_GL_CreateContext(m_Window);

    Assets::initialize(m_Renderer, m_InitializedAudio);

    if (!m_Timer) m_Timer = new Clock(60);
    if (!m_Random) m_Random = new Randomizer();
    if (!m_Draw) m_Draw = new Drawing(this);
    if (!m_AssetsHandler) m_AssetsHandler = new AssetsManager(m_Renderer, m_InitializedAudio);

    m_GameWorld = new GameWorld(this, 50, 30);
    m_Controllers = new GameControllers();

    SoundManager* SoundHandler = m_AssetsHandler->SoundHandler();
    Assets* decals = Assets::Get();

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

    m_Draw->SetWorld(m_GameWorld);
    Character::ms_BotNamePlate = new TextSurface(m_GameWorld->GameWindow()->Assetz(),
                                                 m_GameWorld->GameWindow()->Assetz()->TextHandler()->GetMainFont(),
                                                 "Bot User", { 255, 150, 150, 255 });
    new Crate(m_GameWorld, Vec2d(200, 200), DropType(rand() % 2));
    new Crate(m_GameWorld, Vec2d(400, 200), DropType(rand() % 2));
    new Crate(m_GameWorld, Vec2d(600, 200), DropType(rand() % 2));
    new Crate(m_GameWorld, Vec2d(200, 400), DropType(rand() % 2));
    new Crate(m_GameWorld, Vec2d(400, 400), DropType(rand() % 2));
    new Crate(m_GameWorld, Vec2d(600, 400), DropType(rand() % 2));
    new Crate(m_GameWorld, Vec2d(200, 600), DropType(rand() % 2));
    new Crate(m_GameWorld, Vec2d(400, 600), DropType(rand() % 2));
    new Crate(m_GameWorld, Vec2d(600, 600), DropType(rand() % 2));

    new EntityGlock(m_GameWorld, nullptr, nullptr, Vec2d(800, 200));
    new EntityShotgun(m_GameWorld, nullptr, nullptr, Vec2d(900, 200));
    new EntityBurst(m_GameWorld, nullptr, nullptr, Vec2d(1000, 200));
    new EntityMinigun(m_GameWorld, nullptr, nullptr, Vec2d(1100, 200));
    new EntitySniper(m_GameWorld, nullptr, nullptr, Vec2d(1200, 200));

    auto Player1 = new Player(m_GameWorld, "Keyboard");
    auto Char1 = new Character(m_GameWorld,
                               Player1,
                               100.0,
                               Vec2d(32 * 17.5, 32 * 17.5),
                               Vec2d(10, 10));
    Player1->GainXP(300);
    // Char1->GiveWeapon(new WeaponGlock(nullptr));

    return true;
}

void GameReference::Deinitialize(bool keep_sound) {
    delete m_GameWorld;
    delete m_Controllers;
    Assets::deinitialize();
    m_AssetsHandler->DeinitializeImages();
    delete m_Draw;
    m_Draw = nullptr;
    delete m_Random;
    m_Random = nullptr;
    delete m_Timer;
    m_Timer = nullptr;
    if (m_Renderer) SDL_DestroyRenderer(m_Renderer);
    if (m_Window) SDL_DestroyWindow(m_Window);

    m_Draw = nullptr;
    m_Timer = nullptr;
    m_Renderer = nullptr;
    m_Window = nullptr;

    if (m_InitializedTTF) {
        m_InitializedTTF = false;
        TTF_Quit();
    }

    if (m_InitializedImages) {
        m_InitializedImages = false;
        IMG_Quit();
    }

    if (!keep_sound) { // TODO: Check this out -_- looks very sus
        m_AssetsHandler->DeinitializeSounds();

        if (m_InitializedAudio) {
            m_InitializedAudio = false;
            Mix_CloseAudio();
        }
        if (m_InitializedMix) {
            m_InitializedMix = false;
            Mix_Quit();
        }
        if (m_InitializedSDL) {
            m_InitializedSDL = false;
            SDL_Quit();
        }
    }
}

void GameReference::Event(const SDL_Event& event) {
    if (event.type != SDL_WINDOWEVENT ||
        event.window.event != SDL_WINDOWEVENT_SIZE_CHANGED)
        return;

    UpdateDimensions(event.window.data1, event.window.data2);
}