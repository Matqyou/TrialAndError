// CartesianGameWorld.cpp
#include "CartesianGameWorld.h"
#include "client/core/Assets.h"
#include "entities/characters/character/Character.h"
#include "client/core/Drawing.h"
#include "../game/planetary/PlanetaryGameWorld.h"
#include "entities/planetary/PlanetaryCharacter.h"

CartesianGameWorld::CartesianGameWorld(GameData *game_window, int width, int height)
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

CartesianGameWorld::~CartesianGameWorld()
{
    for (auto *p : m_PlanetEntities)
        delete p;
}

void CartesianGameWorld::Tick()
{
    GameWorld::Tick();

    if (!FirstPlayer())
        return;
    Character *c = FirstPlayer()->GetCharacter();
    if (!c)
        return;

    Vec2d ppos = c->GetCore().Pos;

    if (m_ShowEnterPopup && m_ActivePlanet)
    {
        int mx, my;
        Uint32 mstate = SDL_GetMouseState(&mx, &my);
        SDL_Point mouse{mx, my};
        m_PopupStartHover = SDL_PointInRect(&mouse, &m_PopupStartButton);
        m_PopupCancelHover = SDL_PointInRect(&mouse, &m_PopupCancelButton);

        if (mstate & SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            if (m_PopupStartHover)
            {
                GameData *gd = GameWindow();
                PlanetaryGameWorld *pgw = new PlanetaryGameWorld(gd, 50, 30);

                std::vector<Player *> players;
                for (Player *p = FirstPlayer(); p; p = p->Next())
                {
                    players.push_back(p);
                }

                for (Player *p : players)
                {
                    Character *oldChar = p->GetCharacter();
                    GameController *oldController = nullptr;
                    if (oldChar)
                        oldController = oldChar->GetGameController();

                    RemovePlayer(p);
                    p->SetGameWorld(pgw);
                    pgw->AddPlayer(p);

                    if (oldChar)
                        delete oldChar;

                    Vec2d worldCenter(pgw->GetWidth() / 2.0, pgw->GetHeight() / 2.0);
                    auto newChar = new PlanetaryCharacter(pgw, p, 100.0, worldCenter, Vec2d(10, 10), false);
                    if (oldController && newChar)
                        newChar->SetGameController(oldController);
                }

                gd->SetWorld(pgw);
            }
            else if (m_PopupCancelHover)
            {
                m_ShowEnterPopup = false;
                if (m_ActivePlanet)
                {
                    Vec2d planetPos = m_ActivePlanet->GetCore().Pos;
                    Vec2d dir = ppos - planetPos;
                    if (dir.Length() == 0)
                        dir = Vec2d(1, 0);
                    dir.Normalize();
                    c->GetCore().Vel = dir * 20;
                }
            }
        }

        return;
    }

    for (auto *pe : m_PlanetEntities)
    {
        Vec2d d = ppos - pe->GetCore().Pos;
        if (d.Length() < pe->Radius() + 8)
        {
            m_ShowEnterPopup = true;
            m_ActivePlanet = pe;
            break;
        }
    }
}

void CartesianGameWorld::Draw()
{
    Drawing *render = GameWindow()->Render();
    SDL_Renderer *r = GameWindow()->Renderer();
    render->SetColor(10, 10, 30, 255);
    render->Clear();

    for (auto *pe : m_PlanetEntities)
    {
        pe->Draw();
    }

    GameWorld::Draw();

    if (m_ShowEnterPopup && m_ActivePlanet)
    {
        int pw = 480, ph = 220;
        m_PopupRect = SDL_Rect{(GameWindow()->GetWidth() - pw) / 2, (GameWindow()->GetHeight() - ph) / 2, pw, ph};
        int bw = 160, bh = 56;
        m_PopupStartButton = SDL_Rect{m_PopupRect.x + 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh};
        m_PopupCancelButton = SDL_Rect{m_PopupRect.x + m_PopupRect.w - bw - 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh};

        SDL_Renderer *renderer = GameWindow()->Renderer();
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
        SDL_RenderFillRect(renderer, nullptr);

        render->SetColor(30, 30, 40, 230);
        render->FillRect(m_PopupRect);

        TextSurface titleTex(GameWindow()->Assetz(), GameWindow()->Assetz()->TextHandler()->GetMainFont(), "Enter Planet", {200, 200, 220});
        Texture *titleTexT = titleTex.RequestUpdate();
        if (titleTexT)
        {
            SDL_Rect dst{m_PopupRect.x + (m_PopupRect.w - titleTexT->GetWidth()) / 2, m_PopupRect.y + 16, titleTexT->GetWidth(), titleTexT->GetHeight()};
            render->RenderTexture(titleTexT->SDLTexture(), nullptr, dst);
        }

        char buf[128];
        snprintf(buf, sizeof(buf), "Type: Rocky\nDifficulty: Medium\nSize: %d", (int)m_ActivePlanet->Radius());
        TextSurface detailsTex(GameWindow()->Assetz(), GameWindow()->Assetz()->TextHandler()->GetMainFont(), buf, {200, 200, 200});
        Texture *detailsT = detailsTex.RequestUpdate();
        if (detailsT)
        {
            SDL_Rect dst{m_PopupRect.x + 28, m_PopupRect.y + 80, detailsT->GetWidth(), detailsT->GetHeight()};
            render->RenderTexture(detailsT->SDLTexture(), nullptr, dst);
        }

        render->SetColor(m_PopupStartHover ? 80 : 60, m_PopupStartHover ? 140 : 120, 80, 255);
        render->FillRect(m_PopupStartButton);
        TextSurface startText(GameWindow()->Assetz(), GameWindow()->Assetz()->TextHandler()->GetMainFont(), "Start", {10, 10, 10});
        Texture *startT = startText.RequestUpdate();
        if (startT)
        {
            SDL_Rect dst{m_PopupStartButton.x + (m_PopupStartButton.w - startT->GetWidth()) / 2, m_PopupStartButton.y + (m_PopupStartButton.h - startT->GetHeight()) / 2, startT->GetWidth(), startT->GetHeight()};
            render->RenderTexture(startT->SDLTexture(), nullptr, dst);
        }

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
