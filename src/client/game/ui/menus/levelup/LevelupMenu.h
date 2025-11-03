// src/game/interface/LevelUpMenu.h

#pragma once

#include "client/game/ui/menus/base/FullscreenMenu.h"
//#include "client/game/GameWorld.h"
#include "SDL3/SDL.h"
#include <client/game/powerups/PowerupManager.h>

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
	void ApplySelectedPowerup(Player* player, Powerup powerup);

public:
	LevelupMenu();
	~LevelupMenu() override;

	// Sense
	[[nodiscard]] bool Paused() const { return m_Paused; }

	// Manipulating
	void SwitchToThisMenu() override;

	// Ticking
	void HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary) override;
	void Tick(double elapsed_seconds) override;
	void Render() override;


};