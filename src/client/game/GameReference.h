//
// Created by Matq on 22/03/2023.
//

#pragma once

#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "SDL3_image/SDL_image.h"
#include "SDL3_mixer/SDL_mixer.h"

#include "client/game/ui/menus/class_select/GamemodeMenu.h"
#include "client/game/ui/menus/levelup/LevelUpMenu.h"
#include "client/game/ui/menus/pause/PauseMenu.h"
#include "client/game/ui/menus/main/MainMenu.h"
#include "client/core/drawing/Drawing.h"
#include "client/core/GameControllers.h"
#include "shared/utility/Randomizer.h"
#include "client/core/AssetsManager.h"
#include "client/game/ui/Interface.h"
#include "client/core/Application.h"
#include "shared/utility/Vec2.h"
#include "client/core/Assets.h"
#include "client/core/Clock.h"
#include "client/Protocol.h"
#include "GameWorld.h"
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
	std::vector<PlayerClass *> m_PendingPlayerClasses;

	std::vector<ClassSelectMenu *> m_ClassSelectMenus;
	MainMenu *main_menu;

	PauseMenu *pause_menu;
	LevelUpMenu *levelup_menu;
	GamemodeMenu *gamemode_menu;
	FullscreenMenu *current_menu;

	Callback exit_callback;

public:
	GameData();
	bool Initialize();
	void InitUI(); // After assets have loaded
	void Deinitialize(bool play_quit_sound);
	~GameData();

	// Getting
	[[nodiscard]] Camera& GetCamera() { return camera; }
	[[nodiscard]] GameWorld *World() const { return world; }

	[[nodiscard]] Interface *GetInterface() const { return interface; }

	[[nodiscard]] MainMenu *Menu() { return main_menu; }
	[[nodiscard]] PauseMenu *GetPauseMenu() { return pause_menu; }
	[[nodiscard]] LevelUpMenu *GetLevelupMenu() { return levelup_menu; }
	[[nodiscard]] GamemodeMenu *GameSelectMenu() { return gamemode_menu; }
	[[nodiscard]] const std::vector<ClassSelectMenu *>& GetClassMenus() const { return m_ClassSelectMenus; }

	[[nodiscard]] FullscreenMenu *GetCurrentMenu() const { return current_menu; }

	// Manipulating
	void SetExitApplicationCallback(Callback callback);
	void ExitApplication();

	void AddPlayerClassMenu();
	void AddPendingClass(PlayerClass *playerClass);
	void RemovePlayerClassMenu();

	void SetCurrentMenu(FullscreenMenu *new_menu);

	void DeleteWorld();

	void StartGame(Gamemode mode);
	void InitializeSandbox();
	void InitializeInfinite();
	void InitializeLevelMode(int level);

};

extern GameData GameReference;
