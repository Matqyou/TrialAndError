// src/game/interface/LevelUpMenu.h

#pragma once

#include "client/game/ui/menus/FullscreenMenu.h"
#include "client/game/GameWorld.h"
#include "SDL3/SDL.h"

class Player;
class LevelUpMenu : public FullscreenMenu
{
private:
	bool m_Paused;
	std::vector<Texture *> m_powerupTextures;
	std::vector<Texture *> m_specialPowerupTextures;
	std::vector<int> m_selectedIndices;
//	SDL_FRect m_ErrorIconRect;
//	SDL_FRect m_ErrorOutlineRect;

	// Powerup effect functions
//	void ApplyAllStats();
//	void ApplyBombs();
//	void ApplyDoubleDamage();
//	void ApplyBossDamage();
//	void ApplyExplosiveAmmo();
//	void ApplyExtraLives();
//	void ApplySpeed();
//	void ApplySpiky();
//	void ApplyHealth();
//	void ApplyInfiniteGlockAmmo();

public:
	LevelUpMenu();
	~LevelUpMenu() override;

	// Sense
	[[nodiscard]] bool Paused() const { return m_Paused; }

	// Manipulating
	void SwitchToThisMenu() override;
//	void Show() override;

	// Ticking
//	void Event(const SDL_Event& event);
//	void Render();

};