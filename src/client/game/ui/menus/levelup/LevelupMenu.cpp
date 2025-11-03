//
// Created by mimmy
//

#include "LevelupMenu.h"
#include "client/game/ui/advanced_elements/StarBackground.h"
#include "client/game/ui/elements/Button.h"
#include "client/game/ui/elements/TextElement.h"
#include "client/game/ui/menus/Menus.h"
#include "client/game/powerups/PowerupManager.h"
#include "client/game/GameReference.h"
#include "client/game/ui/CommonUI.h"
#include "client/game/players/Player.h"
#include "client/core/Application.h"
#include <shared/utility/Rectangles.h>

#include <algorithm>
#include <cstdlib>
#include <vector>

static LinkTexture sTextureFrame("interface.menu");
static LinkTexture sTextureBack("ui.gamemodemenu.backbutton");
static LinkTexture sTextureAllStats("interface.permanenterrors.allstats");
static LinkTexture sTextureBombs("interface.permanenterrors.bombs");
static LinkTexture sTextureBossDamage("interface.permanenterrors.bossdamage");
static LinkTexture sTextureDoubleDamage("interface.permanenterrors.doubledamage");
static LinkTexture sTextureExplosiveAmmo("interface.permanenterrors.explosiveammo");
static LinkTexture sTextureSpeed("interface.permanenterrors.speed");
static LinkTexture sTextureSpiky("interface.permanenterrors.spiky");
static LinkTexture sTextureHealth("interface.permanenterrors.health");
static LinkTexture sTextureInfiniteGlockAmmo("interface.permanenterrors.infiniteglockammo");
static LinkTexture sTextureErrorOutline("interface.permanenterrors.regularoutlinefull");


LevelupMenu::LevelupMenu()
	: FullscreenMenu()
{
	m_powerupTextures = {sTextureAllStats, sTextureBossDamage, sTextureSpeed, sTextureSpiky, sTextureHealth};
	m_specialPowerupTextures = {sTextureBombs, sTextureBossDamage, sTextureDoubleDamage, sTextureExplosiveAmmo, sTextureInfiniteGlockAmmo};
	srand(static_cast<unsigned int>(time(nullptr)));
	SetName("LevelupMenu");
}

LevelupMenu::~LevelupMenu() = default;

void LevelupMenu::SwitchToThisMenu()
{
	GameReference.World()->SetPaused(true);
	m_Paused = true;

	auto world = GameReference.World();
	if (!world)
		return;

	// --- Find the player that triggered the levelup ---
	Player* selectedPlayer = nullptr;
	for (auto* player : GameReference.GetPlayers())
	{
		if (player && player->PowerupToBeSelected())
		{
			selectedPlayer = player;
			break;
		}
	}

	if (!selectedPlayer)
	{
		GameReference.World()->SetPaused(false);
		return;
	}

	// --- Build eligible powerups for this player ---
	std::vector<Texture*> eligiblePowerups;
	int num_powerup_types = static_cast<int>(Powerup::NUM_POWERUPS);

	for (int i = 0; i < num_powerup_types; i++)
	{
		auto powerup = static_cast<Powerup>(i);
		int count = selectedPlayer->GetPowerupUpgradeCount(powerup);

		if (count >= 10 && i < (int)m_specialPowerupTextures.size())
			eligiblePowerups.push_back(m_specialPowerupTextures[i]);
		else if (i < (int)m_powerupTextures.size())
			eligiblePowerups.push_back(m_powerupTextures[i]);
	}

	// --- Randomly select 3 unique powerups ---
	std::vector<int> selected;
	while (selected.size() < 3 && selected.size() < eligiblePowerups.size())
	{
		int index = rand() % eligiblePowerups.size();
		if (std::find(selected.begin(), selected.end(), index) == selected.end())
			selected.push_back(index);
	}

	m_selectedIndices = selected;

	auto star_bg = new StarBackground();
	std::vector<Element*> powerupFrames;

	// --- Create 3 framed buttons with overlay + description ---
	for (int i = 0; i < (int)m_selectedIndices.size(); ++i)
	{
		int idx = m_selectedIndices[i];
		Texture* tex = eligiblePowerups[idx];

		auto iconElement = (new Element())
			->SetRelative(Vec2i(100,100))
			->SetSize(Vec2i(125, 125))
			->SetDraw(DRAW_TEXTURE)
			->SetTexture(tex);

		auto textElement = (new TextElement())
			->UpdateTextOutline(CommonUI::fontDefault,
								"This powerup does something very cool!",
								2,
								{ 0, 0, 0, 255 },
								{ 255, 255, 255, 255 })
			->SetRelative(Vec2i(70,330));

		auto originalSize = textElement->GetSize();
		textElement->SetSize(Rectangles::ScaleToBounds(Vec2i(originalSize), Vec2i(175, 150)));
		// === Powerup Icon Button with Overlay ===
		auto powerupButton = (Button*)(new Button(
			sTextureErrorOutline.GetTexture(),
			sTextureErrorOutline.GetTexture()
		))
			->SetSize(Vec2i(320, 640))
			->SetDraw(DRAW_TEXTURE)
			->SetAlign(Align::CENTER, Align::DONT)
			->AddChildren({ iconElement, textElement })
			->SetName("PowerupButton");

		powerupButton->SetCallback([selectedPlayer, idx, this]()
		{
			CommonUI::soundUiPitchMid.GetSound()->PlaySound();
			selectedPlayer->AddPowerupUpgrade(static_cast<Powerup>(idx));
			m_Paused = false;
			GameReference.World()->SetPaused(false);
			Menus.levelup_menu->SwitchToThisMenu();
		});


		auto frame = (new Element())
			->SetAdaptive(true, true)
			->AddChildren({ powerupButton });

		powerupFrames.push_back(frame);
	}

	// === Horizontal Layout (Left, Center, Right) ===
	auto buttons_row = (new Element())
		->SetAdaptive(true, true)
		->SetAlign(Align::CENTER, Align::CENTER)
		->SetFlex(Flex::WIDTH, 60)
		->AddChildren(powerupFrames)
		->SetName("LevelupButtonsRow");

	auto frame = (new Element())
		->SetFullyOccupy(true, true)
		->SetAlign(Align::CENTER, Align::CENTER)
		->AddChildren({ buttons_row })
		->SetName("LevelupFrame");

	AddChildren({ star_bg, frame });
	Menus.SetCurrentMenu(this);
	RefreshMenu();
}

void LevelupMenu::HandleEvent(const SDL_Event &sdl_event, EventContext &event_summary)
{
	if (sdl_event.type == SDL_EVENT_KEY_DOWN && sdl_event.key.scancode == SDL_SCANCODE_ESCAPE)
	{
		CommonUI::soundUiPitchLow.GetSound()->PlaySound();
		m_Paused = false;
		GameReference.World()->SetPaused(false);
		Menus.levelup_menu->SwitchToThisMenu();
	}
	HandleEventChildren(sdl_event, event_summary);
	FullscreenMenuEvent(sdl_event, event_summary);
}

void LevelupMenu::Tick(double elapsed_seconds)
{
	TickChildren(elapsed_seconds);
}

void LevelupMenu::Render()
{
	RenderChildren();
}
