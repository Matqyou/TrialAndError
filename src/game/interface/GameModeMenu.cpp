// src/game/interface/GameModeMenu.cpp

#include "GameModeMenu.h"

LoadedTexture GameModeMenu::sMenuTexture("interface.menu");
LoadedTexture GameModeMenu::sTextureTitle("ui.main.title");
LoadedTexture GameModeMenu::sTexturePlay("ui.main.playbutton");
LoadedTexture GameModeMenu::sTextureExit("ui.main.exit");

GameModeMenu::GameModeMenu(GameReference *game_window)
    : m_GameWindow(game_window)
{
    m_TitleRect = {int(m_GameWindow->GetWidth2()) - 250, 50, 500, 200};
    m_PlayButtonRect = {int(m_GameWindow->GetWidth2()) - 180, int(m_GameWindow->GetHeight2()) - 40, 360, 80};
    m_ExitButtonRect = {int(m_GameWindow->GetWidth2()) - 180, int(m_GameWindow->GetHeight2()) + 121, 360, 80};
}

GameModeMenu::~GameModeMenu()
{
}

void GameModeMenu::Show()
{
    bool running = true;
    bool menuOpen = true;

    while (menuOpen)
    {
        Tick();
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

void GameModeMenu::HandleEvent(const SDL_Event &event, bool &running, bool &menuOpen)
{
    switch (event.type)
    {
    case SDL_QUIT:
        m_GameWindow->Deinitialize(true); // close everything except sound
        while (Mix_Playing(-1))
        {
        } // wait until last sound is done playing
        delete m_GameWindow;
        exit(0);
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
                Assets::PauseMusic();
                Assets::Get()->GetSound("ui.pitch.low")->PlaySound();
                m_GameWindow->TestEnvironment();
            }
            if (x >= m_ExitButtonRect.x && x < m_ExitButtonRect.x + m_ExitButtonRect.w &&
                y >= m_ExitButtonRect.y && y < m_ExitButtonRect.y + m_ExitButtonRect.h)
            {
                m_GameWindow->Deinitialize(true);
                while (Mix_Playing(-1))
                {
                } // wait until last sound is done playing
                delete m_GameWindow;
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
            break;
        }
    }
}

void GameModeMenu::Tick()
{
    for (int i = 0; i < 3; i++)
    {
        auto random_position = Vec2d(rand() % m_GameWindow->GetWidth(), rand() % m_GameWindow->GetHeight());
        auto duration = 1500.0;
        m_Stars.emplace_back(random_position, Vec2d(0.0, 0.0), duration);
    }

    Vec2i MousePosition;
    SDL_GetMouseState(&MousePosition.x, &MousePosition.y);
    auto MousePositionD = Vec2d(MousePosition.x, MousePosition.y);

    for (int i = m_Stars.size() - 1; i >= 0; --i)
    {
        auto &[position, velocity, duration] = m_Stars[i];

        auto direction = position - MousePositionD;
        double distance = direction.Length();
        velocity += direction.Normalize() / distance * 0.15;
        velocity.x += (sin((position.x + position.y * 2) / 50)) * 0.0015;

        velocity *= 0.98;

        position.x += velocity.x;
        position.y += velocity.y;
        if (position.x > m_GameWindow->GetWidth())
            position.x -= m_GameWindow->GetWidth();
        if (position.y > m_GameWindow->GetHeight())
            position.y -= m_GameWindow->GetHeight();

        duration -= 1;
        if (duration <= 0.0)
            m_Stars.erase(m_Stars.begin() + i);
    }
}

void GameModeMenu::Render()
{
    Drawing *render = m_GameWindow->Render();
    SDL_Renderer *renderer = m_GameWindow->Renderer();

    render->SetColor(0, 0, 50, 255);
    render->Clear();

    render->SetColor(200, 200, 200, 255);
    for (int i = m_Stars.size() - 1; i >= 0; --i)
    {
        auto &[position, velocity, duration] = m_Stars[i];

        auto size = (int)duration / 750.0;
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
            {
                int draw_x = (int)(position.x - size / 2 + j);
                int draw_y = (int)(position.y - size / 2 + k);

                SDL_RenderDrawPoint(renderer, draw_x, draw_y);
            }
        }
    }

    render->RenderTexture(sTextureTitle.GetTexture()->SDLTexture(), nullptr, m_TitleRect);
    render->RenderTexture(sTexturePlay.GetTexture()->SDLTexture(), nullptr, m_PlayButtonRect);
    render->RenderTexture(sTextureExit.GetTexture()->SDLTexture(), nullptr, m_ExitButtonRect);
}