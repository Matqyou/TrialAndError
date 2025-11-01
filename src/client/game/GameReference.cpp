//
// Created by Matq on 22/03/2023.
//

#include "GameReference.h"

#include <utility>
#include "client/game/entities/characters/character/Character.h"
#include "client/game/ui/menus/class_select/GamemodeMenu.h"
#include "client/game/entities/item/weapons/EntityGuns.h"
#include "client/game/ui/CommonUI.h"
#include "client/core/Assets.h"

LinkSound sQuitSound("ui.quit");

GameData::GameData()
{
	world = nullptr;
	interface = nullptr;
	exit_callback = { };

	next_player_id = 0;
}

GameData::~GameData()
{
	Deinitialize(false);
}

size_t GameData::NextPlayerID()
{
	return next_player_id++;
}

Player* GameData::GetPlayerFromID(size_t player_id)
{
	for (Player* player : players)
		if (player->GetPlayerID() == player_id)
			return player;

	return nullptr;  // Player not found
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

void GameData::AddPlayer(Player* new_player)
{
	players.push_back(new_player);
}

void GameData::RemovePlayer(Player* player)
{
	players.erase(std::remove(players.begin(), players.end(), player), players.end());
}

void GameData::SetWorld(GameWorld *new_world, bool delete_old)
{
	if (delete_old)
		DeleteWorld();

	world = new_world;
}

void GameData::DeleteWorld()
{
	delete world;
	world = nullptr;
}

bool GameData::Initialize()
{
	interface = new Interface();

	// Creates 1 player preferences object
	AddPlayer(new Player("DefaultPlayer"));
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
	Character::sBotNameplate = new TextSurface(CommonUI::fontDefault, "Bot User", { 255, 150, 150, 255 });

	for (int y = 0; y < 5; y++)
		for (int x = 0; x < 5; x++)
		{
			auto new_crate = new Crate(
				Vec2f(200 + 50 * static_cast<float>(x), 200 + 50 * static_cast<float>(y)),
				DropType(Application.GetRandomizer()->Int() % NUM_DROP_TYPES)
				);
			world->AddEntity(new_crate, false);
		}

	for (int x = 0; x < NUM_ERROR_TYPES; x++)
	{
		auto new_error = new Error(Vec2f(50.0f + 50.0f * static_cast<float>(x), world->GetHeight() - 50.0f), (ErrorType)x);
		world->AddEntity(new_error, false);
	}

	for (int x = 0; x < NUM_AMMO_TYPES; x++)
	{
		auto new_ammo_box = new AmmoBox(AmmoType(x), Vec2f(50.0f + 50.0f * static_cast<float>(x), world->GetHeight() - 100.0f), 1000);
		world->AddEntity(new_ammo_box, false);
	}

	float WeaponsY = world->GetHeight() - 150;
	world->AddEntity(new EntityGlock(nullptr, nullptr, Vec2f(100, WeaponsY)), false);
	world->AddEntity(new EntityShotgun(nullptr, nullptr, Vec2f(200, WeaponsY)), false);
	world->AddEntity(new EntityBurst(nullptr, nullptr, Vec2f(300, WeaponsY)), false);
	world->AddEntity(new EntityMinigun(nullptr, nullptr, Vec2f(400, WeaponsY)), false);
	world->AddEntity(new EntitySniper(nullptr, nullptr, Vec2f(500, WeaponsY)), false);
	world->AddEntity(new EntityPatersonNavy(nullptr, nullptr, Vec2f(600, WeaponsY)), false);

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
	Character::sBotNameplate = new TextSurface(CommonUI::fontDefault, "Bot User", { 255, 150, 150, 255 });

//	world->InitPlayers();
}

void GameData::HandleEvent(const SDL_Event& sdl_event, EventContext& event_context)
{
	if (world) world->HandleEvent(sdl_event, event_context);
}

GameData GameReference;
