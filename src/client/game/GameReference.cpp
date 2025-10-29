//
// Created by Matq on 22/03/2023.
//

#include "GameReference.h"

#include <utility>
#include "client/game/entities/characters/character/Character.h"
#include "client/game/entities/item/weapons/EntityGuns.h"
#include "client/game/ui/menus/class_select/ClassSelectMenu.h"
#include "client/game/ui/menus/class_select/GamemodeMenu.h"
#include "client/game/entities/Projectile.h"
#include "client/game/entities/Crate.h"
#include "client/game/entities/Error.h"
#include "client/game/ui/CommonUI.h"
#include "client/core/Assets.h"

LinkSound sQuitSound("ui.quit");

GameData::GameData()
{
	world = nullptr;
	interface = nullptr;
	exit_callback = { };
}

GameData::~GameData()
{
	Deinitialize(false);
}

void GameData::SetExitApplicationCallback(Callback callback)
{
	exit_callback = std::move(callback);
}

void GameData::ExitApplication()
{
	exit_callback();
}

//void GameData::AddPlayerClassMenu()
//{
//	auto menu = new ClassSelectMenu();
//	m_ClassSelectMenus.push_back(menu);
//}

//void GameData::RemovePlayerClassMenu()
//{
//	m_ClassSelectMenus.pop_back();
//}

void GameData::DeleteWorld()
{
	delete world;
	world = nullptr;
}

bool GameData::Initialize()
{
	interface = new Interface();

	// Creates 1 player preferences object
	player_preferences.emplace_back();

	return true;
}

void GameData::Deinitialize(bool play_quit_sound)
{
	Assets.PauseMusic();
	delete interface;
	delete world;
}

//void GameData::AddPendingClass(PlayerClass *playerClass)
//{
//	m_PendingPlayerClasses.push_back(playerClass);
//	m_ClassSelectMenus.pop_back();
//}

void GameData::StartGame(Gamemode mode)
{
	if (mode == Gamemode::Sandbox)
	{
		InitializeSandbox();
	}
	else if (mode == Gamemode::Solo)
	{
		InitializeInfinite();
	}
	else if (mode == Gamemode::Levels)
	{
		// Levels are not implemented yet
	}

	// ADD ANY FUTURE GAME MODES HERE
}

void GameData::InitializeSandbox()
{
	world = new GameWorld(50, 30);
	world->SetTestingMode(true); // stop waves
	Character::ms_BotNamePlate = new TextSurface(CommonUI::fontDefault, "Bot User", { 255, 150, 150, 255 });

	for (int y = 0; y < 5; y++)
		for (int x = 0; x < 5; x++)
			new Crate(world, Vec2d(200 + 50 * x, 200 + 50 * y), DropType(rand() % 2));

	for (int x = 0; x < NUM_ERROR_TYPES; x++)
		new Error(world, Vec2d(50 + 50 * x, world->GetHeight() - 50), ErrorType(x));

	for (int x = 0; x < NUM_AMMO_TYPES; x++)
		new AmmoBox(world, AmmoType(x), Vec2d(50 + 50 * x, world->GetHeight() - 100), 1000);

	double WeaponsY = world->GetHeight() - 150;
	new EntityGlock(world, nullptr, nullptr, Vec2d(100, WeaponsY));
	new EntityShotgun(world, nullptr, nullptr, Vec2d(200, WeaponsY));
	new EntityBurst(world, nullptr, nullptr, Vec2d(300, WeaponsY));
	new EntityMinigun(world, nullptr, nullptr, Vec2d(400, WeaponsY));
	new EntitySniper(world, nullptr, nullptr, Vec2d(500, WeaponsY));
	new EntityPatersonNavy(world, nullptr, nullptr, Vec2d(600, WeaponsY));

//	auto Player1 = new Player(world, "Keyboard", m_PendingPlayerClasses[0]);
//	auto Char1 = new Character(world,
//							   Player1,
//							   100.0,
//							   Vec2d(32 * 17.5, 32 * 17.5),
//							   Vec2d(10, 10),
//							   false);
//	Player1->GainXP(300);
}

void GameData::InitializeInfinite()
{
	world = new GameWorld(50, 30);
	Character::ms_BotNamePlate = new TextSurface(CommonUI::fontDefault, "Bot User", { 255, 150, 150, 255 });

	world->InitPlayers();
}

void GameData::HandleEvent(const SDL_Event& sdl_event, EventContext& event_context)
{
	if (GameReference.World())
		GameReference.World()->HandleEvent(sdl_event, event_context);
}

GameData GameReference;
