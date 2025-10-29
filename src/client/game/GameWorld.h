//
// Created by Matq on 22/03/2023.
//

#pragma once

#include "client/game/ui/menus/pause/PauseMenu.h"
#include "client/core/GameControllers.h"
#include "client/game/ui/Particles.h"
#include "indicators/TextSurface.h"
#include "collision/TileMap.h"
#include "client/Protocol.h"

class Player;
class Entity;
class Character;
class Crate;
class PauseMenu;
class LevelupMenu;
class GameWorld
{
private:
	TileMap *m_Tiles;
	Particles *m_Particles;
	double m_Width, m_Height;
	double m_ShowNamesVisibility;
	bool m_ShowNames;
	bool m_Paused;
	bool m_GameOver;
	bool m_LevelUpDelay;
	std::vector<Player *> players;
	std::vector<Entity *> entities;
	std::vector<Entity *> entities_by_types[NUM_ENTITY_TYPES];
	unsigned long long m_CurrentTick;

	bool m_TestingMode; // Waves paused

	// Cool scrolling background                                        cap
	Texture *background_texture;
	int m_BackgroundW{ }, m_BackgroundH{ };

	unsigned long long m_LastWave;
	unsigned long long m_TimeBetweenWaves;
	unsigned int m_NumEnemiesPerWave;
	unsigned int m_Round;
	unsigned int m_Score;
	TextSurface *m_ScoreText;

	void TickCamera(double elapsed_seconds);
	void TickSpawner(double elapsed_seconds);
	void TickEntities(double elapsed_seconds);
	void TickDestroy();

public:
	GameWorld(int width, int height);
	~GameWorld();

	// Getting
//	[[nodiscard]] PauseMenu *Menu() const { return m_PauseMenu; }
//	[[nodiscard]] LevelUpMenu *LvlMenu() const { return m_LevelUpMenu; }
	[[nodiscard]] Particles *GetParticles() const { return m_Particles; };
	[[nodiscard]] double GetWidth() const { return m_Width; }
	[[nodiscard]] double GetHeight() const { return m_Height; }
	[[nodiscard]] double GetNamesShown() const { return m_ShowNamesVisibility < 0.1 ? 0.0 : m_ShowNamesVisibility; }
	[[nodiscard]] bool GameOver() const { return m_GameOver; }
	[[nodiscard]] unsigned long long GetTick() const { return m_CurrentTick; }
	[[nodiscard]] unsigned int GetNextPlayerIndex() const;
	[[nodiscard]] std::vector<Player *>& GetPlayers() { return players; }
	[[nodiscard]] std::vector<Entity *>& GetEntities() { return entities; }
	[[nodiscard]] std::vector<Entity *>& GetEntitiesByType(int entity_type) { return entities_by_types[entity_type]; }
	[[nodiscard]] bool GetDelay() const { return m_LevelUpDelay; }
	// void GetPointInWorld(double relative_x, double relative_y, double& out_x, double& out_y) const;

	// Setting
	void InitPlayers();

	void SetPaused(bool state) { m_Paused = state; }
	void SetDelay(bool state) { m_LevelUpDelay = state; }

	// Events
	void EnemiesKilled();
	void AlliesGone();
	void EnemyKilled(Player *player, Character *enemy);
	void CheckLevelUps();

	// Manipulating
	void AddScore(unsigned int score);
//	void DestroyPlayerByController(GameController *DeletedController) const;
//	void DestroyCharacterByController(GameController *DeletedController) const;
	void ToggleShowNames();
	void SetTestingMode(bool testing_mode) { m_TestingMode = testing_mode; }

	// Managing
	Player *AddPlayer(Player *player);
	Entity *AddEntity(Entity *entity);
	void RemovePlayer(Player *player);
	void RemoveEntity(Entity *entity);

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_context);
	void Tick(double elapsed_seconds);
	void Draw();

};
