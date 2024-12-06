#include "LevelUpMenu.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include "../Player.h"

LevelUpMenu::LevelUpMenu(GameWorld *gameWorld, Player *Player)
    : m_GameWorld(gameWorld)
{
    m_Player = Player;
    m_GameWindow = m_GameWorld->GameWindow();
    AssetsManager *assetsHandler = m_GameWindow->Assetz();
    Assets* decals = Assets::Get();

    m_Font = TTF_OpenFont("assets/fonts/Minecraft.ttf", 24); // Adjust the path and size as needed
    if (!m_Font)
    {
        // Handle error
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    }

    m_TextureAllStats = decals->GetTexture("interface.permanenterrors.allstats");
    m_TextureBombs = decals->GetTexture("interface.permanenterrors.bombs");
    m_TextureBossDamage = decals->GetTexture("interface.permanenterrors.bossdamage");
    m_TextureDoubleDamage = decals->GetTexture("interface.permanenterrors.doubledamage");
    m_TextureExplosiveAmmo = decals->GetTexture("interface.permanenterrors.explosiveammo");
    m_TextureSpeed = decals->GetTexture("interface.permanenterrors.speed");
    m_TextureSpiky = decals->GetTexture("interface.permanenterrors.spiky");
    m_TextureHealth = decals->GetTexture("interface.permanenterrors.health");
    m_TextureInfiniteGlockAmmo = decals->GetTexture("interface.permanenterrors.infiniteglockammo");
    m_TextureErrorOutline = decals->GetTexture("interface.permanenterrors.regularoutlinefull");

    m_ErrorIconRect = {0, 0, int(m_GameWindow->GetWidth2() * 0.4), int(m_GameWindow->GetHeight2() * 0.4)};
    m_ErrorOutlineRect = {0, 0, int(m_GameWindow->GetWidth2() / 2.2), int(m_GameWindow->GetHeight2() / 0.6)};
    m_powerupTextures = {m_TextureAllStats, m_TextureBombs, m_TextureBossDamage, m_TextureDoubleDamage, m_TextureExplosiveAmmo, m_TextureSpeed, m_TextureSpiky, m_TextureHealth, m_TextureInfiniteGlockAmmo};
    srand(static_cast<unsigned int>(time(nullptr)));
}

LevelUpMenu::~LevelUpMenu()
{
    if (m_Font)
    {
        TTF_CloseFont(m_Font);
    }
    // Clean up textures if necessary
}

void LevelUpMenu::Show()
{

    m_GameWorld->SetDelay(true);
    m_selectedIndices = {};
    m_GameWorld->SetPaused(true);

    m_Paused = true;

    // Randomly select 3 powerups
    while (m_selectedIndices.size() < 3)
    {
        int index = rand() % m_powerupTextures.size();
        if (std::find(m_selectedIndices.begin(), m_selectedIndices.end(), index) == m_selectedIndices.end())
        {
            m_selectedIndices.push_back(index);
        }
    }
}

void LevelUpMenu::HandleEvent(const SDL_Event &event)
{
    if (m_GameWorld->GetDelay())
    {
        // Ignore events during delay
        return;
    }
    Sound *lowUISound = Assets::Get()->GetSound("lowui");
    Sound *midUISound = Assets::Get()->GetSound("midui");

    switch (event.type)
    {
    case SDL_QUIT:
        m_GameWindow->Deinitialize(true);
        while (Mix_Playing(-1)) {} // wait until last sound is done playing
        delete m_GameWindow;
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
            int x = event.button.x;
            int y = event.button.y;
            for (int i = 0; i < m_selectedIndices.size(); ++i)
            {
                SDL_Rect rect = m_ErrorOutlineRect;
                rect.x = int(m_GameWindow->GetWidth2() / 12) + i * (rect.w + int(m_GameWindow->GetWidth2() * 0.25));
                rect.y = int(m_GameWindow->GetHeight2() / 6);
                if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h)
                {
                    midUISound->PlaySound();

                    // Apply the selected powerup
                    switch (m_selectedIndices[i])
                    {
                    case 0:
                        ApplyAllStats();
                        break;
                    case 1:
                        ApplyBombs();
                        break;
                    case 2:
                        ApplyDoubleDamage();
                        break;
                    case 3:
                        ApplyBossDamage();
                        break;
                    case 4:
                        ApplyExplosiveAmmo();
                        break;
                    case 5:
                        ApplySpeed();
                        break;
                    case 6:
                        ApplySpiky();
                        break;
                    case 7:
                        ApplyHealth();
                        break;
                    case 8:
                        ApplyInfiniteGlockAmmo();
                        break;
                    default:
                        break;
                    }
//                    m_Player->GetCharacter(); // Example method to increase health
                    m_Paused = false;
                    break;
                }
            }
        }
        break;
    case SDL_MOUSEMOTION:
    {
        int x = event.motion.x;
        int y = event.motion.y;
        bool hovering = false;
        for (int i = 0; i < m_selectedIndices.size(); ++i)
        {
            SDL_Rect rect = m_ErrorOutlineRect;
            rect.x = int(m_GameWindow->GetWidth2() / 12) + i * (rect.w + int(m_GameWindow->GetWidth2() * 0.25));
            rect.y = int(m_GameWindow->GetHeight2() / 6);
            if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h / 1.2)
            {
                hovering = true;
                break;
            }
        }
        if (hovering)
        {
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
        }
        else
        {
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        }
    }
    break;
    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            // Adjust positions and sizes of powerup textures
            int startX = int(m_GameWindow->GetWidth2() / 12);
            int startY = int(m_GameWindow->GetHeight2() / 6);
            int spacing = int(m_GameWindow->GetWidth2() * 0.25);

            for (int i = 0; i < m_selectedIndices.size(); ++i)
            {
                SDL_Rect &rect = m_ErrorOutlineRect;
                rect.x = startX + i * (rect.w + spacing);
                rect.y = startY;
                rect.w = int(m_GameWindow->GetWidth2() / 2.2);
                rect.h = int(m_GameWindow->GetHeight2() / 0.6);
            }
            m_GameWindow->Render()->UpdateWindow();
        }
        break;
    case SDL_KEYDOWN:
        SDL_Scancode ScancodeKey = event.key.keysym.scancode;
        if (ScancodeKey == SDL_SCANCODE_ESCAPE)
        {
            lowUISound->PlaySound();
            m_Paused = false;
        }
        break;
    }
}

void LevelUpMenu::Render()
{
    Drawing *render = m_GameWindow->Render();
    int startX = int(m_GameWindow->GetWidth2() / 12);
    int startY = int(m_GameWindow->GetHeight2() / 6);
    int spacing = int(m_GameWindow->GetWidth2() * 0.25);

    for (int i = 0; i < m_selectedIndices.size(); ++i)
    {
        SDL_Rect rect = m_ErrorOutlineRect;
        Texture *texture = m_powerupTextures[m_selectedIndices[i]];

        // Adjust position and size
        rect.x = startX + i * (rect.w + spacing);
        rect.y = startY;

        // Draw ErrorOutline texture as the base
        render->RenderTexture(m_TextureErrorOutline->SDLTexture(), nullptr, rect);

        m_ErrorIconRect.x = rect.x + int(rect.w * 0.3);
        m_ErrorIconRect.y = rect.y + int(rect.h * 0.15);
        m_ErrorIconRect.w = int(rect.w * 0.4);
        m_ErrorIconRect.h = int(rect.h * 0.225);
        // Draw powerup texture
        render->RenderTexture(texture->SDLTexture(), nullptr, m_ErrorIconRect);

        // Draw description below the powerup
        SDL_Rect descriptionRect = {rect.x + int(rect.w / 4), int((rect.y + rect.h) / 1.9), rect.w - int(rect.w / 2.25), rect.h - int(rect.h / 1.35)};
        render->SetColor(0, 0, 0, 255);

        // Create surface and texture for the text
        SDL_Color textColor = {0, 0, 0, 255};
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(m_Font, "This powerup does something very very cool and you will never know what hehehehehehehehehe + nerd", textColor, descriptionRect.w);
        if (textSurface)
        {
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(render->Renderer(), textSurface);
            if (textTexture)
            {
                SDL_RenderCopy(render->Renderer(), textTexture, nullptr, &descriptionRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
    }
    render->UpdateWindow();
}

// Powerup effect functions
void LevelUpMenu::ApplyAllStats()
{
    // Implement the effect of the AllStats powerup
    std::cout << "AllStats powerup applied" << std::endl;
}

void LevelUpMenu::ApplyBombs()
{
    // Implement the effect of the Bombs powerup
    std::cout << "Bombs powerup applied" << std::endl;
}

void LevelUpMenu::ApplyDoubleDamage()
{
    // Implement the effect of the DoubleDamage powerup
    std::cout << "DoubleDamage powerup applied" << std::endl;
    m_Player->IncreaseDamageAmp(2.0);
}

void LevelUpMenu::ApplyBossDamage()
{
    // Implement the effect of the BossDamage powerup
    std::cout << "BossDamage powerup applied" << std::endl;
    m_Player->IncreaseBossDamageAmp(0.2);
}

void LevelUpMenu::ApplyExplosiveAmmo()
{
    // Implement the effect of the ExplosiveAmmo powerup
    std::cout << "ExplosiveAmmo powerup applied" << std::endl;
}

void LevelUpMenu::ApplyExtraLives()
{
    // Implement the effect of the ExtraLives powerup
    std::cout << "ExtraLives powerup applied" << std::endl;
    m_Player->SetExtraLife(true);
}

void LevelUpMenu::ApplySpeed()
{
    // Implement the effect of the Speed powerup
    std::cout << "Speed powerup applied" << std::endl;
    // m_Player->IncreaseSpeed(1.1);
}

void LevelUpMenu::ApplySpiky()
{
    // Implement the effect of the Spiky powerup
    std::cout << "Spiky powerup applied" << std::endl;
}

void LevelUpMenu::ApplyHealth()
{
    // Implement the effect of the Health powerup
    std::cout << "Health powerup applied" << std::endl;
    m_Player->IncreaseMaxHealthAmp(1.1);
}

void LevelUpMenu::ApplyInfiniteGlockAmmo()
{
    // Implement the effect of the InfiniteGlockAmmo powerup
    std::cout << "InfiniteGlockAmmo powerup applied" << std::endl;
}