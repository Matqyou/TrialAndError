// src/game/interface/PauseMenu.cpp

#include "LevelUpMenu.h"
#include <vector>
#include <cstdlib>
#include <ctime>

LevelUpMenu::LevelUpMenu(GameReference *gameWindow)
    : m_GameWindow(gameWindow)
{
    AssetsManager *assetsHandler = m_GameWindow->Assets();
    ImageManager *imageHandler = assetsHandler->ImageHandler();

    m_Font = TTF_OpenFont("assets/fonts/Minecraft.ttf", 24); // Adjust the path and size as needed


    m_TextureAllStats = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/AllStats.jpg", true);
    m_TextureBombs = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/Bombs.jpg", true);
    m_TextureBossDamage = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/BossDamage.jpg", true);
    m_TextureDoubleDamage = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/DoubleDamage.jpg", true);
    m_TextureExtraLives = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/ExtraLives.jpg", true);
    m_TextureExplosiveAmmo = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/ExplosiveAmmo.jpg", true);
    m_TextureSpeed = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/Speed.jpg", true);
    m_TextureSpiky = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/Spiky.jpg", true);
    m_TextureHealth = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/Health.jpg", true);
    m_TextureInfiniteGlockAmmo = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/InfiniteGlockAmmo.jpg", true);

    int textureWidth = int(m_GameWindow->GetWidth2() * 0.4);
    int textureHeight = int(m_GameWindow->GetHeight2() * 0.6);

    m_TextureAllStatsRect = {0, 0, textureWidth, textureHeight};
    m_TextureBombsRect = {textureWidth, 0, textureWidth, textureHeight};
    m_TextureBossDamageRect = {2 * textureWidth, 0, textureWidth, textureHeight};
    m_TextureDoubleDamageRect = {0, textureHeight, textureWidth, textureHeight};
    m_TextureExplosiveAmmoRect = {textureWidth, textureHeight, textureWidth, textureHeight};
    m_TextureSpeedRect = {2 * textureWidth, textureHeight, textureWidth, textureHeight};
    m_TextureSpikyRect = {0, 2 * textureHeight, textureWidth, textureHeight};
    m_TextureHealthRect = {textureWidth, 2 * textureHeight, textureWidth, textureHeight};
    m_TextureInfiniteGlockAmmoRect = {2 * textureWidth, 2 * textureHeight, textureWidth, textureHeight};
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
    bool running = true;
    bool lvlMenuOpen = true;

    // Randomly select 3 powerups
    std::vector<SDL_Rect> powerupRects = {m_TextureAllStatsRect, m_TextureBombsRect, m_TextureBossDamageRect, m_TextureDoubleDamageRect, m_TextureExplosiveAmmoRect, m_TextureSpeedRect, m_TextureSpikyRect, m_TextureHealthRect, m_TextureInfiniteGlockAmmoRect};
    std::vector<Texture *> powerupTextures = {m_TextureAllStats, m_TextureBombs, m_TextureBossDamage, m_TextureDoubleDamage, m_TextureExplosiveAmmo, m_TextureSpeed, m_TextureSpiky, m_TextureHealth, m_TextureInfiniteGlockAmmo};
    std::vector<int> selectedIndices;
    while (selectedIndices.size() < 3)
    {
        int index = rand() % powerupRects.size();
        if (std::find(selectedIndices.begin(), selectedIndices.end(), index) == selectedIndices.end())
        {
            selectedIndices.push_back(index);
        }
    }

    while (lvlMenuOpen)
    {
        Render(selectedIndices, powerupRects, powerupTextures);
        SDL_ShowCursor(1);
        SDL_Event currentEvent;
        while (SDL_PollEvent(&currentEvent))
        {
            m_GameWindow->Event(currentEvent);
            HandleEvent(currentEvent, running, lvlMenuOpen, selectedIndices, powerupRects);
        }
        m_GameWindow->Render()->UpdateWindow();
    }
}
void LevelUpMenu::HandleEvent(const SDL_Event &event, bool &running, bool &lvlMenuOpen, const std::vector<int> &selectedIndices, const std::vector<SDL_Rect> &powerupRects)
{
    SoundManager *soundHandler = m_GameWindow->Assets()->SoundHandler();
    Sound *lowUISound = soundHandler->LoadSound("assets/sounds/LowUI.wav", true);
    Sound *midUISound = soundHandler->LoadSound("assets/sounds/MidUI.wav", true);

    switch (event.type)
    {
    case SDL_QUIT:
        lvlMenuOpen = false;
        running = false;
        m_GameWindow->Deinitialize(true);
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            
            int x = event.button.x;
            int y = event.button.y;
            for (int i = 0; i < selectedIndices.size(); ++i)
            {
                // TODO: Change to use the whole powerup rect instead of just the icon rect (i should probably just declare the whole rect not jsut the image
                SDL_Rect rect = powerupRects[selectedIndices[i]];
                if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h)
                {
                    soundHandler->PlaySound(midUISound);
                    // Apply the selected powerup
                    lvlMenuOpen = false;
                    break;
                }
            }
        }
        break;
    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            // Adjust positions and sizes of powerup textures
            // TODO still need to fix scaling smthn is off
            int startX = int(m_GameWindow->GetWidth2() / 6);    
            int startY = int(m_GameWindow->GetHeight2() / 2.5); 
            int spacing = int(m_GameWindow->GetWidth2() * 0.25); 

            for (int i = 0; i < selectedIndices.size(); ++i)
            {
                SDL_Rect rect = powerupRects[selectedIndices[i]];
                rect.w = int(m_GameWindow->GetWidth2() * 0.4);
                rect.h = int(m_GameWindow->GetHeight2() * 0.6);
                rect.x = startX + i * (rect.w + spacing);
                rect.y = startY;
            }
        }
        break;
    case SDL_KEYDOWN:
        SDL_Scancode ScancodeKey = event.key.keysym.scancode;
        if (ScancodeKey == SDL_SCANCODE_ESCAPE)
        {
            // Apply a random powerup (idk maybe some speedrunners will like this)
            soundHandler->PlaySound(lowUISound);
            lvlMenuOpen = false;
        }
        break;
    }
}
void LevelUpMenu::Render(const std::vector<int> &selectedIndices, const std::vector<SDL_Rect> &powerupRects, const std::vector<Texture *> &powerupTextures)
{
    Drawing *render = m_GameWindow->Render();

    int startX = int(m_GameWindow->GetWidth2() / 6);   
    int startY = int(m_GameWindow->GetHeight2() / 2.5);   
    int spacing = int(m_GameWindow->GetWidth2() * 0.25); // Space between powerups

    for (int i = 0; i < selectedIndices.size(); ++i)
    {
        SDL_Rect rect = powerupRects[selectedIndices[i]];
        Texture *texture = powerupTextures[selectedIndices[i]];

        // Adjust position and size
        rect.x = startX + i * (rect.w + spacing);
        rect.y = startY;

        // Draw container with padding
        SDL_Rect containerRect = {rect.x - 50, rect.y - 10, rect.w + 100, rect.h + 230};
        render->SetColor(200, 200, 200, 255); // Light gray background
        render->FillRect(containerRect);

        // Draw powerup texture
        render->RenderTexture(texture->SDLTexture(), nullptr, rect);

        // Draw description below the powerup
        SDL_Rect descriptionRect = {rect.x - 10, rect.y + rect.h + 10, int(floor((rect.w + 50) / 1.25)), int(floor((rect.h + 115) / 1.45))}; // Adjust height dynamically
        render->SetColor(0, 0, 0, 255);                                                                                                      // Black text color

        // Create surface and texture for the text
        SDL_Color textColor = {0, 0, 0, 255};
        SDL_Surface *textSurface = TTF_RenderText_Blended_Wrapped(m_Font, "This powerup does something very very cool and you will never know what hehehehehehehehehe + nerd", textColor, rect.w);
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
}