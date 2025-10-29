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
#include "client/core/drawing/Drawing.h"
#include "client/core/GameControllers.h"
#include "shared/utility/Randomizer.h"
#include "client/core/AssetsManager.h"
#include "client/game/ui/Interface.h"
#include "client/game/Preferences.h"
#include "client/core/Application.h"
#include "client/game/GameWorld.h"
#include "shared/utility/Vec2.h"
#include "client/core/Assets.h"
#include "client/core/Clock.h"
#include "client/Protocol.h"
#include <vector>

class MainMenu;
class PlayerClass;
class GamemodeMenu;
class ClassSelectMenu;
class GameData
{
private:
	Camera camera;
	GameWorld *world;
	Interface *interface;

	// Picked class, etc.
	std::vector<Preferences> player_preferences;

	Callback exit_callback;

public:
	GameData();
	bool Initialize();
	void Deinitialize(bool play_quit_sound);
	~GameData();

	// Getting
	[[nodiscard]] Camera& GetCamera() { return camera; }
	[[nodiscard]] GameWorld *World() const { return world; }
	[[nodiscard]] Interface *GetInterface() const { return interface; }
	[[nodiscard]] Preferences& GetPlayerPreferences(int index) { return player_preferences[index]; }
	[[nodiscard]] size_t NumExpectedPlayers() const { return player_preferences.size(); }

	// Manipulating
	void SetExitApplicationCallback(Callback callback);
	void ExitApplication();

//	void AddPlayerClassMenu();
//	void AddPendingClass(PlayerClass *playerClass);
//	void RemovePlayerClassMenu();
	void DeleteWorld();
	void StartGame(Gamemode mode);
	void InitializeSandbox();
	void InitializeInfinite();
	void InitializeLevelMode(int level);

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_context);

};

extern GameData GameReference;
