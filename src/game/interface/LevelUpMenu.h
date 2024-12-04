// src/game/interface/LevelUpMenu.h

#pragma once

#include "../../GameReference.h"
#include "../GameWorld.h"
#include <SDL.h>

class LevelUpMenu
{
private:
    TTF_Font *m_Font;

    GameReference *m_GameWindow;
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
    std::vector<Texture *>
        m_powerupTextures;
    std::vector<int> m_selectedIndices;
    SDL_Rect m_ErrorIconRect;
    SDL_Rect m_ErrorOutlineRect;

public:
    LevelUpMenu(GameWorld *gameWorld);
    ~LevelUpMenu();

    [[nodiscard]] bool Paused() const { return m_Paused; }
    void Show();
    void HandleEvent(const SDL_Event &event);
    void Render();
};