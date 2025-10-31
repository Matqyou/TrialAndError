// PlanetEnterMenu.cpp
#include "PlanetEnterMenu.h"
#include <client/game/entities/planetary/characters/character/PlanetaryCharacter.h>
#include <client/game/world/planetary/PlanetaryGameWorld.h>
#include <client/game/ui/CommonUI.h>
#include <client/core/Assets.h>

#include <shared/utility/Rectangles.h>
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
	if (GetInterface())
		GameReference.World()->SetPaused(true);
}

void PlanetEnterMenu::Close()
{
	m_ActivePlanet = nullptr;
	SetActive(false);
	// Unpause the world when closing the menu (if any)
	if (GetInterface())
		GameReference.World()->SetPaused(false);
}

void PlanetEnterMenu::Draw()
{
	if (!IsActive() || !m_ActivePlanet)
		return;

	Drawing *render = Application.GetDrawing();
	SDL_Renderer *renderer = Application.GetRenderer();

	// Recompute centered popup rects
	float pw = 480, ph = 220;
	m_PopupRect = SDL_FRect{ ((float)Application.GetWidth() - pw) / 2.0f, ((float)Application.GetHeight() - ph) / 2.0f, pw, ph };
	float bw = 160, bh = 56;
	m_PopupStartButton = SDL_FRect{ m_PopupRect.x + 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh };
	m_PopupCancelButton = SDL_FRect{ m_PopupRect.x + m_PopupRect.w - bw - 40, m_PopupRect.y + m_PopupRect.h - bh - 24, bw, bh };

	// update hover states
	float mx, my;
	Uint32 mouse_state = SDL_GetMouseState(&mx, &my);
	SDL_Point mouse{ static_cast<int>(mx), static_cast<int>(my) };
	m_PopupStartHover = Rectangles::PointCollides(mx, my,
												  m_PopupStartButton.x, m_PopupStartButton.y,
												  m_PopupStartButton.w, m_PopupStartButton.h);
	m_PopupStartHover = Rectangles::PointCollides(mx, my,
												  m_PopupStartButton.x, m_PopupStartButton.y,
												  m_PopupStartButton.w, m_PopupStartButton.h);
//	m_PopupStartHover = SDL_PointInRect(&mouse, &m_PopupStartButton);
//	m_PopupCancelHover = SDL_PointInRect(&mouse, &m_PopupCancelButton);

	// Fullscreen dim (less intense)
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 110);
	SDL_RenderFillRect(renderer, nullptr);

	// Panel background
	render->SetColor(18, 18, 28, 240);
	render->DrawRect(m_PopupRect, true);
//	render->FillRect(m_PopupRect);

	// Title
	TextSurface titleTex(CommonUI::fontDefault, "Enter Planet", { 230, 230, 240 });
	Texture *titleTexT = titleTex.RequestUpdate();
	if (titleTexT)
	{
		float tw = titleTexT->GetWidth() * 2.0f;
		float th = titleTexT->GetHeight() * 2.0f;
		SDL_FRect dst{ m_PopupRect.x + (m_PopupRect.w - tw) / 2.0f, m_PopupRect.y + 16.0f, tw, th };
		render->RenderTexture(titleTexT->SDLTexture(), nullptr, dst);
	}

	// Details
	char buf[256];
	auto planetPos = m_ActivePlanet->GetCore().pos;
	std::snprintf(buf, sizeof(buf), "Radius: %d\nPosition: (%.0f, %.0f)",
				  (int)m_ActivePlanet->Radius(), planetPos.x, planetPos.y);
	TextSurface detailsTex(CommonUI::fontDefault, buf, { 200, 200, 200 });
	Texture *detailsT = detailsTex.RequestUpdate();
	if (detailsT)
	{
		SDL_FRect dst{ m_PopupRect.x + 28.0f, m_PopupRect.y + 90.0f, detailsT->GetWidth() * 2, detailsT->GetHeight() * 2 };
		render->RenderTexture(detailsT->SDLTexture(), nullptr, dst);
	}

	// Buttons
	render->SetColor(m_PopupStartHover ? 70 : 50, m_PopupStartHover ? 140 : 120, 70, 255);
	render->DrawRect(m_PopupStartButton, true);
	TextSurface startText(CommonUI::fontDefault, "Enter", { 10, 10, 10 });
	Texture *startT = startText.RequestUpdate();
	if (startT)
	{
		float sw = startT->GetWidth() * 1.5f;
		float sh = startT->GetHeight() * 1.5f;
		SDL_FRect dst{ m_PopupStartButton.x + (m_PopupStartButton.w - sw) / 2, m_PopupStartButton.y + (m_PopupStartButton.h - sh) / 2, sw, sh };
		render->RenderTexture(startT->SDLTexture(), nullptr, dst);
	}

	render->SetColor(m_PopupCancelHover ? 140 : 120, m_PopupCancelHover ? 70 : 50, 70, 255);
	render->DrawRect(m_PopupCancelButton, true);
	TextSurface cancelText(CommonUI::fontDefault, "Cancel", { 10, 10, 10 });
	Texture *cancelT = cancelText.RequestUpdate();
	if (cancelT)
	{
		float cw = cancelT->GetWidth() * 1.5f;
		float ch = cancelT->GetHeight() * 1.5f;
		SDL_FRect dst{ m_PopupCancelButton.x + (m_PopupCancelButton.w - cw) / 2, m_PopupCancelButton.y + (m_PopupCancelButton.h - ch) / 2, cw, ch };
		render->RenderTexture(cancelT->SDLTexture(), nullptr, dst);
	}

	// Handle clicks (left button)
	if (mouse_state & SDL_BUTTON_MASK(SDL_BUTTON_LEFT))
	{
		if (m_PopupStartHover)
		{
			GameWorld* old_world = GameReference.World();
			PlanetaryGameWorld *new_world = new PlanetaryGameWorld(50, 30);

			auto players = old_world->GetPlayers();
			for (Player *player : players)
			{
				Character *oldChar = player->GetCharacter();
//				GameController *oldController = nullptr;
//				if (oldChar)
//					oldController = oldChar->GetGameController();

				// Clear player's character pointer so new character can be set safely
				player->SetCharacter(nullptr);

				// Move player to the new world
				old_world->RemovePlayer(player);
				player->SetGameWorld(new_world);
				new_world->AddPlayer(player);

				// Delete the old character (now the player's pointer is cleared)
				if (oldChar)
					delete oldChar;

				// Create a new planetary character in the new world and restore controller
				Vec2f worldCenter(new_world->GetWidth() / 2.0f, new_world->GetHeight() / 2.0f);
				auto newChar = new PlanetaryCharacter(new_world, player, 100.0, worldCenter, Vec2f(10.0f, 10.0f), false);
//				if (oldController && newChar)
//					newChar->SetGameController(oldController);
			}

			// Switch the active world (this deletes the old world safely)
			GameReference.SetWorld(new_world);
			Close();
		}
		else if (m_PopupCancelHover)
		{
			// Nudge player away from planet
			if (m_ActivePlanet)
			{
				Player *player = (Player*)GameReference.World()->GetPlayers().front();
				if (player)
				{
					Character *character = player->GetCharacter();
					if (character)
					{
						Vec2f planet_pos = m_ActivePlanet->GetCore().pos;
						Vec2f character_pos = character->GetCore().pos;

						Vec2f nudge_direction = character_pos - planet_pos;
						if (nudge_direction.Length() == 0)
							nudge_direction = Vec2f(1, 0);

						float nudge_strength = 20.0f;
						character->GetCore().vel = nudge_direction.Normalize() * nudge_strength;
					}
				}
			}
			Close();
		}
	}
}
