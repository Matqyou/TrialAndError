//
// Created by Matq on 22/03/2023.
//

#include "GameWorld.h"
#include <client/game/entities/cartesian/characters/character/Character.h>
#include <client/game/entities/cartesian/characters/CharacterNPC.h>
#include "client/game/entities/cartesian/base/Entity.h"
#include <client/game/players/Player.h>
#include "client/game/ui/CommonUI.h"

#include <cmath>

GameWorld::GameWorld(int width, int height)
{


//	m_PauseMenu = new PauseMenu();
//	m_LevelUpMenu = nullptr;
	m_Particles = new Particles(this); //
	m_Tiles = new TileMap(32, width, height);
	m_Width = static_cast<float>(m_Tiles->TotalWidth());
	m_Height = 100.0f;
	m_Depth = static_cast<float>(m_Tiles->TotalHeight());
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

	ground_mesh = MeshPresets::CreateGround(1, 100);
	ground_mesh.SetTranslation(Mat4x4::Translation(Vec3f(m_Width / 2.0f, 0.0f, m_Depth / 2.0f)) * Mat4x4::Scale(Vec3f(m_Width, 1.0f, m_Depth)));
	ground_mesh.UpdateColor({ 150, 0, 0, 255 });
	Drawing.QueueUpdate(&ground_mesh);
}

GameWorld::~GameWorld()
{
	delete m_Tiles;
	delete m_Particles;

	for (auto entity : entities)
		delete entity;
	entities.clear();
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

//unsigned int GameWorld::GetNextPlayerIndex() const
//{
//	unsigned int Index = 0;
//
//	while (true)
//	{
//		bool Used = false;
//		for (auto player : players)
//			if (player->GetIndex() == Index)
//				Used = true;
//
//		if (!Used)
//			return Index;
//
//		Index++;
//	}
//}

void GameWorld::AddScore(unsigned int score)
{
	m_Score += score;
	char msg[64];
	std::snprintf(msg, sizeof(msg), "Score: %i", m_Score);
	m_ScoreText->SetText(msg);
	m_ScoreText->FlagForUpdate();
}

//void GameWorld::InitPlayers()
//{
//	auto num_expected_players = GameReference.NumExpectedPlayers();
//	for (int i = 0; i < num_expected_players; i++)
//	{
//		const char *default_username = i == 0 ? "Keyboard" : "Controller";
//
//		auto preferences = GameReference.GetPlayerPreferences(i);
//		auto preferred_class_type = preferences.GetPlayerClassType();
//		auto new_player_class_object = PlayerClass::CreateClass(preferred_class_type);
//
////		auto new_player = new Player default_username, new_player_class_object);
//		auto new_character = new Character(
//			new_player,
//			100.0,
//			Vec2f(32 * 17.5, 32 * 17.5),
//			Vec2f(10, 10),
//			false
//			);
//		AddEntity(new_character, false);
//	}
//}

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

//Player *GameWorld::AddPlayer(Player *player)
//{
//	players.push_back(player);
////	if (!m_FirstPlayer)
////	{
////		m_FirstPlayer = player;
////		m_LastPlayer = player;
////		player->m_Prev = nullptr;
////		player->m_Next = nullptr;
////	}
////	else
////	{
////		m_LastPlayer->m_Next = player;
////		player->m_Prev = m_LastPlayer;
////		m_LastPlayer = player;
////	}
//
//	return player;
//}

// ::RemovePlayer() doesn't reset players Previous and Next player pointers
//void GameWorld::RemovePlayer(Player *player)
//{
//	players.erase(std::remove(players.begin(), players.end(), player), players.end());
//}

// add_next_tick is important, when iterating through entities, any new entity will cause a crash
Entity *GameWorld::AddEntity(Entity *new_entity, bool add_next_tick)
{
	new_entity->world = this;
	if (add_next_tick)
	{
		pending_entities.push_back(new_entity);
		new_entity->pending = true;
	}
	else
	{
		entities.push_back(new_entity);
		entities_by_types[new_entity->GetType()].push_back(new_entity);
		new_entity->pending = false;
	}

	new_entity->entity_added_event.Invoke(this); // connect any custom events to this
	return new_entity;
}

void GameWorld::RemoveEntity(Entity *entity)
{
	if (entity->pending)
	{
		pending_entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());
	}
	else
	{
		entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());

		auto entity_type = entity->GetType();
		auto& entities_with_this_type = entities_by_types[entity_type];
		entities_with_this_type.erase(std::remove(entities_with_this_type.begin(), entities_with_this_type.end(), entity),
									  entities_with_this_type.end());
	}
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

	for (Entity *entity : entities_by_types[ENTITY_CHARACTER])
	{
		auto character = (Character *)entity;
		character->HandleEvent(sdl_event);
	}
}

void GameWorld::TickCamera(double elapsed_seconds)
{
	if (entities_by_types[ENTITY_CHARACTER].empty())
		return;

	for (Entity *entity : entities_by_types[ENTITY_CHARACTER])
	{
		auto character = (Character *)entity;
		if (character->IsNPC()) continue;

		Camera3D& camera = GameReference.GetCamera3D();
		Vec3f camera_pos = camera.GetPosition();
		Vec3f new_camera_pos = character->GetCore().pos - (character->GetCore().vel).NormalizeF() * 50.0f + character->GetDirectionalCore().orientation.GetUp() * 50.0f;
		Vec3f delta = new_camera_pos - camera_pos;

		camera.SetPosition(camera_pos + delta * 0.1f);
		camera.SetRotation(Quaternion::FromDirection(character->GetCore().pos - camera.GetPosition()));

		break;
	}

//	if (entities_by_types[ENTITY_CHARACTER].empty())
//		return;
//
//	bool FirstIteration = true;
//	float min_x, max_x, min_z, max_z;
//	for (Entity *entity : entities_by_types[ENTITY_CHARACTER])
//	{
//		auto character = (Character *)entity;
//		if (character->IsNPC())
//			if (!character->GetCurrentWeapon())
//				continue;
//
//		EntityCore& entity_core = character->GetCore();
//		if (FirstIteration)
//		{
//			FirstIteration = false;
//			min_x = entity_core.pos.x;
//			max_x = entity_core.pos.x;
//			min_z = entity_core.pos.z;
//			max_z = entity_core.pos.z;
//		}
//		else
//		{
//			if (entity_core.pos.x < min_x)
//				min_x = entity_core.pos.x;
//			if (entity_core.pos.x > max_x)
//				max_x = entity_core.pos.x;
//			if (entity_core.pos.z < min_z)
//				min_z = entity_core.pos.z;
//			if (entity_core.pos.z > max_z)
//				max_z = entity_core.pos.z;
//		}
//	}
//
//	if (!FirstIteration)
//	{
//		float camera_x = ((float)max_x + (float)min_x) / 2.0f;
//		float camera_z = ((float)max_z + (float)min_z) / 2.0f;
//
//		Camera3D& camera = GameReference.GetCamera3D();
//		Vec3f old_camera_pos = camera.GetPosition();
//		Vec3f new_camera_pos = Vec3f(
//			old_camera_pos.x + 0.1f * (camera_x - old_camera_pos.x),
//			100.f,
//			old_camera_pos.z + 0.1f * (camera_z - old_camera_pos.z)
//		);
//		camera.SetPosition(new_camera_pos);
//		camera.SetRotation(Quaternion::FromDirection((Vec3f(camera_x, 0, camera_z) - camera.GetPosition())));
//	}
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
	float Depth2 = m_Depth / 2.0f;

	// Boss every 10 rounds
	if (m_Round % 10 == 0)
	{
		float angle = (180.0 + (rand() % 180)) / 180.0 * M_PI;
		Vec3f SpawnPos = Vec3f(Width2 + std::cos(angle) * Width2, 0, Depth2 + std::sin(angle) * Depth2);
		auto new_npc = new CharacterNPC(
			200.0 + m_Round * 10.0,
			SpawnPos,
			Vec3f(0, 0, 0),
			NPC_TURRET,
			true
		);
		AddEntity(new_npc, true);

		ProjectileWeapon *boss_minigun = ProjectileWeapon::CreateWeaponFromWeaponType(WEAPON_MINIGUN);
		new_npc->GiveWeapon(boss_minigun);
	}

	// Wave enemies
	for (int i = 0; i < m_NumEnemiesPerWave; i++)
	{
		float angle = (180.0 + (rand() % 180)) / 180.0 * M_PI;
		Vec3f SpawnPos = Vec3f(Width2 + std::cos(angle) * Width2, 0, Depth2 + std::sin(angle) * Depth2);
		double Health = std::pow(m_Round, 1.0 / 3) * 10.0;
		auto new_npc = new CharacterNPC(
			Health,
			SpawnPos,
			Vec3f(0, 0, 0),
			NPC_TURRET,
			false
		);
		AddEntity(new_npc, true);

		int Weaponizer = rand() % 100;
		if (m_Round >= 15)
		{
			WeaponType give_weapon_type = WEAPON_NONE;
			if (Weaponizer < 10)
				give_weapon_type = WEAPON_GLOCK;
			else if (Weaponizer < 20)
				give_weapon_type = WEAPON_SHOTGUN;
			else if (Weaponizer < 30)
				give_weapon_type = WEAPON_BURST;

			ProjectileWeapon *new_weapon = ProjectileWeapon::CreateWeaponFromWeaponType(give_weapon_type); // todo: probably bad memory leak if we already have one
			new_npc->GiveWeapon(new_weapon);
		}
		else if (m_Round >= 10)
		{
			WeaponType give_weapon_type = WEAPON_NONE;
			if (Weaponizer < 10)
				give_weapon_type = WEAPON_GLOCK;
			else if (Weaponizer < 20)
				give_weapon_type = WEAPON_SHOTGUN;

			ProjectileWeapon *new_weapon = ProjectileWeapon::CreateWeaponFromWeaponType(give_weapon_type);
			new_npc->GiveWeapon(new_weapon);
		}
		else if (m_Round >= 5)
		{
			if (Weaponizer < 10)
				new_npc->GiveWeapon(ProjectileWeapon::CreateWeaponFromWeaponType(WEAPON_GLOCK));
		}
	}
}

void GameWorld::TickEntities(double elapsed_seconds)
{
	// Loop through each entity and tick
	for (Entity *entity : entities)
		entity->Tick(elapsed_seconds);
}

void GameWorld::TickPending()
{
	for (Entity *new_entity : pending_entities)
		AddEntity(new_entity, false);
	pending_entities.clear();
}

void GameWorld::TickDestroy()
{
	std::vector<Entity *> to_delete;
	for (Entity *entity : entities)
		if (!entity->IsAlive())
			to_delete.push_back(entity);

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

	Vec2f mouse_poition = ApplicationClass::GetMousePosition();
	for (int i = m_Stars.size() - 1; i >= 0; --i)
	{
		auto& [position, velocity, duration] = m_Stars[i];

		Vec2f direction = position - mouse_poition;
		float distance = direction.LengthF();
		velocity += direction.Normalize() / distance * 0.15;
		velocity.x += (sinf((position.x + position.y * 2) / 50.0f)) * 0.0015f;

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
	TickPending();
	TickDestroy();

	m_Particles->Tick();
	TickCamera(elapsed_seconds);
	TickBackground(elapsed_seconds);

	m_CurrentTick++;
}

void GameWorld::Draw()
{
//	auto drawing = Application.GetDrawing();
//
//	drawing->SetColor(0, 0, 50, 255);
//	drawing->Clear();
//
//	drawing->SetColor(200, 200, 200, 255);
//	for (int i = m_Stars.size() - 1; i >= 0; --i)
//	{
//		auto& [position, velocity, duration] = m_Stars[i];
//
//		auto size = (int)duration / 750.0;
//		for (int j = 0; j < size; j++)
//		{
//			for (int k = 0; k < size; k++)
//			{
//				int draw_x = (int)(position.x - size / 2 + j);
//				int draw_y = (int)(position.y - size / 2 + k);
//
//				SDL_RenderPoint(drawing->Renderer(), draw_x, draw_y);
//			}
//		}
//	}
	// Stop drawing when the game has been triggered as over
	if (!m_GameOver)
	{
//		SDL_FRect DrawRect = { 0, 0, m_Width, m_Height };
//		drawing->SetColor(100, 100, 100, 255);
//		drawing->DrawRect(DrawRect, false, GameReference.GetCamera());

		ground_mesh.Draw();
		m_Particles->Draw();
		for (auto& entities_by_type : entities_by_types)
			for (Entity *entity : entities_by_type)
				entity->Draw();
//		m_Tiles->Draw();
	}
//
//	// Draw the score value
//	Texture *ScoreTexture = m_ScoreText->RequestUpdate();
//	float ScoreWidth = ScoreTexture->GetWidth() * 2.5f;
//	float ScoreHeight = ScoreTexture->GetHeight() * 2.5f;
//	SDL_FRect ScoreRect = { 0, Application.GetHeight() - ScoreHeight, ScoreWidth, ScoreHeight };
//	if (!m_GameOver)
//	{
//		drawing->RenderTexture(ScoreTexture->SDLTexture(), nullptr, ScoreRect);
//	}
//	else
//	{
//		// Render a semi-opaque dark overlay
//		drawing->SetColor(0, 0, 0, 200);
//		SDL_FRect full = { 0, 0, (float)Application.GetWidth(), (float)Application.GetHeight() };
//		SDL_RenderFillRect(drawing->Renderer(), &full);
//
//		// Panel dimensions
//		float pw = (Application.GetWidth() * 0.6);
//		float ph = (Application.GetHeight() * 0.6);
//		float px = (Application.GetWidth() - pw) / 2;
//		float py = (Application.GetHeight() - ph) / 2;
//		m_DeathPanelRect = { px, py, pw, ph };
//
//		// Panel background
//		drawing->SetColor(20, 20, 30, 230);
//		drawing->DrawRect(m_DeathPanelRect, true);
////		SDL_RenderFillRect(renderer, &m_DeathPanelRect);
//
//		// Title: You Died
//		drawing->SetColor(220, 40, 40, 255);
//		TextSurface titleTex(CommonUI::fontDefault, "You Died", { 220, 40, 40 });
//		Texture *tTex = titleTex.RequestUpdate();
//		float tw = tTex->GetWidth() * 3.0f;
//		float th = tTex->GetHeight() * 3.0f;
//		SDL_FRect titleRect = { px + (pw - tw) / 2.0f, py + 20, tw, th };
//		drawing->RenderTexture(tTex->SDLTexture(), nullptr, titleRect);
//
//		// Stats: Score + Playtime
//		drawing->SetColor(200, 200, 200, 255);
//		char buf[256];
//		std::snprintf(buf, sizeof(buf), "Score: %u", m_Score);
//		TextSurface scoreLine(CommonUI::fontDefault, buf, { 200, 200, 200 });
//		Texture *sLineTex = scoreLine.RequestUpdate();
//		SDL_FRect sRect = { px + 40, py + 100, sLineTex->GetWidth() * 2.0f, sLineTex->GetHeight() * 2.0f };
//		drawing->RenderTexture(sLineTex->SDLTexture(), nullptr, sRect);
//
//		// Playtime: if Clock exists
//		double seconds = 0.0;
////		if (m_GameWindow->Timer())
//		seconds = (double)m_CurrentTick / std::max(1.0, (double)Application.GetClock()->GetFramerate());
//		int mins = (int)seconds / 60;
//		int secs = (int)seconds % 60;
//		std::snprintf(buf, sizeof(buf), "Playtime: %02d:%02d", mins, secs);
//		TextSurface timeLine(CommonUI::fontDefault, buf, { 200, 200, 200 });
//		Texture *tLineTex = timeLine.RequestUpdate();
//		SDL_FRect tRect = { px + 40, py + 140, tLineTex->GetWidth() * 2.0f, tLineTex->GetHeight() * 2.0f };
//		drawing->RenderTexture(tLineTex->SDLTexture(), nullptr, tRect);
//
//		// Additional stats could go here (kills, accuracy, etc.) if you track them.
//
//		// Back to Menu button
//		float buttonWidth = 300.0f;
//		float buttonHeight = 80.0f;
//		float buttonX = px + (pw - buttonWidth) / 2.0f;
//		float buttonY = py + ph - buttonHeight - 40.0f;
//		m_DeathBackButtonRect = { buttonX, buttonY, buttonWidth, buttonHeight };
//
//		// Button background
//		if (m_DeathBackHover)
//			drawing->SetColor(100, 200, 255, 255);
//		else
//			drawing->SetColor(80, 180, 230, 255);
//		drawing->DrawRect(m_DeathBackButtonRect, true);
////		SDL_RenderFillRect(renderer, &m_DeathBackButtonRect);
//
//		// Button text
//		TextSurface backTextSurface(CommonUI::fontDefault, "Back to Menu", { 10, 10, 10 });
//		Texture *buttonTexture = backTextSurface.RequestUpdate();
//		SDL_FRect buttonTextRect = {
//			buttonX + (buttonWidth - buttonTexture->GetWidth() * 1.5f) / 2.0f,
//			buttonY + (buttonHeight - buttonTexture->GetHeight() * 1.5f) / 2.0f,
//			buttonTexture->GetWidth() * 1.5f, buttonTexture->GetHeight() * 1.5f
//		};
//		drawing->RenderTexture(buttonTexture->SDLTexture(), nullptr, buttonTextRect);
//	}
}
