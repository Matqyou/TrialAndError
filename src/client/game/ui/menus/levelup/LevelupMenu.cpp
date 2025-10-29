//
// Created by mimmy
//

#include "client/game/powerups/PowerupManager.h"
#include "client/game/ui/menus/Menus.h"
#include "client/game/GameReference.h"
#include "client/core/Application.h"
#include "client/game/ui/CommonUI.h"
#include "client/game/Player.h"
#include "LevelupMenu.h"
#include <cstdlib>
#include <vector>
#include <ctime>

LevelupMenu::LevelupMenu()
{
//	m_ErrorIconRect = { 0, 0, float(Application.GetWidth2() * 0.4), float(Application.GetHeight2() * 0.4) };
//	m_ErrorOutlineRect = { 0, 0, float(Application.GetWidth2() / 2.2), float(Application.GetHeight2() / 0.6) };
//	m_powerupTextures = { m_TextureAllStats, m_TextureBossDamage, m_TextureSpeed, m_TextureSpiky, m_TextureHealth };
//	m_specialPowerupTextures = { m_TextureBombs, m_TextureBossDamage, m_TextureDoubleDamage, m_TextureExplosiveAmmo,  m_TextureInfiniteGlockAmmo };
//	srand(static_cast<unsigned int>(time(nullptr)));

	auto frame = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::HEIGHT, 10)
		->SetAlign(Align::CENTER, Align::CENTER);

	SetName("LevelupMenu");
	AddChildren({ frame });
}

LevelupMenu::~LevelupMenu()
{

}

void LevelupMenu::SwitchToThisMenu()
{
	GameReference.World()->SetDelay(true);
	GameReference.World()->SetPaused(true);
	m_Paused = true;

	std::vector<Texture *> eligiblePowerups;
	auto num_powerup_types = (int)Powerup::NUM_POWERUPS;
//	for (int i = 0; i < num_powerup_types; i++)
//	{
//		auto current_powerup = (Powerup)i;
//		int count = m_Player->GetPowerupUpgradeCount(current_powerup);
//		if (count == 10)
//			eligiblePowerups.push_back(m_specialPowerupTextures[i]);
//		else
//			eligiblePowerups.push_back(m_powerupTextures[i]);
//	}

	// Randomly select 3 unique indices from eligible list
	std::vector<int> selected;
	while (selected.size() < 3 && selected.size() < eligiblePowerups.size())
	{
		int index = rand() % eligiblePowerups.size();
		if (std::find(selected.begin(), selected.end(), index) == selected.end())
			selected.push_back(index);
	}

	m_selectedIndices = selected;

	Menus.SetCurrentMenu(this);
	RefreshMenu();
}

//void LevelUpMenu::Event(const SDL_Event& event)
//{
//	auto world = GameReference.World();
//	if (world->GetDelay())
//	{
//		// Ignore events during delay
//		return;
//	}
//
//	switch (event.type)
//	{
////		case SDL_EVENT_QUIT:
////		{
////			m_GameWindow->Deinitialize(true);
////			while (Mix_Playing(-1))
////			{
////			}
////			delete m_GameWindow;
////			break;
////		}
//		case SDL_EVENT_MOUSE_BUTTON_DOWN:
//			if (event.button.button == SDL_BUTTON_LEFT)
//			{
//				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
//				int x = event.button.x;
//				int y = event.button.y;
//				for (int i = 0; i < m_selectedIndices.size(); ++i)
//				{
//					SDL_FRect rect = m_ErrorOutlineRect;
//					rect.x = float(Application.GetWidth2() / 12) + i * (rect.w + float(Application.GetWidth2() * 0.25));
//					rect.y = float(Application.GetHeight2() / 6);
//					if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h)
//					{
//						Assets.GetSound("ui.pitch.mid")->PlaySound();
//
//						// Apply the selected powerup
//						switch (m_selectedIndices[i])
//						{
//							case 0: ApplyAllStats();
//								break;
//							case 1: ApplyBombs();
//								break;
//							case 2: ApplyDoubleDamage();
//								break;
//							case 3: ApplyBossDamage();
//								break;
//							case 4: ApplyExplosiveAmmo();
//								break;
//							case 5: ApplySpeed();
//								break;
//							case 6: ApplySpiky();
//								break;
//							case 7: ApplyHealth();
//								break;
//							case 8: ApplyInfiniteGlockAmmo();
//								break;
//						}
//						// m_Player->GetCharacter(); // Example method to increase health
//						m_Paused = false;
//						world->SetPaused(false);
//						world->CheckLevelUps();
//						return;
//					}
//				}
//			}
//			break;
//		case SDL_EVENT_MOUSE_MOTION:
//		{
//			int x = event.motion.x;
//			int y = event.motion.y;
//			bool hovering = false;
//			for (int i = 0; i < m_selectedIndices.size(); ++i)
//			{
//				SDL_FRect rect = m_ErrorOutlineRect;
//				rect.x = float(Application.GetWidth2() / 12) + i * (rect.w + float(Application.GetWidth2() * 0.25));
//				rect.y = float(Application.GetHeight2() / 6);
//				if (x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h / 1.2)
//				{
//					hovering = true;
//					break;
//				}
//			}
//			if (hovering)
//				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
//			else
//				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
//			break;
//		}
//		case SDL_EVENT_WINDOW_RESIZED:
//		{
//			// Adjust positions and sizes of powerup textures
//			int startX = int(Application.GetWidth2() / 12);
//			int startY = int(Application.GetHeight2() / 6);
//			int spacing = int(Application.GetWidth2() * 0.25);
//
//			for (int i = 0; i < m_selectedIndices.size(); ++i)
//			{
//				SDL_FRect& rect = m_ErrorOutlineRect;
//				rect.x = startX + i * (rect.w + spacing);
//				rect.y = startY;
//				rect.w = float(Application.GetWidth2() / 2.2);
//				rect.h = float(Application.GetHeight2() / 0.6);
//			}
//			break;
//		}
//		case SDL_EVENT_KEY_DOWN:
//		{
//			SDL_Scancode ScancodeKey = event.key.scancode;
//			if (ScancodeKey == SDL_SCANCODE_ESCAPE)
//			{
//				Assets.GetSound("ui.pitch.low")->PlaySound();
//				m_Paused = false;
//				world->SetPaused(false);
//				world->CheckLevelUps();
//			}
//
//			break;
//		}
//	}
//}
//
//void LevelUpMenu::Render()
//{
//	auto drawing = Application.GetDrawing();
//	int startX = int(Application.GetWidth2() / 12);
//	int startY = int(Application.GetHeight2() / 6);
//	int spacing = int(Application.GetWidth2() * 0.25);
//
//	for (int i = 0; i < m_selectedIndices.size(); ++i)
//	{
//		SDL_FRect rect = m_ErrorOutlineRect;
//		Texture *texture = m_powerupTextures[m_selectedIndices[i]];
//
//		// Adjust position and size
//		rect.x = startX + i * (rect.w + spacing);
//		rect.y = startY;
//
//		// Draw ErrorOutline texture as the base
//		drawing->RenderTexture(m_TextureErrorOutline->SDLTexture(), nullptr, rect);
//
//		m_ErrorIconRect.x = rect.x + float(rect.w * 0.3);
//		m_ErrorIconRect.y = rect.y + float(rect.h * 0.15);
//		m_ErrorIconRect.w = float(rect.w * 0.4);
//		m_ErrorIconRect.h = float(rect.h * 0.225);
//		// Draw powerup texture
//
//		drawing->SetDrawBlendMode(SDL_BLENDMODE_BLEND);
//		drawing->RenderTexture(texture->SDLTexture(), nullptr, m_ErrorIconRect);
//		// Draw description below the powerup
//		SDL_FRect descriptionRect = { rect.x + float(rect.w / 4), float((rect.y + rect.h) / 1.9),
//									  rect.w - float(rect.w / 2.25), rect.h - float(rect.h / 1.35) };
//		drawing->SetColor(0, 0, 0, 255);
//
//		auto text = Assets.RenderTextBlended(CommonFonts::fontBig,
//											 "This powerup does something very very cool and you will never know what hehehehehehehehehe + nerd",
//											 { 0, 0, 0, 255 });
//		drawing->RenderTexture(text->SDLTexture(), nullptr, descriptionRect);
//		delete text;
//	}
//	drawing->UpdateWindow();
//}
//
//// TODO seperate these between the ones that are applied after max and ones that are shown before
//// Powerup effect functions
//void LevelUpMenu::ApplyAllStats()
//{
//	// Implement the effect of the AllStats powerup
//	std::cout << "AllStats powerup applied" << std::endl;
//	m_Player->AddPowerupUpgrade("AllStats");
//}
//
//void LevelUpMenu::ApplyBombs()
//{
//	// Implement the effect of the Bombs powerup
//	std::cout << "Bombs powerup applied" << std::endl;
//	m_Player->AddPowerupUpgrade("Bombs");
//}
//
//void LevelUpMenu::ApplyDoubleDamage()
//{
//	// Implement the effect of the DoubleDamage powerup
//	std::cout << "DoubleDamage powerup applied" << std::endl;
//	m_Player->IncreaseDamageAmp(2.0);
//}
//
//void LevelUpMenu::ApplyBossDamage()
//{
//	// Implement the effect of the BossDamage powerup
//	std::cout << "BossDamage powerup applied" << std::endl;
//	m_Player->IncreaseBossDamageAmp(0.2);
//	m_Player->AddPowerupUpgrade("BossDamage");
//}
//
//void LevelUpMenu::ApplyExplosiveAmmo()
//{
//	// Implement the effect of the ExplosiveAmmo powerup
//	std::cout << "ExplosiveAmmo powerup applied" << std::endl;
//}
//
//void LevelUpMenu::ApplyExtraLives()
//{
//	// Implement the effect of the ExtraLives powerup
//	std::cout << "ExtraLives powerup applied" << std::endl;
//	m_Player->SetExtraLife(true);
//}
//
//void LevelUpMenu::ApplySpeed()
//{
//	// Implement the effect of the Speed powerup
//	std::cout << "Speed powerup applied" << std::endl;
//	// m_Player->IncreaseSpeed(1.1);
//}
//
//void LevelUpMenu::ApplySpiky()
//{
//	// Implement the effect of the Spiky powerup
//	std::cout << "Spiky powerup applied" << std::endl;
//	m_Player->AddPowerupUpgrade("Spiky");
//}
//
//void LevelUpMenu::ApplyHealth()
//{
//	// Implement the effect of the Health powerup
//	std::cout << "Health powerup applied" << std::endl;
//	m_Player->IncreaseMaxHealthAmp(1.1);
//	m_Player->AddPowerupUpgrade("Health");
//}
//
//void LevelUpMenu::ApplyInfiniteGlockAmmo()
//{
//	// Implement the effect of the InfiniteGlockAmmo powerup
//	std::cout << "InfiniteGlockAmmo powerup applied" << std::endl;
//}