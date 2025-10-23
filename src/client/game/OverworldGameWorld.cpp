// OverworldGameWorld.cpp
#include "OverworldGameWorld.h"
#include "client/core/Assets.h"
#include "entities/characters/character/Character.h"
#include "client/core/Drawing.h"
#include "../game/planetaryTrials/PlanetaryGameWorld.h"
#include "../game/planetaryTrials/PlanetaryCharacter.h"

OverworldGameWorld::OverworldGameWorld(GameData *game_window, int width, int height)
    : GameWorld(game_window, width, height)
{
    // Create some dummy planets in the world (with color)
    m_Planets.push_back({Vec2d(width * 3, height * 4), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2d(width * 5, height * 5), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2d(width, height), 48, SDL_Color{120, 120, 255, 255}});
    m_Planets.push_back({Vec2d(width * 0.7, height * 0.6), 64, SDL_Color{200, 160, 255, 255}});

    // Spawn Planet entities
    for (size_t i = 0; i < m_Planets.size(); ++i)
    {
        auto &p = m_Planets[i];
        // Choose a plausible planet texture; fall back to no texture if missing
        std::string tex = (i == 0) ? std::string("backgrounds.planet2") : std::string("backgrounds.planet1");
        Planet *pe = new Planet(this, p.pos, p.radius, p.color, tex);
        m_PlanetEntities.push_back(pe);
    }

    // Setup popup rects (centered)
    int pw = 480, ph = 220;
    m_PopupRect = SDL_Rect{(width - pw) / 2, (height - ph) / 2, pw, ph};
    int bw = 160, bh = 56;
    m_PopupStartButton = SDL_Rect{m_PopupRect.x + 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh};
    m_PopupCancelButton = SDL_Rect{m_PopupRect.x + m_PopupRect.w - bw - 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh};
}

OverworldGameWorld::~OverworldGameWorld()
{
    for (auto *p : m_PlanetEntities)
        delete p;
}

void OverworldGameWorld::Tick()
{
    GameWorld::Tick();

    if (!FirstPlayer())
        return;
    Character *c = FirstPlayer()->GetCharacter();
    if (!c)
        return;

    // Use the character's world position for proximity checks (not the camera target)
    Vec2d ppos = c->GetCore().Pos;

    // If already showing popup, check input for buttons
    if (m_ShowEnterPopup && m_ActivePlanet)
    {
        // update hover states based on mouse
        int mx, my;
        Uint32 mstate = SDL_GetMouseState(&mx, &my);
        SDL_Point mouse{mx, my};
        m_PopupStartHover = SDL_PointInRect(&mouse, &m_PopupStartButton);
        m_PopupCancelHover = SDL_PointInRect(&mouse, &m_PopupCancelButton);

        // Check for click (left mouse pressed)
        if (mstate & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (m_PopupStartHover)
            {
                // Create a planetary world and transfer existing players into it.
                GameData *gd = GameWindow();
                // Create the new planetary world (use same size as InitializeInfinite uses)
                PlanetaryGameWorld *pgw = new PlanetaryGameWorld(gd, 50, 30);

                // Collect players from the current world first
                std::vector<Player *> players;
                for (Player *p = FirstPlayer(); p; p = p->Next())
                {
                    players.push_back(p);
                }

                // Move each player into the new planetary world
                for (Player *p : players)
                {
                    // Remember old controller and character
                    Character *oldChar = p->GetCharacter();
                    GameController *oldController = nullptr;
                    if (oldChar)
                        oldController = oldChar->GetGameController();

                    // Remove from this world's player list
                    RemovePlayer(p);

                    // Update player's game world pointer to the new world
                    p->SetGameWorld(pgw);

                    // Add to new world's player list
                    pgw->AddPlayer(p);

                    // Delete old character if present
                    if (oldChar)
                        delete oldChar;

                    // Spawn a PlanetaryCharacter for this player at the new world's center
                    Vec2d worldCenter(pgw->GetWidth() / 2.0, pgw->GetHeight() / 2.0);
                    auto newChar = new PlanetaryCharacter(pgw, p, 100.0, worldCenter, Vec2d(10, 10), false);
                    if (oldController && newChar)
                        newChar->SetGameController(oldController);
                }

                // Switch the active world in GameData (this will delete the old world)
                gd->SetWorld(pgw);
            }
            else if (m_PopupCancelHover)
            {
                // close popup and nudge player away from planet
                m_ShowEnterPopup = false;
                if (m_ActivePlanet)
                {
                    Vec2d planetPos = m_ActivePlanet->GetCore().Pos;
                    Vec2d dir = ppos - planetPos;
                    if (dir.Length() == 0)
                        dir = Vec2d(1, 0);
                    dir.Normalize();
                    // push player to a bit beyond radius
                    c->GetCore().Vel = dir * 20;
                }
            }
        }

        return;
    }

    // Detect proximity to any planet
    for (auto *pe : m_PlanetEntities)
    {
        Vec2d d = ppos - pe->GetCore().Pos;
        if (d.Length() < pe->Radius() + 8)
        {
            // show popup and set active planet
            m_ShowEnterPopup = true;
            m_ActivePlanet = pe;
            break;
        }
    }
}

void OverworldGameWorld::Draw()
{
    Drawing *render = GameWindow()->Render();
    SDL_Renderer *r = GameWindow()->Renderer();
    render->SetColor(10, 10, 30, 255);
    render->Clear();

    // Draw planets via their entities
    for (auto *pe : m_PlanetEntities)
    {
        pe->Draw();
    }

    // Draw rest via base (players, UI)
    GameWorld::Draw();

    // Draw enter popup if needed
    if (m_ShowEnterPopup && m_ActivePlanet)
    {
        // Recompute centered popup rects to handle window size or zoom changes
        int pw = 480, ph = 220;
        m_PopupRect = SDL_Rect{(GameWindow()->GetWidth() - pw) / 2, (GameWindow()->GetHeight() - ph) / 2, pw, ph};
        int bw = 160, bh = 56;
        m_PopupStartButton = SDL_Rect{m_PopupRect.x + 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh};
        m_PopupCancelButton = SDL_Rect{m_PopupRect.x + m_PopupRect.w - bw - 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh};
        // semi-transparent backdrop using SDL direct calls
        SDL_Renderer *renderer = GameWindow()->Renderer();
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
        SDL_RenderFillRect(renderer, nullptr);

        // popup box
        render->SetColor(30, 30, 40, 230);
        render->FillRect(m_PopupRect);

        // Draw header text and mock info using TextSurface like death overlay
        TextSurface titleTex(GameWindow()->Assetz(), GameWindow()->Assetz()->TextHandler()->GetMainFont(), "Enter Planet", {200, 200, 220});
        Texture *titleTexT = titleTex.RequestUpdate();
        if (titleTexT)
        {
            SDL_Rect dst{m_PopupRect.x + (m_PopupRect.w - titleTexT->GetWidth()) / 2, m_PopupRect.y + 16, titleTexT->GetWidth(), titleTexT->GetHeight()};
            render->RenderTexture(titleTexT->SDLTexture(), nullptr, dst);
        }

        // Mock details: difficulty/type/size
        char buf[128];
        snprintf(buf, sizeof(buf), "Type: Rocky\nDifficulty: Medium\nSize: %d", (int)m_ActivePlanet->Radius());
        TextSurface detailsTex(GameWindow()->Assetz(), GameWindow()->Assetz()->TextHandler()->GetMainFont(), buf, {200, 200, 200});
        Texture *detailsT = detailsTex.RequestUpdate();
        if (detailsT)
        {
            SDL_Rect dst{m_PopupRect.x + 28, m_PopupRect.y + 80, detailsT->GetWidth(), detailsT->GetHeight()};
            render->RenderTexture(detailsT->SDLTexture(), nullptr, dst);
        }

        // Start button
        render->SetColor(m_PopupStartHover ? 80 : 60, m_PopupStartHover ? 140 : 120, 80, 255);
        render->FillRect(m_PopupStartButton);
        TextSurface startText(GameWindow()->Assetz(), GameWindow()->Assetz()->TextHandler()->GetMainFont(), "Start", {10, 10, 10});
        Texture *startT = startText.RequestUpdate();
        if (startT)
        {
            SDL_Rect dst{m_PopupStartButton.x + (m_PopupStartButton.w - startT->GetWidth()) / 2, m_PopupStartButton.y + (m_PopupStartButton.h - startT->GetHeight()) / 2, startT->GetWidth(), startT->GetHeight()};
            render->RenderTexture(startT->SDLTexture(), nullptr, dst);
        }

        // Cancel button
        render->SetColor(m_PopupCancelHover ? 140 : 120, m_PopupCancelHover ? 80 : 60, 80, 255);
        render->FillRect(m_PopupCancelButton);
        TextSurface cancelText(GameWindow()->Assetz(), GameWindow()->Assetz()->TextHandler()->GetMainFont(), "Cancel", {10, 10, 10});
        Texture *cancelT = cancelText.RequestUpdate();
        if (cancelT)
        {
            SDL_Rect dst{m_PopupCancelButton.x + (m_PopupCancelButton.w - cancelT->GetWidth()) / 2, m_PopupCancelButton.y + (m_PopupCancelButton.h - cancelT->GetHeight()) / 2, cancelT->GetWidth(), cancelT->GetHeight()};
            render->RenderTexture(cancelT->SDLTexture(), nullptr, dst);
        }
    }
}
