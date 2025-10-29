//
// Created by Matq on 22/03/2023.
//

#include "GameWorld.h"
#include "entities/characters/character/Character.h"
#include "entities/characters/CharacterNPC.h"
#include "client/game/ui/CommonUI.h"
#include "entities/Entity.h"
#include "Player.h"
#include <cmath>

GameWorld::GameWorld(int width, int height)
{
//	m_PauseMenu = new PauseMenu();
//	m_LevelUpMenu = nullptr;
	m_Particles = new Particles(); //
	m_Tiles = new TileMap(32, width, height);
	m_Width = m_Tiles->TotalWidth();
	m_Height = m_Tiles->TotalHeight();
	m_ShowNamesVisibility = 0.0;
	m_ShowNames = false;
	m_Paused = false;
	m_GameOver = false;
	m_CurrentTick = 0;

	m_TestingMode = false;

//	m_First = nullptr;
//	m_Last = nullptr;
//	memset(m_FirstType, 0, sizeof(m_FirstType));
//	memset(m_LastType, 0, sizeof(m_LastType));
//	m_FirstPlayer = nullptr;
//	m_LastPlayer = nullptr;

	background_texture = Assets.GetTexture("backgrounds.background_pattern");
	m_BackgroundW = (int)background_texture->GetWidth();
	m_BackgroundH = (int)background_texture->GetHeight();

	m_LastWave = 0;
	m_TimeBetweenWaves = 300;
	m_NumEnemiesPerWave = 1;
	m_Round = 0;
	m_Score = 0;
	m_ScoreText = new TextSurface(CommonUI::fontDefault, "Score: 0", { 150, 150, 0 });
}

GameWorld::~GameWorld()
{
	delete m_Tiles;
	delete m_Particles;

	for (auto player : players)
		delete player;
	for (auto entity : entities)
		delete entity;
}

void GameWorld::EnemyKilled(Player *player, Character *enemy)
{
	// Add score for killing the enemy
	AddScore(100); // Example score value

	// Check if the enemy is a boss and add more score
	// if (enemy->IsBoss()) {
	//     AddScore(500); // Example boss score value
	// }

	// Grant XP to the player
	player->GainXP(10); // Example XP reward
}

void GameWorld::CheckLevelUps()
{
//	for (auto player = GameReference.World()->FirstPlayer(); player != nullptr; player = player->Next())
//	{
//		std::queue<LevelUpMenu *> playerQueue = player->GetLevelUpMenuQueue();
//		if (!playerQueue.empty())
//		{
//			m_LevelUpMenu = (playerQueue.front());
//			playerQueue.pop();
//			player->SetLevelUpMenuQueue(playerQueue);
//		}
//		else
//			m_LevelUpMenu = nullptr;
//	}
//
//	if (m_LevelUpMenu != nullptr)
//		m_LevelUpMenu->Show();
}

unsigned int GameWorld::GetNextPlayerIndex() const
{
	unsigned int Index = 0;
	while (true)
	{
		bool Used = false;
		for (auto player : players)
			if (player->GetIndex() == Index)
				Used = true;

		if (!Used)
			return Index;

		Index++;
	}
}

void GameWorld::AddScore(unsigned int score)
{
	m_Score += score;
	char msg[64];
	std::snprintf(msg, sizeof(msg), "Score: %i", m_Score);
	m_ScoreText->SetText(msg);
	m_ScoreText->FlagForUpdate();
}

void GameWorld::InitPlayers()
{
	auto num_expected_players = GameReference.NumExpectedPlayers();
	for (int i = 0; i < num_expected_players; i++)
	{
		const char *default_username = i == 0 ? "Keyboard" : "Controller";

		auto preferences = GameReference.GetPlayerPreferences(i);
		auto preferred_class_type = preferences.GetPlayerClassType();
		auto new_player_class_object = PlayerClass::CreateClass(preferred_class_type);

		auto new_player = new Player(this, default_username, new_player_class_object);
		auto new_character = new Character(this, new_player, 100.0,
										   Vec2d(32 * 17.5, 32 * 17.5), Vec2d(10, 10),
										   false);
	}
}

void GameWorld::EnemiesKilled()
{
	if (m_TimeBetweenWaves - (m_CurrentTick - m_LastWave) > 300)
		m_LastWave = m_CurrentTick - m_TimeBetweenWaves + 300;
}

void GameWorld::AlliesGone()
{
	m_GameOver = true;
	char msg[64];
	std::snprintf(msg, sizeof(msg), "%i", m_Score);
	m_ScoreText->SetText(msg);
	m_ScoreText->FlagForUpdate();
}

Player *GameWorld::AddPlayer(Player *player)
{
	players.push_back(player);
//	if (!m_FirstPlayer)
//	{
//		m_FirstPlayer = player;
//		m_LastPlayer = player;
//		player->m_Prev = nullptr;
//		player->m_Next = nullptr;
//	}
//	else
//	{
//		m_LastPlayer->m_Next = player;
//		player->m_Prev = m_LastPlayer;
//		m_LastPlayer = player;
//	}

	return player;
}

// ::RemovePlayer() doesn't reset players Previous and Next player pointers
void GameWorld::RemovePlayer(Player *player)
{
	players.erase(std::remove(players.begin(), players.end(), player), players.end());
}

Entity *GameWorld::AddEntity(Entity *entity)
{
	entities.push_back(entity);

	auto entity_type = entity->GetType();
	entities_by_types[entity_type].push_back(entity);

	return entity;
}

// ::RemoveEntity() doesn't reset entities Previous and Next entity pointers
void GameWorld::RemoveEntity(Entity *entity)
{
	entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());

	auto entity_type = entity->GetType();
	auto& entities_with_this_type = entities_by_types[entity_type];
	entities_with_this_type.erase(std::remove(entities_with_this_type.begin(), entities_with_this_type.end(), entity),
								  entities_with_this_type.end());
}

void GameWorld::ToggleShowNames()
{
	m_ShowNames = !m_ShowNames;
	if (m_ShowNames)
		m_ShowNamesVisibility = 1.0;
}

void GameWorld::HandleEvent(const SDL_Event& sdl_event, EventContext& event_context)
{
	if (m_Paused)
		return;

	if (sdl_event.type == SDL_EVENT_KEY_DOWN)
	{
		if (sdl_event.key.scancode == SDL_SCANCODE_SPACE)
			ToggleShowNames();
		else if (sdl_event.key.scancode == SDL_SCANCODE_O)
		{
			m_Tiles->LoadTilemap("assets/tilemaps/test_level");
			m_Width = m_Tiles->TotalWidth();
			m_Height = m_Tiles->TotalHeight();
		}
		else if (sdl_event.key.scancode == SDL_SCANCODE_P)
			m_Tiles->SaveTilemap("assets/tilemaps/test_level");
	}

	for (Entity *entity : entities_by_types[CHARACTER_ENTITY])
	{
		auto character = (Character *)entity;
		character->HandleEvent(sdl_event);
	}
}

void GameWorld::TickCamera(double elapsed_seconds)
{
	if (entities_by_types[CHARACTER_ENTITY].empty())
		return;

	bool FirstIteration = true;
	double minX, maxX, minY, maxY;

	for (Entity *entity : entities_by_types[CHARACTER_ENTITY])
	{
		auto character = (Character *)entity;
		if (character->IsNPC())
			if (!character->GetCurrentWeapon())
				continue;

		EntityCore& entity_core = character->GetCore();
		if (FirstIteration)
		{
			FirstIteration = false;
			minX = entity_core.Pos.x;
			maxX = entity_core.Pos.x;
			minY = entity_core.Pos.y;
			maxY = entity_core.Pos.y;
		}
		else
		{
			if (entity_core.Pos.x < minX)
				minX = entity_core.Pos.x;
			if (entity_core.Pos.x > maxX)
				maxX = entity_core.Pos.x;
			if (entity_core.Pos.y < minY)
				minY = entity_core.Pos.y;
			if (entity_core.Pos.y > maxY)
				maxY = entity_core.Pos.y;
		}
	}

	if (!FirstIteration)
	{

		float CameraX = ((float)maxX + (float)minX) / 2.0f;
		float CameraY = ((float)maxY + (float)minY) / 2.0f;
		double ZoomX = Application.GetWidth() / (maxX - minX + 500);
		double ZoomY = Application.GetHeight() / (maxY - minY + 500);
		double Zoom = std::min(ZoomX, ZoomY);

		auto& camera = GameReference.GetCamera();
		auto old_camera_pos = camera.GetPos();
		auto new_camera_pos = Vec2f(old_camera_pos.x + 0.1f * (CameraX - old_camera_pos.x),
									old_camera_pos.y + 0.1f * (CameraY - old_camera_pos.y));
		camera.SetPos(new_camera_pos);

		double old_camera_zoom = camera.GetZoom();
		camera.SetZoom(old_camera_zoom + 0.1 * (Zoom - old_camera_zoom));
	}
}

void GameWorld::TickSpawner(double elapsed_seconds)
{
	if (m_TestingMode || m_CurrentTick - m_LastWave < m_TimeBetweenWaves)
		return;

	m_LastWave = m_CurrentTick;
	m_Round += 1;
	m_NumEnemiesPerWave = (unsigned int)(1.0 + std::pow(m_Round, 0.5) * 2);
	m_TimeBetweenWaves = (unsigned long long)((m_Round * m_NumEnemiesPerWave) * Application.GetClock()->GetFramerate());
	m_Score += m_Round * 50;
	char msg[64];
	std::snprintf(msg, sizeof(msg), "Score: %i", m_Score);
	m_ScoreText->SetText(msg);
	m_ScoreText->FlagForUpdate();

	double Width2 = m_Width / 2.0;
	double Height2 = m_Height / 2.0;

	// Boss every 10 rounds
	if (m_Round % 10 == 0)
	{
		double Angle = (180.0 + (rand() % 180)) / 180.0 * M_PI;
		Vec2d SpawnPos = Vec2d(Width2 + std::cos(Angle) * Width2, Height2 + std::sin(Angle) * Height2);
		auto NewNPC = new CharacterNPC(this,
									   200.0 + m_Round * 10.0,
									   SpawnPos,
									   Vec2d(0.0, 0.0),
									   NPC_TURRET,
									   true);
		NewNPC->GiveWeapon(new WeaponMinigun(nullptr));
	}

	// Wave enemies
	for (int i = 0; i < m_NumEnemiesPerWave; i++)
	{
		double Angle = (180.0 + (rand() % 180)) / 180.0 * M_PI;
		Vec2d SpawnPos = Vec2d(Width2 + std::cos(Angle) * Width2, Height2 + std::sin(Angle) * Height2);
		double Health = std::pow(m_Round, 1.0 / 3) * 10.0;
		auto NewNPC = new CharacterNPC(this,
									   Health,
									   SpawnPos,
									   Vec2d(0.0, 0.0),
									   NPC_TURRET,
									   false);

		int Weaponizer = rand() % 100;
		if (m_Round >= 15)
		{
			if (Weaponizer < 10)
				NewNPC->GiveWeapon(new WeaponGlock(nullptr));
			else if (Weaponizer < 20)
				NewNPC->GiveWeapon(new WeaponShotgun(nullptr));
			else if (Weaponizer < 30)
				NewNPC->GiveWeapon(new WeaponBurst(nullptr));
		}
		else if (m_Round >= 10)
		{
			if (Weaponizer < 10)
				NewNPC->GiveWeapon(new WeaponGlock(nullptr));
			else if (Weaponizer < 20)
				NewNPC->GiveWeapon(new WeaponShotgun(nullptr));
		}
		else if (m_Round >= 5)
		{
			if (Weaponizer < 10)
				NewNPC->GiveWeapon(new WeaponGlock(nullptr));
		}
	}
}

void GameWorld::TickEntities(double elapsed_seconds)
{
	// Loop through each entity and tick
	for (Entity *entity : entities)
		entity->Tick(elapsed_seconds);
}

void GameWorld::TickDestroy()
{
	std::vector<Entity*> to_delete;
	for (Entity* entity : entities)
	{
		if (!entity->IsAlive())
			to_delete.push_back(entity);
	}

	// Delete them (this will call RemoveEntity for each)
	for (Entity* entity : to_delete)
		delete entity;
}
void GameWorld::Tick(double elapsed_seconds)
{
	if (m_Paused || m_GameOver)
		return;

	if (!m_ShowNames)
		m_ShowNamesVisibility *= 0.98;

	TickSpawner(elapsed_seconds);
	TickEntities(elapsed_seconds);
	TickDestroy();

	m_Particles->Tick();
	TickCamera(elapsed_seconds);

	m_CurrentTick++;
}

void GameWorld::Draw()
{
	auto drawing = Application.GetDrawing();

	// Stop drawing when the game has been triggered as over
	if (!m_GameOver)
	{
		SDL_FRect DestinationRect = { 0, 0, float(m_Width), float(m_Height) };
		drawing->RenderTexture(background_texture->SDLTexture(), nullptr, DestinationRect, GameReference.GetCamera());

		SDL_FRect DrawRect = { 0, 0, float(m_Width), float(m_Height) };
		drawing->SetColor(100, 100, 100, 255);
		drawing->DrawRect(DrawRect, false, GameReference.GetCamera());

		m_Particles->Draw();
		for (auto& entities_by_type : entities_by_types)
			for (Entity *entity : entities_by_type)
				entity->Draw();
//		m_Tiles->Draw();
	}

	// Draw the score value
	Texture *ScoreTexture = m_ScoreText->RequestUpdate();
	auto ScoreWidth = (float)(ScoreTexture->GetWidth() * 2.5);
	auto ScoreHeight = (float)(ScoreTexture->GetHeight() * 2.5);
	SDL_FRect ScoreRect = { 0, (float)(Application.GetHeight()) - ScoreHeight, ScoreWidth, ScoreHeight };
	if (!m_GameOver)
		drawing->RenderTexture(ScoreTexture->SDLTexture(), nullptr, ScoreRect);
	else
		drawing->RenderTextureFullscreen(ScoreTexture->SDLTexture(), nullptr);
}
