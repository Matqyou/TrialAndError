//
// Created by Matq on 22/03/2023.
//

#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

#include <client/core/GameControllers.h>
#include <shared/utility/Randomizer.h>
#include <client/core/AssetsManager.h>
#include <client/game/interface/Interface.h>
#include <client/game/interface/MainMenu.h>
#include <shared/utility/Vec2.h>
#include <client/core/Drawing.h>
#include <client/core/Clock.h>
#include <client/game/world/GameWorld.h>
#include <shared/Protocol.h>
#include <vector>

class MainMenu;
class GameModeMenu;
class PlayerClass;
class ClassSelectMenu;

class GameData
{
private:
	SDL_Window *m_Window;
	SDL_Renderer *m_Renderer;
	SDL_GLContext m_GLContext;
	Clock *m_Timer;
	Drawing *m_Draw;
	Randomizer *m_Random;
	AssetsManager *m_AssetsHandler;
	Interface *m_Interface;

	MainMenu *m_MainMenu;
	GameModeMenu *m_GameModeMenu;
	std::vector<ClassSelectMenu *> m_ClassSelectMenus;
	GameWorld *m_GameWorld;
	GameControllers *m_Controllers;

	std::vector<PlayerClass *> m_PendingPlayerClasses;

	int m_Width, m_Height;
	double m_Width2, m_Height2;
	bool m_InitializedSDL;
	bool m_InitializedMix;
	bool m_InitializedAudio;
	bool m_InitializedImages;
	bool m_InitializedTTF;

	static LoadedSound sQuitSound;

	// Manipulating
	void UpdateDimensions(int width, int height);
	bool InitializeSDL();
	bool InitializeMix();
	bool InitializeAudio();
	bool InitializeImages();
	bool InitializeTTF();

	// Waiting
	void WaitForSoundToFinish();

public:
	GameData();
	~GameData();

	// Getting
	[[nodiscard]] SDL_Window *Window() const { return m_Window; }
	[[nodiscard]] SDL_Renderer *Renderer() const { return m_Renderer; }
	[[nodiscard]] SDL_GLContext& GlContext() { return m_GLContext; }
	[[nodiscard]] Clock *Timer() const { return m_Timer; }
	[[nodiscard]] Randomizer *Random() const { return m_Random; }
	[[nodiscard]] Drawing *Render() const { return m_Draw; }
	[[nodiscard]] Interface *GetInterface() const { return m_Interface; }
	[[nodiscard]] AssetsManager *Assetz() const { return m_AssetsHandler; }
	[[nodiscard]] MainMenu *Menu() const { return m_MainMenu; }
	[[nodiscard]] const std::vector<ClassSelectMenu *>& GetClassMenus() const { return m_ClassSelectMenus; }
	[[nodiscard]] GameModeMenu *GameSelectMenu() const { return m_GameModeMenu; }
	[[nodiscard]] GameWorld *World() const { return m_GameWorld; }
	[[nodiscard]] GameControllers *Controllers() const { return m_Controllers; }
	[[nodiscard]] int GetWidth() const { return m_Width; }
	[[nodiscard]] int GetHeight() const { return m_Height; }
	[[nodiscard]] double GetWidth2() const { return m_Width2; }
	[[nodiscard]] double GetHeight2() const { return m_Height2; }

	// Manipulating
	bool Initialize();
	void Deinitialize(bool play_quit_sound);
	// Spawn any pending player classes into the current world (after SetWorld)
	void SpawnPendingPlayers();
	void SetWorld(GameWorld *world);
	
	void AddPlayerClassMenu();
	void AddPendingClass(PlayerClass *playerClass);
	void ResetPlayerClassMenus();
	void ResetPendingClasses();
	void RemovePlayerClassMenu();

	void StartGame(GameMode mode);
	void InitializeSandbox();
	void InitializeInfinite();
	void InitializeLevelMode(int level);

	// Listening
	void Event(const SDL_Event& event);

};

extern GameData GameReference;