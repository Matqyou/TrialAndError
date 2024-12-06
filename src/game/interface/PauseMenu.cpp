// src/game/interface/PauseMenu.cpp

#include "PauseMenu.h"

PauseMenu::PauseMenu(GameWorld *GameWorld, MainMenu *mainMenu)
    : m_GameWorld(GameWorld), m_MainMenu(mainMenu)
{
    AssetsManager *assetsHandler = m_GameWorld->GameWindow()->Assets();
    Decals* decals = Decals::Get();

    m_GameWindow = m_GameWorld->GameWindow();
    m_TextureResume = decals->GetTexture("interface.resume");
    m_TextureBack = decals->GetTexture("interface.back");
    m_Paused = false;
    m_ResumeButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) - 150, 200, 70};
    m_BackToMenuButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) + 50, 200, 70};
}

PauseMenu::~PauseMenu()
{
    // Clean up textures if necessary
}

void PauseMenu::Show()
{
    m_GameWorld->SetPaused(true);
    m_Paused = true;
}

void PauseMenu::HandleEvent(const SDL_Event &event)
{
    SoundManager *soundHandler = m_GameWindow->Assets()->SoundHandler();
    Sound *lowUISound = soundHandler->LoadSound("assets/sounds/LowUI.wav", true);
    Sound *midUISound = soundHandler->LoadSound("assets/sounds/MidUI.wav", true);

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
            if (x >= m_ResumeButtonRect.x && x < m_ResumeButtonRect.x + m_ResumeButtonRect.w &&
                y >= m_ResumeButtonRect.y && y < m_ResumeButtonRect.y + m_ResumeButtonRect.h)
            {
                soundHandler->PlaySound(lowUISound);
                m_Paused = false;
                m_GameWorld->SetPaused(false);
            }
            else if (x >= m_BackToMenuButtonRect.x && x < m_BackToMenuButtonRect.x + m_BackToMenuButtonRect.w &&
                     y >= m_BackToMenuButtonRect.y && y < m_BackToMenuButtonRect.y + m_BackToMenuButtonRect.h)
            {
                soundHandler->PlaySound(midUISound);
                m_GameWorld->SetPaused(true);
                m_Paused = false;
                m_MainMenu->Show(); // Open the main menu
            }
        }
        break;
        case SDL_MOUSEMOTION:
        {
            int x = event.motion.x;
            int y = event.motion.y;
            bool hovering = false;
            if ((x >= m_ResumeButtonRect.x && x < m_ResumeButtonRect.x + m_ResumeButtonRect.w &&
                 y >= m_ResumeButtonRect.y && y < m_ResumeButtonRect.y + m_ResumeButtonRect.h) ||
                (x >= m_BackToMenuButtonRect.x && x < m_BackToMenuButtonRect.x + m_BackToMenuButtonRect.w &&
                 y >= m_BackToMenuButtonRect.y && y < m_BackToMenuButtonRect.y + m_BackToMenuButtonRect.h))
            {
                hovering = true;
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
            m_ResumeButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) - 150, 200, 70};
            m_BackToMenuButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) + 50, 200, 70};
            m_GameWindow->Render()->UpdateWindow();
        }
        break;
    case SDL_KEYDOWN:
        SDL_Scancode ScancodeKey = event.key.keysym.scancode;
        if (ScancodeKey == SDL_SCANCODE_ESCAPE)
        {
            std::cout << "Escape key pressed" << std::endl;
            soundHandler->PlaySound(lowUISound);
            m_Paused = false;
            m_GameWorld->SetPaused(false);
           
        }
        break;
    }
}

void PauseMenu::Render()
{
    Drawing *render = m_GameWindow->Render();
    SDL_Renderer *renderer = m_GameWindow->Renderer();
    render->RenderTexture(m_TextureResume->SDLTexture(), nullptr, m_ResumeButtonRect);
    render->RenderTexture(m_TextureBack->SDLTexture(), nullptr, m_BackToMenuButtonRect);
    render->UpdateWindow();
}