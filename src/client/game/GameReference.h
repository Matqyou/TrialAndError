//
// Created by Matq on 22/03/2023.
//

#pragma once

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_mixer/SDL_mixer.h"

#include "client/game/ui/menus/class_select/GamemodeMenu.h"
#include "client/game/ui/menus/levelup/LevelupMenu.h"
#include "client/game/ui/menus/pause/PauseMenu.h"
#include "client/game/ui/menus/main/MainMenu.h"
#include "client/game/world/GameWorld.h"
#include "client/core/drawing/Drawing.h"
#include "client/game/ui/Interface.h"
#include "client/game/Preferences.h"
#include "client/core/Application.h"
#include "client/core/Gamepads.h"
#include "client/core/Assets.h"
#include "client/core/Clock.h"
#include "client/Protocol.h"

#include <shared/utility/Randomizer.h>
#include "shared/math/Vec2.h"
#include <vector>

class MainMenu;
class PlayerClass;
class GamemodeMenu;
class ClassSelectMenu;
class GameData
{
private:
	Camera camera;
	Camera3D camera_3d;
	GameWorld *world;
	Interface *interface;

	// Picked class, etc.
	std::vector<Player*> players;
	std::vector<Preferences> player_preferences;
	int next_player_id;

	Callback exit_callback;

public:
	GameData();
	bool Initialize();
	void Deinitialize(bool play_quit_sound);
	~GameData();

	// Getting
	[[nodiscard]] Camera& GetCamera() { return camera; }
	[[nodiscard]] Camera3D& GetCamera3D() { return camera_3d; }
	[[nodiscard]] GameWorld *World() const { return world; }
	[[nodiscard]] Interface *GetInterface() const { return interface; }
	[[nodiscard]] std::vector<Player*>& GetPlayers() { return players; }
	[[nodiscard]] Preferences& GetPlayerPreferences(int index) { return player_preferences[index]; }
	[[nodiscard]] int NextPlayerID();
	[[nodiscard]] Player* GetPlayerFromID(int player_id);
	[[nodiscard]] int GetAvailableGamepadIndex();

	// Manipulating
	void SetExitApplicationCallback(Callback callback);
	void ExitApplication();

//	void AddPlayerClassMenu();
//	void AddPendingClass(PlayerClass *playerClass);
//	void RemovePlayerClassMenu();
	void AddPlayer(Player* new_player);
	void RemovePlayer(Player* player);
	void SetWorld(GameWorld *new_world, bool delete_old = true);
	void DeleteWorld();
	void StartGame(Gamemode mode);
	void InitializeSandbox();
	void InitializeInfinite();
	void InitializeLevelMode(int level); // ?

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_context);

};

extern GameData GameReference;
