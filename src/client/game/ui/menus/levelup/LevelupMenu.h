// src/game/interface/LevelUpMenu.h

#pragma once

#include "client/game/ui/menus/base/FullscreenMenu.h"
//#include "client/game/GameWorld.h"
#include "SDL3/SDL.h"

class Player;
class LevelupMenu : public FullscreenMenu
{
private:
	bool m_Paused;
	std::vector<Texture *> m_powerupTextures;
	std::vector<Texture *> m_specialPowerupTextures;
	std::vector<int> m_selectedIndices;

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
	LevelupMenu();
	~LevelupMenu() override;

	// Sense
	[[nodiscard]] bool Paused() const { return m_Paused; }

	// Manipulating
	void SwitchToThisMenu() override;
//	void Show() override;

	// Ticking
//	void Event(const SDL_Event& event);
//	void Render();

};