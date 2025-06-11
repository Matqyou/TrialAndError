//
// Created by Matq on 22/03/2023.
//

#include "GameReference.h"
#include "client/Assets.h"
#include "game/entities/Crate.h"
#include "game/entities/item/weapons/EntityGuns.h"
#include "game/entities/Projectile.h"
#include "game/entities/Error.h"
#include "game/entities/characters/character/Character.h"
#include "game/interface/GameModeMenu.h"
#include "game/interface/ClassSelectMenu.h"

LoadedSound GameReference::sQuitSound("ui.quit");

GameReference::GameReference()
{
    m_MainMenu = nullptr;
    m_GameWorld = nullptr;
    m_GameModeMenu = nullptr;
    m_Window = nullptr;
    m_Controllers = nullptr;
    m_Renderer = nullptr;
    m_GLContext = nullptr;
    m_Timer = nullptr;
    m_Random = nullptr;
    m_Interface = nullptr;
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

GameReference::~GameReference()
{
    Deinitialize(false);
    delete m_AssetsHandler;
}

void GameReference::AddPlayerClassMenu()
{
    ClassSelectMenu *menu = new ClassSelectMenu(this);
    m_ClassSelectMenus.push_back(menu);
}

void GameReference::RemovePlayerClassMenu()
{
    m_ClassSelectMenus.pop_back();
}

void GameReference::UpdateDimensions(int width, int height)
{
    m_Width = width;
    m_Height = height;
    m_Width2 = width / 2.0;
    m_Height2 = height / 2.0;
}

bool GameReference::InitializeSDL()
{
    if (!m_InitializedSDL)
    {
        m_InitializedSDL = !SDL_Init(SDL_INIT_EVERYTHING);
        if (!m_InitializedSDL)
        {
            std::printf("Error while initializing main %s\n", SDL_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeMix()
{
    if (!m_InitializedMix)
    {
        int MixFlags = MIX_INIT_MP3 | MIX_INIT_OGG;
        m_InitializedMix = Mix_Init(MixFlags) == MixFlags;
        if (!m_InitializedMix)
        {
            std::printf("Error while initializing Mix %s\n", Mix_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeAudio()
{
    if (!m_InitializedAudio)
    {
        m_InitializedAudio = !Mix_OpenAudio(44100 * 2, MIX_DEFAULT_FORMAT, 2, 1024 * 4);
        Mix_AllocateChannels(64);
        if (!m_InitializedAudio)
            std::printf("Warning while opening audio %s\n", Mix_GetError());
    }

    return true;
}

bool GameReference::InitializeImages()
{
    if (!m_InitializedImages)
    {
        int ImageFlags = IMG_INIT_PNG | IMG_INIT_JPG;
        m_InitializedImages = IMG_Init(ImageFlags) == ImageFlags;
        if (!m_InitializedImages)
        {
            std::printf("Error while initializing Image %s\n", IMG_GetError());
            return false;
        }
    }

    return true;
}

bool GameReference::InitializeTTF()
{
    if (!m_InitializedTTF)
    {
        m_InitializedTTF = !TTF_Init();
        if (!m_InitializedTTF)
        {
            std::printf("Error while initializing TTF %s\n", TTF_GetError());
            return false;
        }
    }

    return true;
}

void GameReference::WaitForSoundToFinish()
{
    while (Mix_Playing(-1))
    {
        SDL_Delay(10);
    }
}

bool GameReference::Initialize()
{
    SDL_version Version;
    SDL_GetVersion(&Version);
    auto image_version = IMG_Linked_Version();
    auto mixer_version = Mix_Linked_Version();
    auto ttf_version = TTF_Linked_Version();
    std::cout << FStringColors("&8SDL2 %u.%u.%u", Version.major, Version.minor, Version.patch) << std::endl;
    std::cout << FStringColors("&8SDLimage %u.%u.%u", image_version->major, image_version->minor, image_version->patch) << std::endl;
    std::cout << FStringColors("&8SDLmixer %u.%u.%u", mixer_version->major, mixer_version->minor, mixer_version->patch) << std::endl;
    std::cout << FStringColors("&8SDLttf %u.%u.%u", ttf_version->major, ttf_version->minor, ttf_version->patch) << std::endl;
    std::cout << FStringColors("&8---------------------------- &fInitializing &8----------------------------") << std::endl;

    if (!InitializeSDL() ||
        !InitializeMix() ||
        !InitializeAudio() ||
        !InitializeImages() ||
        !InitializeTTF())
        return false;

    if (!m_Window)
    {
        UpdateDimensions(1280, 720);
        m_Window = SDL_CreateWindow("TrialAndError",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    m_Width,
                                    m_Height,
                                    SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if (!m_Window)
        {
            std::printf("Error while creating the window %s\n", SDL_GetError());
            return false;
        }
    }

    if (!m_Renderer)
    {
        m_Renderer = SDL_CreateRenderer(m_Window, -1, 0);
        if (!m_Renderer)
        {
            std::printf("Error while creating the renderer %s\n", SDL_GetError());
            return false;
        }
    }
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    m_GLContext = SDL_GL_CreateContext(m_Window);

    Assets::initialize(m_Renderer, m_InitializedAudio);

    if (!m_Timer)
        m_Timer = new Clock(60);
    if (!m_Random)
        m_Random = new Randomizer();
    if (!m_Draw)
        m_Draw = new Drawing(this);
    if (!m_Interface)
        m_Interface = new Interface(this);
    if (!m_AssetsHandler)
        m_AssetsHandler = new AssetsManager();
    if (!m_MainMenu)
        m_MainMenu = new MainMenu(this);
    if (!m_GameModeMenu)
        m_GameModeMenu = new GameModeMenu(this);
    if (!m_Controllers)
        m_Controllers = new GameControllers();

    //    m_Draw->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    m_AssetsHandler->TextHandler()->LoadFont("assets/fonts/Minecraft.ttf", 10);

    std::cout << FStringColors("&8------------------------------------------------------------------------") << std::endl;
    std::cout << std::endl;
    return true;
}

void GameReference::Deinitialize(bool play_quit_sound)
{
    std::cout << FStringColors(
                     "&8---------------------------- &fDeinitializing(play_quit_sound = %s) &8----------------------------",
                     play_quit_sound ? "true" : "false")
              << std::endl;
    Assets::PauseMusic();
    delete m_GameWorld;
    m_GameWorld = nullptr;
    delete m_MainMenu;
    m_MainMenu = nullptr;
    delete m_Controllers;
    m_Controllers = nullptr;
    m_AssetsHandler->DeinitializeImages();
    delete m_Draw;
    m_Draw = nullptr;
    delete m_Random;
    m_Random = nullptr;
    delete m_Timer;
    m_Timer = nullptr;
    if (m_Renderer)
        SDL_DestroyRenderer(m_Renderer);
    if (m_Window)
        SDL_DestroyWindow(m_Window);

    m_Draw = nullptr;
    m_Timer = nullptr;
    m_Renderer = nullptr;
    m_Window = nullptr;

    if (m_InitializedTTF)
    {
        m_InitializedTTF = false;
        TTF_Quit();
        std::cout << FStringColors("[Game] &8Closed TTF") << std::endl;
    }

    if (m_InitializedImages)
    {
        m_InitializedImages = false;
        IMG_Quit();
        std::cout << FStringColors("[Game] &8Closed Images") << std::endl;
    }

    if (play_quit_sound)
    {
        sQuitSound.GetSound()->PlaySound();
        WaitForSoundToFinish();
        delete this;
        return;
    }

    Assets::deinitialize();
    if (m_InitializedAudio)
    {
        m_InitializedAudio = false;
        Mix_CloseAudio();
        std::cout << FStringColors("[Game] &8Closed Audio") << std::endl;
    }
    if (m_InitializedMix)
    {
        m_InitializedMix = false;
        Mix_Quit();
        std::cout << FStringColors("[Game] &8Closed Mixer") << std::endl;
    }
    if (m_InitializedSDL)
    {
        m_InitializedSDL = false;
        SDL_Quit();
        std::cout << FStringColors("[Game] &8Closed SDL") << std::endl;
    }

    exit(0);
}

void GameReference::Event(const SDL_Event &event)
{
    if (event.type != SDL_WINDOWEVENT ||
        event.window.event != SDL_WINDOWEVENT_SIZE_CHANGED)
        return;

    UpdateDimensions(event.window.data1, event.window.data2);
}

void GameReference::AddPendingClass(PlayerClass *playerClass)
{
    m_PendingPlayerClasses.push_back(playerClass);
    m_ClassSelectMenus.pop_back();
}

void GameReference::InitializeSandbox(PlayerClass *primaryClass)
{
    m_GameWorld = new GameWorld(this, 50, 30);
    m_GameWorld->SetTestingMode(true); // stop waves
    m_Draw->SetWorld(m_GameWorld);
    Character::ms_BotNamePlate = new TextSurface(m_GameWorld->GameWindow()->Assetz(),
                                                 m_GameWorld->GameWindow()->Assetz()->TextHandler()->GetMainFont(),
                                                 "Bot User", {255, 150, 150, 255});

    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
            new Crate(m_GameWorld, Vec2d(200 + 50 * x, 200 + 50 * y), DropType(rand() % 2));

    for (int x = 0; x < NUM_ERROR_TYPES; x++)
        new Error(m_GameWorld, Vec2d(50 + 50 * x, m_GameWorld->GetHeight() - 50), ErrorType(x));

    for (int x = 0; x < NUM_AMMO_TYPES; x++)
        new AmmoBox(m_GameWorld, AmmoType(x), Vec2d(50 + 50 * x, m_GameWorld->GetHeight() - 100), 1000);

    double WeaponsY = m_GameWorld->GetHeight() - 150;
    new EntityGlock(m_GameWorld, nullptr, nullptr, Vec2d(100, WeaponsY));
    new EntityShotgun(m_GameWorld, nullptr, nullptr, Vec2d(200, WeaponsY));
    new EntityBurst(m_GameWorld, nullptr, nullptr, Vec2d(300, WeaponsY));
    new EntityMinigun(m_GameWorld, nullptr, nullptr, Vec2d(400, WeaponsY));
    new EntitySniper(m_GameWorld, nullptr, nullptr, Vec2d(500, WeaponsY));
    new EntityPatersonNavy(m_GameWorld, nullptr, nullptr, Vec2d(600, WeaponsY));

    auto Player1 = new Player(m_GameWorld, "Keyboard", primaryClass);
    auto Char1 = new Character(m_GameWorld,
                               Player1,
                               100.0,
                               Vec2d(32 * 17.5, 32 * 17.5),
                               Vec2d(10, 10),
                               false);
    Player1->GainXP(300);
}

void GameReference::InitializeInfinite()
{
    m_GameWorld = new GameWorld(this, 50, 30);
    m_Draw->SetWorld(m_GameWorld);
    Character::ms_BotNamePlate = new TextSurface(
        m_GameWorld->GameWindow()->Assetz(),
        m_GameWorld->GameWindow()->Assetz()->TextHandler()->GetMainFont(),
        "Bot User", {255, 150, 150, 255});

    const auto &connectedControllers = Controllers()->GetConnectedControllers();

    for (size_t i = 0; i < m_PendingPlayerClasses.size(); ++i)
    {
        std::string name = (i == 0) ? "Keyboard" : "Controller";
        auto player = new Player(m_GameWorld, name, m_PendingPlayerClasses[i]);
        auto character = new Character(m_GameWorld,
                                       player,
                                       100.0,
                                       Vec2d(32 * 17.5, 32 * 17.5),
                                       Vec2d(10, 10),
                                       false);

        if (i > 0 && connectedControllers.size() >= i)
        {
            character->SetGameController(connectedControllers[i - 1]);
        }
    }
    m_PendingPlayerClasses.clear();
}