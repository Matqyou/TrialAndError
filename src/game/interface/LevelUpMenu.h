// src/game/interface/LevelUpMenu.h

#pragma once

#include "../../GameReference.h"
#include <SDL.h>

class LevelUpMenu
{
private:
    TTF_Font *m_Font;
    GameReference *m_GameWindow;
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

    SDL_Rect m_TextureAllStatsRect;
    SDL_Rect m_TextureBombsRect;
    SDL_Rect m_TextureDoubleDamageRect;
    SDL_Rect m_TextureBossDamageRect;
    SDL_Rect m_TextureExplosiveAmmoRect;
    SDL_Rect m_TextureExtraLivesect;
    SDL_Rect m_TextureSpeedRect;
    SDL_Rect m_TextureSpikyRect;
    SDL_Rect m_TextureHealthRect;
    SDL_Rect m_TextureInfiniteGlockAmmoRect;

public:
    LevelUpMenu(GameReference *gameWindow);
    ~LevelUpMenu();

    void Show();
    void HandleEvent(const SDL_Event &event, bool &running, bool &lvlMenuOpen, const std::vector<int> &selectedIndices,const  std::vector<SDL_Rect> &powerupRects);
    void Render(const std::vector<int> &selectedIndices,  const std::vector<SDL_Rect> &powerupRects, const std::vector<Texture *> &powerupTextures);
};