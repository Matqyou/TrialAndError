// PlanetEnterMenu.cpp
#include "PlanetEnterMenu.h"
#include <client/core/Assets.h>
#include <client/core/Drawing.h>
#include <client/game/world/planetary/PlanetaryGameWorld.h>
#include <client/game/entities/planetary/characters/character/PlanetaryCharacter.h>
#include <vector>

PlanetEnterMenu::PlanetEnterMenu(Interface *parent)
    : InterfaceElement(parent, true, false)
{
}

PlanetEnterMenu::~PlanetEnterMenu() = default;

void PlanetEnterMenu::Open(Planet *planet)
{
    m_ActivePlanet = planet;
    SetActive(true);
    // Pause the world while the menu is active
    if (GetInterface() && GetInterface()->GameWindow() && GetInterface()->GameWindow()->World())
        GetInterface()->GameWindow()->World()->SetPaused(true);
}

void PlanetEnterMenu::Close()
{
    m_ActivePlanet = nullptr;
    SetActive(false);
    // Unpause the world when closing the menu (if any)
    if (GetInterface() && GetInterface()->GameWindow() && GetInterface()->GameWindow()->World())
        GetInterface()->GameWindow()->World()->SetPaused(false);
}

void PlanetEnterMenu::Draw()
{
    if (!IsActive() || !m_ActivePlanet)
        return;

    GameData *gd = GetInterface()->GameWindow();
    Drawing *render = gd->Render();
    SDL_Renderer *renderer = gd->Renderer();

    // Recompute centered popup rects
    int pw = 480, ph = 220;
    m_PopupRect = SDL_Rect{(gd->GetWidth() - pw) / 2, (gd->GetHeight() - ph) / 2, pw, ph};
    int bw = 160, bh = 56;
    m_PopupStartButton = SDL_Rect{m_PopupRect.x + 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh};
    m_PopupCancelButton = SDL_Rect{m_PopupRect.x + m_PopupRect.w - bw - 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh};

    // update hover states
    int mx, my;
    Uint32 mstate = SDL_GetMouseState(&mx, &my);
    SDL_Point mouse{mx, my};
    m_PopupStartHover = SDL_PointInRect(&mouse, &m_PopupStartButton);
    m_PopupCancelHover = SDL_PointInRect(&mouse, &m_PopupCancelButton);

    // Fullscreen dim (less intense)
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 110);
    SDL_RenderFillRect(renderer, nullptr);

    // Panel background
    render->SetColor(18, 18, 28, 240);
    render->FillRect(m_PopupRect);

    // Title
    TextSurface titleTex(gd->Assetz(), gd->Assetz()->TextHandler()->GetMainFont(), "Enter Planet", {230, 230, 240});
    Texture *titleTexT = titleTex.RequestUpdate();
    if (titleTexT)
    {
        int tw = (int)(titleTexT->GetWidth() * 2.0);
        int th = (int)(titleTexT->GetHeight() * 2.0);
        SDL_Rect dst{m_PopupRect.x + (m_PopupRect.w - tw) / 2, m_PopupRect.y + 16, tw, th};
        render->RenderTexture(titleTexT->SDLTexture(), nullptr, dst);
    }

    // Details
    char buf[256];
    auto planetPos = m_ActivePlanet->GetCore().Pos;
    std::snprintf(buf, sizeof(buf), "Radius: %d\nPosition: (%.0f, %.0f)",
                  (int)m_ActivePlanet->Radius(), planetPos.x, planetPos.y);
    TextSurface detailsTex(gd->Assetz(), gd->Assetz()->TextHandler()->GetMainFont(), buf, {200, 200, 200});
    Texture *detailsT = detailsTex.RequestUpdate();
    if (detailsT)
    {
        SDL_Rect dst{m_PopupRect.x + 28, m_PopupRect.y + 90, detailsT->GetWidth() * 2, detailsT->GetHeight() * 2};
        render->RenderTexture(detailsT->SDLTexture(), nullptr, dst);
    }

    // Buttons
    render->SetColor(m_PopupStartHover ? 70 : 50, m_PopupStartHover ? 140 : 120, 70, 255);
    render->FillRect(m_PopupStartButton);
    TextSurface startText(gd->Assetz(), gd->Assetz()->TextHandler()->GetMainFont(), "Enter", {10, 10, 10});
    Texture *startT = startText.RequestUpdate();
    if (startT)
    {
        int sw = (int)(startT->GetWidth() * 1.5);
        int sh = (int)(startT->GetHeight() * 1.5);
        SDL_Rect dst{m_PopupStartButton.x + (m_PopupStartButton.w - sw) / 2, m_PopupStartButton.y + (m_PopupStartButton.h - sh) / 2, sw, sh};
        render->RenderTexture(startT->SDLTexture(), nullptr, dst);
    }

    render->SetColor(m_PopupCancelHover ? 140 : 120, m_PopupCancelHover ? 70 : 50, 70, 255);
    render->FillRect(m_PopupCancelButton);
    TextSurface cancelText(gd->Assetz(), gd->Assetz()->TextHandler()->GetMainFont(), "Cancel", {10, 10, 10});
    Texture *cancelT = cancelText.RequestUpdate();
    if (cancelT)
    {
        int cw = (int)(cancelT->GetWidth() * 1.5);
        int ch = (int)(cancelT->GetHeight() * 1.5);
        SDL_Rect dst{m_PopupCancelButton.x + (m_PopupCancelButton.w - cw) / 2, m_PopupCancelButton.y + (m_PopupCancelButton.h - ch) / 2, cw, ch};
        render->RenderTexture(cancelT->SDLTexture(), nullptr, dst);
    }

    // Handle clicks (left button)
    if (mstate & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        if (m_PopupStartHover)
        {
            GameData *gd = GetInterface()->GameWindow();
            PlanetaryGameWorld *pgw = new PlanetaryGameWorld(gd, 50, 30);

            std::vector<Player *> players;
            for (Player *p = gd->World()->FirstPlayer(); p; p = p->Next())
                players.push_back(p);

            for (Player *p : players)
            {
                Character *oldChar = p->GetCharacter();
                GameController *oldController = nullptr;
                if (oldChar)
                    oldController = oldChar->GetGameController();

                // Clear player's character pointer so new character can be set safely
                p->SetCharacter(nullptr);

                // Move player to the new world
                gd->World()->RemovePlayer(p);
                p->SetGameWorld(pgw);
                pgw->AddPlayer(p);

                // Delete the old character (now the player's pointer is cleared)
                if (oldChar)
                    delete oldChar;

                // Create a new planetary character in the new world and restore controller
                Vec2d worldCenter(pgw->GetWidth() / 2.0, pgw->GetHeight() / 2.0);
                auto newChar = new PlanetaryCharacter(pgw, p, 100.0, worldCenter, Vec2d(10, 10), false);
                if (oldController && newChar)
                    newChar->SetGameController(oldController);
            }

            // Switch the active world (this deletes the old world safely)
            gd->SetWorld(pgw);
            Close();
        }
        else if (m_PopupCancelHover)
        {
            // Nudge player away from planet
            if (m_ActivePlanet)
            {
                Player *pl = GetInterface()->GameWindow()->World()->FirstPlayer();
                if (pl)
                {
                    Character *c = pl->GetCharacter();
                    if (c)
                    {
                        Vec2d planetPos = m_ActivePlanet->GetCore().Pos;
                        Vec2d ppos = c->GetCore().Pos;
                        Vec2d dir = ppos - planetPos;
                        if (dir.Length() == 0)
                            dir = Vec2d(1, 0);
                        dir.Normalize();
                        c->GetCore().Vel = dir * 20;
                    }
                }
            }
            Close();
        }
    }
}
