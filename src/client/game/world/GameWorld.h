//
// Created by Matq on 22/03/2023.
//

#pragma once

#include <client/game/ui/menus/pause/PauseMenu.h>
#include <client/game/indicators/TextSurface.h>
#include <client/game/collision/TileMap.h>
#include <client/core/Gamepads.h>
#include <client/game/ui/Particles.h>
//#include <client/GameData.h>
#include <client/Protocol.h>

#include <shared/utility/Vec2.h>
#include <chrono>

class Player;
class Entity;
class Character;
class Crate;
//class PauseMenu;
//class LevelUpMenu;

class GameWorld
{
protected:
//	PauseMenu *m_PauseMenu;
//	LevelUpMenu *m_LevelUpMenu;
	TileMap *m_Tiles;
	Particles *m_Particles;
	float m_Width, m_Height;
	double m_ShowNamesVisibility;
	bool m_ShowNames;
	bool m_Paused;
	bool m_GameOver;
	bool m_LevelUpDelay;
//	std::vector<Player *> players;
	std::vector<Entity *> pending_entities; // added after current tick
	std::vector<Entity *> entities;
	std::vector<Entity *> entities_by_types[NUM_ENTITY_TYPES];
	unsigned long long m_CurrentTick;
	std::vector<std::tuple<Vec2f, Vec2f, float>> m_Stars;

	bool m_TestingMode; // Waves paused

	// Cool scrolling background                                        cap
	Texture *background_texture;
	int m_BackgroundW{ }, m_BackgroundH{ };

	// Death screen UI
	SDL_FRect m_DeathPanelRect{};
	SDL_FRect m_DeathBackButtonRect{};
	bool m_DeathBackHover{false};

	unsigned long long m_LastWave;
	unsigned long long m_TimeBetweenWaves;
	unsigned int m_NumEnemiesPerWave;
	unsigned int m_Round;
	unsigned int m_Score;
	TextSurface *m_ScoreText;

	virtual void TickCamera(double elapsed_seconds);
	virtual void TickSpawner(double elapsed_seconds);
	virtual void TickEntities(double elapsed_seconds);
	void TickPending();
	void TickDestroy();
	virtual void TickBackground(double seconds_elapsed);

public:
	GameWorld(int width, int height);
	virtual ~GameWorld();

	// Getting
//	[[nodiscard]] PauseMenu *Menu() const { return m_PauseMenu; }
//	[[nodiscard]] LevelUpMenu *LvlMenu() const { return m_LevelUpMenu; }
	[[nodiscard]] Particles *GetParticles() const { return m_Particles; };
	[[nodiscard]] float GetWidth() const { return m_Width; }
	[[nodiscard]] float GetHeight() const { return m_Height; }
	[[nodiscard]] double GetNamesShown() const { return m_ShowNamesVisibility < 0.1 ? 0.0 : m_ShowNamesVisibility; }
	[[nodiscard]] bool GameOver() const { return m_GameOver; }
	[[nodiscard]] unsigned long long GetTick() const { return m_CurrentTick; }
//	[[nodiscard]] std::vector<Player *>& GetPlayers() { return players; }
	[[nodiscard]] std::vector<Entity *>& GetEntities() { return entities; }
	[[nodiscard]] std::vector<Entity *>& GetEntitiesByType(int entity_type) { return entities_by_types[entity_type]; }
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
//	void InitPlayers();
	void AddScore(unsigned int score);
//	void DestroyPlayerByController(GameController *DeletedController) const;
//	void DestroyCharacterByController(GameController *DeletedController) const;
	void ToggleShowNames();
	void SetTestingMode(bool testing_mode) { m_TestingMode = testing_mode; }

	// Managing
	Entity *AddEntity(Entity *new_entity, bool add_next_tick = true);
	void RemoveEntity(Entity *entity);

	// Ticking
	virtual void HandleEvent(const SDL_Event& sdl_event, EventContext& event_context);
	virtual void Tick(double elapsed_seconds);
	virtual void Draw();

};
