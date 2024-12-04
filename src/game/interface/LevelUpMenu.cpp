#include "LevelUpMenu.h"
#include <vector>
#include <cstdlib>
#include <ctime>

LevelUpMenu::LevelUpMenu(GameWorld *gameWorld)
    : m_GameWorld(gameWorld)
{
    m_GameWindow = m_GameWorld->GameWindow();
    AssetsManager *assetsHandler = m_GameWindow->Assets();
    ImageManager *imageHandler = assetsHandler->ImageHandler();

    m_Font = TTF_OpenFont("assets/fonts/Minecraft.ttf", 24); // Adjust the path and size as needed
    if (!m_Font)
    {
        // Handle error
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
    }
    
    m_TextureAllStats = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/AllStats.jpg", true);
    m_TextureBombs = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/Bombs.jpg", true);
    m_TextureBossDamage = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/BossDamage.jpg", true);
    m_TextureDoubleDamage = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/DoubleDamage.jpg", true);
    m_TextureExplosiveAmmo = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/ExplosiveAmmo.jpg", true);
    m_TextureSpeed = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/Speed.jpg", true);
    m_TextureSpiky = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/Spiky.jpg", true);
    m_TextureHealth = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/Health.jpg", true);
    m_TextureInfiniteGlockAmmo = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/InfiniteGlockAmmo.jpg", true);
    m_TextureErrorOutline = imageHandler->LoadTexture("assets/images/interface/PermanentErrors/ErrorOutlineFull.png", true);
    
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
    while(m_selectedIndices.size() < 3)
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
    SoundManager *soundHandler = m_GameWindow->Assets()->SoundHandler();
    Sound *lowUISound = soundHandler->LoadSound("assets/sounds/LowUI.wav", true);
    Sound *midUISound = soundHandler->LoadSound("assets/sounds/MidUI.wav", true);

    switch (event.type)
    {
    case SDL_QUIT:
        m_GameWindow->Deinitialize(true);
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
                    soundHandler->PlaySound(midUISound);
                    // Apply the selected powerup
                    m_Paused = false;
                    m_GameWorld->SetPaused(false);
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
                if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h/1.2)
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
            soundHandler->PlaySound(lowUISound);
            m_Paused = false;
            m_GameWorld->SetPaused(false);
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
        m_ErrorIconRect.y = rect.y + int(rect.h * 0.2);
        m_ErrorIconRect.w = int(rect.w * 0.4);
        m_ErrorIconRect.h = int(rect.h * 0.225);
        // Draw powerup texture
        render->RenderTexture(texture->SDLTexture(), nullptr, m_ErrorIconRect);

        // Draw description below the powerup
        SDL_Rect descriptionRect = {rect.x + int(rect.w / 4), int((rect.y + rect.h) / 1.8), rect.w - int(rect.w / 2.25), rect.w - int(rect.w / 2.25)}; // Increase height for multiple lines
        render->SetColor(0, 0, 0, 255);                                                                                         // Black text color

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
    render->UpdateWindow();
}