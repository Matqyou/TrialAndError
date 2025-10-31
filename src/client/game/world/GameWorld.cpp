//
// Created by Matq on 22/03/2023.
//

#include "GameWorld.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/entities/cartesian/characters/CharacterNPC.h>
#include <client/game/world/planetary/PlanetaryGameWorld.h>
#include <client/game/entities/cartesian/Entity.h>
#include <client/game/players/Player.h>
#include "client/game/ui/CommonUI.h"

#include <cmath>

GameWorld::GameWorld(int width, int height)
{
//	m_PauseMenu = new PauseMenu();
//	m_LevelUpMenu = nullptr;
	m_Particles = new Particles(this); //
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
		auto new_character = new Character(
			this,
			new_player,
			100.0,
			Vec2f(32 * 17.5, 32 * 17.5),
			Vec2f(10, 10),
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
			m_Width = static_cast<float>(m_Tiles->TotalWidth());
			m_Height = static_cast<float>(m_Tiles->TotalHeight());
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
	float minX, maxX, minY, maxY;

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
			minX = entity_core.pos.x;
			maxX = entity_core.pos.x;
			minY = entity_core.pos.y;
			maxY = entity_core.pos.y;
		}
		else
		{
			if (entity_core.pos.x < minX)
				minX = entity_core.pos.x;
			if (entity_core.pos.x > maxX)
				maxX = entity_core.pos.x;
			if (entity_core.pos.y < minY)
				minY = entity_core.pos.y;
			if (entity_core.pos.y > maxY)
				maxY = entity_core.pos.y;
		}
	}

	if (!FirstIteration)
	{

		float CameraX = ((float)maxX + (float)minX) / 2.0f;
		float CameraY = ((float)maxY + (float)minY) / 2.0f;
		float ZoomX = static_cast<float>(Application.GetWidth()) / (maxX - minX + 500);
		float ZoomY = static_cast<float>(Application.GetHeight()) / (maxY - minY + 500);
		float Zoom = std::min(ZoomX, ZoomY);

		auto& camera = GameReference.GetCamera();
		auto old_camera_pos = camera.GetPos();
		auto new_camera_pos = Vec2f(old_camera_pos.x + 0.1f * (CameraX - old_camera_pos.x),
									old_camera_pos.y + 0.1f * (CameraY - old_camera_pos.y));
		camera.SetPos(new_camera_pos);

		float old_camera_zoom = camera.GetZoom();
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

	float Width2 = m_Width / 2.0f;
	float Height2 = m_Height / 2.0f;

	// Boss every 10 rounds
	if (m_Round % 10 == 0)
	{
		float angle = (180.0 + (rand() % 180)) / 180.0 * M_PI;
		Vec2f SpawnPos = Vec2f(Width2 + std::cos(angle) * Width2, Height2 + std::sin(angle) * Height2);
		auto NewNPC = new CharacterNPC(
			this,
			200.0 + m_Round * 10.0,
			SpawnPos,
			Vec2f(0.0, 0.0),
			NPC_TURRET,
			true
		);
		NewNPC->GiveWeapon(new WeaponMinigun(nullptr));
	}

	// Wave enemies
	for (int i = 0; i < m_NumEnemiesPerWave; i++)
	{
		float angle = (180.0 + (rand() % 180)) / 180.0 * M_PI;
		Vec2f SpawnPos = Vec2f(Width2 + std::cos(angle) * Width2, Height2 + std::sin(angle) * Height2);
		double Health = std::pow(m_Round, 1.0 / 3) * 10.0;
		auto NewNPC = new CharacterNPC(
			this,
			Health,
			SpawnPos,
			Vec2f(0.0, 0.0),
			NPC_TURRET,
			false
		);

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

void GameWorld::TickDestroy(double seconds_elapsed)
{
	std::vector<Entity *> to_delete;
	for (Entity *entity : entities)
	{
		if (!entity->IsAlive())
			to_delete.push_back(entity);
	}

	// Delete them (this will call RemoveEntity for each)
	for (Entity *entity : to_delete)
		delete entity;
}

void GameWorld::TickBackground(double seconds_elapsed)
{
	for (int i = 0; i < 3; i++)
	{
		auto random_position = Vec2f(rand() % Application.GetWidth(), rand() % Application.GetHeight());
		float duration = 1500.0f;
		m_Stars.emplace_back(random_position, Vec2f(0.0, 0.0), duration);
	}

	Vec2f mouse_poition = Application.GetMousePosition();
	for (int i = m_Stars.size() - 1; i >= 0; --i)
	{
		auto& [position, velocity, duration] = m_Stars[i];

		Vec2f direction = position - mouse_poition;
		double distance = direction.Length();
		velocity += direction.Normalize() / distance * 0.15;
		velocity.x += (sin((position.x + position.y * 2) / 50)) * 0.0015;

		velocity *= 0.98;

		position.x += velocity.x;
		position.y += velocity.y;
		if (position.x > Application.GetWidth())
			position.x -= Application.GetWidth();
		if (position.y > Application.GetHeight())
			position.y -= Application.GetHeight();

		duration -= 1;
		if (duration <= 0.0)
			m_Stars.erase(m_Stars.begin() + i);
	}
}

void GameWorld::Tick(double elapsed_seconds)
{
	if (m_Paused || m_GameOver)
		return;

	if (!m_ShowNames)
		m_ShowNamesVisibility *= 0.98;

	TickSpawner(elapsed_seconds);
	TickEntities(elapsed_seconds);
	TickDestroy(elapsed_seconds);

	m_Particles->Tick();
	TickCamera(elapsed_seconds);
	TickBackground(elapsed_seconds);

	m_CurrentTick++;
}

void GameWorld::Draw()
{
	auto drawing = Application.GetDrawing();

	drawing->SetColor(0, 0, 50, 255);
	drawing->Clear();

	drawing->SetColor(200, 200, 200, 255);
	for (int i = m_Stars.size() - 1; i >= 0; --i)
	{
		auto& [position, velocity, duration] = m_Stars[i];

		auto size = (int)duration / 750.0;
		for (int j = 0; j < size; j++)
		{
			for (int k = 0; k < size; k++)
			{
				int draw_x = (int)(position.x - size / 2 + j);
				int draw_y = (int)(position.y - size / 2 + k);

				SDL_RenderPoint(drawing->Renderer(), draw_x, draw_y);
			}
		}
	}
	// Stop drawing when the game has been triggered as over
	if (!m_GameOver)
	{
		SDL_FRect DrawRect = { 0, 0, m_Width, m_Height };
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
	float ScoreWidth = ScoreTexture->GetWidth() * 2.5f;
	float ScoreHeight = ScoreTexture->GetHeight() * 2.5f;
	SDL_FRect ScoreRect = { 0, Application.GetHeight() - ScoreHeight, ScoreWidth, ScoreHeight };
	if (!m_GameOver)
	{
		drawing->RenderTexture(ScoreTexture->SDLTexture(), nullptr, ScoreRect);
	}
	else
	{
		// Render a semi-opaque dark overlay
		drawing->SetColor(0, 0, 0, 200);
		SDL_FRect full = { 0, 0, (float)Application.GetWidth(), (float)Application.GetHeight() };
		SDL_RenderFillRect(drawing->Renderer(), &full);

		// Panel dimensions
		float pw = (Application.GetWidth() * 0.6);
		float ph = (Application.GetHeight() * 0.6);
		float px = (Application.GetWidth() - pw) / 2;
		float py = (Application.GetHeight() - ph) / 2;
		m_DeathPanelRect = { px, py, pw, ph };

		// Panel background
		drawing->SetColor(20, 20, 30, 230);
		drawing->DrawRect(m_DeathPanelRect, true);
//		SDL_RenderFillRect(renderer, &m_DeathPanelRect);

		// Title: You Died
		drawing->SetColor(220, 40, 40, 255);
		TextSurface titleTex(CommonUI::fontDefault, "You Died", { 220, 40, 40 });
		Texture *tTex = titleTex.RequestUpdate();
		float tw = tTex->GetWidth() * 3.0f;
		float th = tTex->GetHeight() * 3.0f;
		SDL_FRect titleRect = { px + (pw - tw) / 2.0f, py + 20, tw, th };
		drawing->RenderTexture(tTex->SDLTexture(), nullptr, titleRect);

		// Stats: Score + Playtime
		drawing->SetColor(200, 200, 200, 255);
		char buf[256];
		std::snprintf(buf, sizeof(buf), "Score: %u", m_Score);
		TextSurface scoreLine(CommonUI::fontDefault, buf, { 200, 200, 200 });
		Texture *sLineTex = scoreLine.RequestUpdate();
		SDL_FRect sRect = { px + 40, py + 100, sLineTex->GetWidth() * 2.0f, sLineTex->GetHeight() * 2.0f };
		drawing->RenderTexture(sLineTex->SDLTexture(), nullptr, sRect);

		// Playtime: if Clock exists
		double seconds = 0.0;
//		if (m_GameWindow->Timer())
		seconds = (double)m_CurrentTick / std::max(1.0, (double)Application.GetClock()->GetFramerate());
		int mins = (int)seconds / 60;
		int secs = (int)seconds % 60;
		std::snprintf(buf, sizeof(buf), "Playtime: %02d:%02d", mins, secs);
		TextSurface timeLine(CommonUI::fontDefault, buf, { 200, 200, 200 });
		Texture *tLineTex = timeLine.RequestUpdate();
		SDL_FRect tRect = { px + 40, py + 140, tLineTex->GetWidth() * 2.0f, tLineTex->GetHeight() * 2.0f };
		drawing->RenderTexture(tLineTex->SDLTexture(), nullptr, tRect);

		// Additional stats could go here (kills, accuracy, etc.) if you track them.

		// Back to Menu button
		float buttonWidth = 300.0f;
		float buttonHeight = 80.0f;
		float buttonX = px + (pw - buttonWidth) / 2.0f;
		float buttonY = py + ph - buttonHeight - 40.0f;
		m_DeathBackButtonRect = { buttonX, buttonY, buttonWidth, buttonHeight };

		// Button background
		if (m_DeathBackHover)
			drawing->SetColor(100, 200, 255, 255);
		else
			drawing->SetColor(80, 180, 230, 255);
		drawing->DrawRect(m_DeathBackButtonRect, true);
//		SDL_RenderFillRect(renderer, &m_DeathBackButtonRect);

		// Button text
		TextSurface backTextSurface(CommonUI::fontDefault, "Back to Menu", { 10, 10, 10 });
		Texture *buttonTexture = backTextSurface.RequestUpdate();
		SDL_FRect buttonTextRect = {
			buttonX + (buttonWidth - buttonTexture->GetWidth() * 1.5f) / 2.0f,
			buttonY + (buttonHeight - buttonTexture->GetHeight() * 1.5f) / 2.0f,
			buttonTexture->GetWidth() * 1.5f, buttonTexture->GetHeight() * 1.5f
		};
		drawing->RenderTexture(buttonTexture->SDLTexture(), nullptr, buttonTextRect);
	}
}
