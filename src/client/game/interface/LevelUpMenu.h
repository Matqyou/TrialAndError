// src/game/interface/LevelUpMenu.h

#pragma once

#include "client/GameData.h"
#include "../GameWorld.h"
#include "SDL.h"

class Player;
class LevelUpMenu
{
private:
	TTF_Font *m_Font;

	GameData *m_GameWindow;
	GameWorld *m_GameWorld;
	Texture *m_TextureAllStats;
	Texture *m_TextureBombs;
	Texture *m_TextureDoubleDamage;
	Texture *m_TextureBossDamage;
	Texture *m_TextureExplosiveAmmo;
	Texture *m_TextureExtraLives;
	Texture *m_TextureSpeed;
	Texture *m_TextureSpiky;
	Texture *m_TextureHealth;
	Texture *m_TextureInfiniteGlockAmmo;
	Texture *m_TextureErrorOutline;

	bool m_Paused;
	Player *m_Player;
	std::vector<Texture *>
		m_powerupTextures;
	std::vector<Texture *>
		m_specialPowerupTextures;
	std::vector<int> m_selectedIndices;
	SDL_Rect m_ErrorIconRect;
	SDL_Rect m_ErrorOutlineRect;
	int m_HoverIndex{-1};

	// Powerup effect functions
	void ApplyAllStats();
	void ApplyBombs();
	void ApplyDoubleDamage();
	void ApplyBossDamage();
	void ApplyExplosiveAmmo();
	void ApplyExtraLives();
	void ApplySpeed();
	void ApplySpiky();
	void ApplyHealth();
	void ApplyInfiniteGlockAmmo();

public:
	LevelUpMenu(GameWorld *gameWorld, Player *player);
	~LevelUpMenu();

	[[nodiscard]] bool Paused() const { return m_Paused; }
	void Show();
	void HandleEvent(const SDL_Event& event);
	void Render();
};