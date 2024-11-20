// src/game/interface/PauseMenu.cpp

#include "PauseMenu.h"

PauseMenu::PauseMenu(GameReference *gameWindow, MainMenu *mainMenu)
    : m_GameWindow(gameWindow), m_MainMenu(mainMenu)
{
    AssetsManager *assetsHandler = m_GameWindow->Assets();
    ImageManager *imageHandler = assetsHandler->ImageHandler();

    m_TextureResume = imageHandler->LoadTexture("assets/images/interface/Resume.png", true);
    m_TextureBack = imageHandler->LoadTexture("assets/images/interface/Back.png", true);

    m_ResumeButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) - 150, 200, 70};
    m_BackToMenuButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) + 50, 200, 70};
}

PauseMenu::~PauseMenu()
{
    // Clean up textures if necessary
}

void PauseMenu::Show()
{
    bool running = true;
    bool pauseMenu = true;

    while (pauseMenu)
    {
        Render();
        SDL_ShowCursor(1);
        SDL_Event currentEvent;
        while (SDL_PollEvent(&currentEvent))
        {
            m_GameWindow->Event(currentEvent);
            HandleEvent(currentEvent, running, pauseMenu);
        }
        m_GameWindow->Render()->UpdateWindow();
    }
}

void PauseMenu::HandleEvent(const SDL_Event &event, bool &running, bool &pauseMenu)
{
    SoundManager *soundHandler = m_GameWindow->Assets()->SoundHandler();
    Sound *lowUISound = soundHandler->LoadSound("assets/sounds/LowUI.wav", true);
    Sound *midUISound = soundHandler->LoadSound("assets/sounds/MidUI.wav", true);

    switch (event.type)
    {
    case SDL_QUIT:
        pauseMenu = false;
        running = false;
        m_GameWindow->Deinitialize(true);
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= m_ResumeButtonRect.x && x < m_ResumeButtonRect.x + m_ResumeButtonRect.w &&
                y >= m_ResumeButtonRect.y && y < m_ResumeButtonRect.y + m_ResumeButtonRect.h)
            {
                soundHandler->PlaySound(lowUISound);
                pauseMenu = false;
            }
            else if (x >= m_BackToMenuButtonRect.x && x < m_BackToMenuButtonRect.x + m_BackToMenuButtonRect.w &&
                     y >= m_BackToMenuButtonRect.y && y < m_BackToMenuButtonRect.y + m_BackToMenuButtonRect.h)
            {
                soundHandler->PlaySound(midUISound);
                pauseMenu = false;
                running = false;
                m_MainMenu->Show(); // Open the main menu
            }
        }
        break;
    case SDL_WINDOWEVENT:
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            m_ResumeButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) - 150, 200, 70};
            m_BackToMenuButtonRect = {int(m_GameWindow->GetWidth2()) - 100, int(m_GameWindow->GetHeight2()) + 50, 200, 70};
        }
        break;
    case SDL_KEYDOWN:
        SDL_Scancode ScancodeKey = event.key.keysym.scancode;
        if (ScancodeKey == SDL_SCANCODE_ESCAPE)
        {
            soundHandler->PlaySound(lowUISound);
            pauseMenu = false;
        }
        break;
    }
}

void PauseMenu::Render()
{
    Drawing *render = m_GameWindow->Render();

    render->RenderTexture(m_TextureResume->SDLTexture(), nullptr, m_ResumeButtonRect);
    render->RenderTexture(m_TextureBack->SDLTexture(), nullptr, m_BackToMenuButtonRect);
}