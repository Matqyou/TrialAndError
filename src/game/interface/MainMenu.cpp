// src/game/interface/MainMenu.cpp

#include "MainMenu.h"

MainMenu::MainMenu(GameReference *gameWindow)
    : m_GameWindow(gameWindow)
{
    AssetsManager *assetsHandler = m_GameWindow->Assets();
    ImageManager *imageHandler = assetsHandler->ImageHandler();

    m_MenuTexture = imageHandler->LoadTexture("assets/images/interface/Menu.png", true);
    m_TexturePlay = imageHandler->LoadTexture("assets/images/interface/PlayButton.png", true);
    m_TextureExit = imageHandler->LoadTexture("assets/images/interface/Exit.png", true);

    m_PlayButtonRect = {int(m_GameWindow->GetWidth2()) - 180, int(m_GameWindow->GetHeight2()) - 40, 360, 80};
    m_ExitButtonRect = {int(m_GameWindow->GetWidth2()) - 180, int(m_GameWindow->GetHeight2()) + 121, 360, 80};
}

MainMenu::~MainMenu()
{
    // Clean up textures if necessary
}

void MainMenu::Show()
{
    bool running = true;
    bool menuOpen = true;

    while (menuOpen)
    {
        Render();
        SDL_ShowCursor(1);
        SDL_Event currentEvent;
        while (SDL_PollEvent(&currentEvent))
        {
            m_GameWindow->Event(currentEvent);
            HandleEvent(currentEvent, running, menuOpen);
        }
        m_GameWindow->Render()->UpdateWindow();
    }
    m_GameWindow->Render()->Clear();
}

void MainMenu::HandleEvent(const SDL_Event &event, bool &running, bool &menuOpen)
{
    SoundManager *soundHandler = m_GameWindow->Assets()->SoundHandler();
    Sound *quitSound = soundHandler->LoadSound("assets/sounds/Quit.wav", true);
    Sound *lowUISound = soundHandler->LoadSound("assets/sounds/LowUI.wav", true);

    switch (event.type)
    {
    case SDL_QUIT:
        menuOpen = false;
        running = false;
        m_GameWindow->Deinitialize(true);
        break;
    case SDL_MOUSEBUTTONDOWN:
        SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
        if (event.button.button == SDL_BUTTON_LEFT)
        {
            int x = event.button.x;
            int y = event.button.y;
            if (x >= m_PlayButtonRect.x && x < m_PlayButtonRect.x + m_PlayButtonRect.w &&
                y >= m_PlayButtonRect.y && y < m_PlayButtonRect.y + m_PlayButtonRect.h)
            {
                menuOpen = false;
                soundHandler->PlaySound(lowUISound);
            }
            if (x >= m_ExitButtonRect.x && x < m_ExitButtonRect.x + m_ExitButtonRect.w &&
                y >= m_ExitButtonRect.y && y < m_ExitButtonRect.y + m_ExitButtonRect.h)
            {
                soundHandler->PlaySound(quitSound);
                m_GameWindow->Deinitialize(true);
                running = false;
                menuOpen = false;
            }
        }
        break;
    case SDL_MOUSEMOTION:
    {
        int x = event.motion.x;
        int y = event.motion.y;
        bool hovering = false;
        if ((x >= m_PlayButtonRect.x && x < m_PlayButtonRect.x + m_PlayButtonRect.w &&
             y >= m_PlayButtonRect.y && y < m_PlayButtonRect.y + m_PlayButtonRect.h) ||
            (x >= m_ExitButtonRect.x && x < m_ExitButtonRect.x + m_ExitButtonRect.w &&
             y >= m_ExitButtonRect.y && y < m_ExitButtonRect.y + m_ExitButtonRect.h))
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
            m_PlayButtonRect = {int(m_GameWindow->GetWidth2()) - 180, int(m_GameWindow->GetHeight2()) - 40, 360, 80};
            m_ExitButtonRect = {int(m_GameWindow->GetWidth2()) - 180, int(m_GameWindow->GetHeight2()) + 121, 360, 80};
            m_GameWindow->Render()->Clear();
            m_GameWindow->Render()->UpdateWindow();
        }
        break;
    }
}

void MainMenu::Render()
{
    Drawing *render = m_GameWindow->Render();
    SDL_Renderer *renderer = m_GameWindow->Renderer();
    SDL_RenderClear(renderer);
    render->RenderTextureFullscreen(m_MenuTexture->SDLTexture(), nullptr);
    render->RenderTexture(m_TexturePlay->SDLTexture(), nullptr, m_PlayButtonRect);
    render->RenderTexture(m_TextureExit->SDLTexture(), nullptr, m_ExitButtonRect);
    render->UpdateWindow();
}