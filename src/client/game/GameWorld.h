//
// Created by Matq on 22/03/2023.
//

#pragma once

#include "client/GameData.h"
#include "client/core/GameControllers.h"
#include "indicators/TextSurface.h"
#include "collision/TileMap.h"
#include "shared/Protocol.h"
#include "../ui/Particles.h"
#include "./interface/PauseMenu.h"
#include "shared/utility/Vec2.h"
#include <chrono>

class Player;
class Entity;
class Character;
class Crate;
class PauseMenu;
class LevelUpMenu;

class GameWorld
{
protected:
	GameData *m_GameWindow;
	PauseMenu *m_PauseMenu;
	LevelUpMenu *m_LevelUpMenu;
	TileMap *m_Tiles;
	Particles *m_Particles;
	double m_Width, m_Height;
	double m_ShowNamesVisibility;
	bool m_ShowNames;
	bool m_Paused;
	bool m_GameOver;
	bool m_LevelUpDelay;
	Player *m_FirstPlayer, *m_LastPlayer;
	Entity *m_FirstType[NUM_ENTITY_TYPES]{}, *m_LastType[NUM_ENTITY_TYPES]{};
	Entity *m_First, *m_Last;
	unsigned long long m_CurrentTick;
	std::vector<std::tuple<Vec2d, Vec2d, double>> m_Stars;

	bool m_TestingMode; // Waves paused

	// Cool scrolling background                                        cap
	Texture *m_Background;
	int m_BackgroundW{}, m_BackgroundH{};

	// Death screen UI
	SDL_Rect m_DeathPanelRect{};
	SDL_Rect m_DeathBackButtonRect{};
	bool m_DeathBackHover{false};

	unsigned long long m_LastWave;
	unsigned long long m_TimeBetweenWaves;
	unsigned int m_NumEnemiesPerWave;
	unsigned int m_Round;
	unsigned int m_Score;
	TextSurface *m_ScoreText;

	virtual void TickCamera();
	virtual void TickSpawner();
	virtual void TickEntities();
	virtual void TickDestroy();
	virtual void TickBackground();

public:
	GameWorld(GameData *game_window, int width, int height);
	virtual ~GameWorld();

	// Getting
	[[nodiscard]] GameData *GameWindow() const { return m_GameWindow; }
	[[nodiscard]] PauseMenu *Menu() const { return m_PauseMenu; }
	[[nodiscard]] LevelUpMenu *LvlMenu() const { return m_LevelUpMenu; }
	[[nodiscard]] Particles *GetParticles() const { return m_Particles; };
	[[nodiscard]] double GetWidth() const { return m_Width; }
	[[nodiscard]] double GetHeight() const { return m_Height; }
	[[nodiscard]] double GetNamesShown() const { return m_ShowNamesVisibility < 0.1 ? 0.0 : m_ShowNamesVisibility; }
	[[nodiscard]] bool GameOver() const { return m_GameOver; }
	[[nodiscard]] unsigned long long GetTick() const { return m_CurrentTick; }
	[[nodiscard]] unsigned int GetNextPlayerIndex() const;
	[[nodiscard]] Entity *FirstEntity() const { return m_First; }
	[[nodiscard]] Entity *FirstEntityType(EntityType entity_type) const { return m_FirstType[entity_type]; }
	[[nodiscard]] Player *FirstPlayer() const { return m_FirstPlayer; }
	[[nodiscard]] Character *FirstCharacter() const { return (Character *)(FirstEntityType(CHARACTER_ENTITY)); }
	[[nodiscard]] Crate *FirstCrate() const { return (Crate *)(FirstEntityType(CRATE_ENTITY)); }
	[[nodiscard]] bool GetDelay() const { return m_LevelUpDelay; }
	// void GetPointInWorld(double relative_x, double relative_y, double& out_x, double& out_y) const;

	// Setting
	void SetPaused(bool state) { m_Paused = state; }
	void SetDelay(bool state) { m_LevelUpDelay = state; }

	// Events
	void EnemiesKilled();
	void AlliesGone();
	void EnemyKilled(Player *player, Character *enemy);
	void CheckLevelUps();

	// Manipulating
	void AddScore(unsigned int score);
	void DestroyPlayerByController(GameController *DeletedController) const;
	void DestroyCharacterByController(GameController *DeletedController) const;
	void ToggleShowNames();
	void SetTestingMode(bool testing_mode) { m_TestingMode = testing_mode; }

	// Managing
	Player *AddPlayer(Player *player);
	Entity *AddEntity(Entity *entity);
	void RemovePlayer(Player *player);
	void RemoveEntity(Entity *entity);

	// Listening & Ticking
	virtual void Event(const SDL_Event &currentEvent);
	virtual void Tick();
	virtual void Draw();
};
