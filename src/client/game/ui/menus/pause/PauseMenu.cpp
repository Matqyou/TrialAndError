//
// Created by mimmy
//

#include "PauseMenu.h"
#include "client/game/ui/elements/Button.h"
#include "client/game/GameReference.h"

static LoadTexture sTextureResume("interface.resume", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureBack("interface.back", AssetsClass::TexturePurpose::GUI_ELEMENT);

PauseMenu::PauseMenu()
{
	m_Paused = false;

	auto resume_button = (Button*)(new Button())
		->SetSize(Vec2i(200, 70))
		->SetAlign(Align::CENTER, Align::DONT)
		->SetDraw(DRAW_TEXTURE)
		->SetTexture(sTextureResume)
		->SetName("Resume");

	auto back_button = (Button*)(new Button())
		->SetSize(Vec2i(200, 70))
		->SetAlign(Align::CENTER, Align::DONT)
		->SetDraw(DRAW_TEXTURE)
		->SetTexture(sTextureBack)
		->SetName("Back");

	auto frame = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::HEIGHT, 10)
		->SetAlign(Align::CENTER, Align::CENTER)
		->AddChildren({ resume_button, back_button });

	SetDraw(DONT_DRAW);
	SetName("PauseMenu");
	AddChildren({ frame });
}

PauseMenu::~PauseMenu()
{
	// Clean up textures if necessary
}

//void PauseMenu::Show()
//{
//	SetAsCurrentMenu();
//	GameReference.World()->SetPaused(true);
//	m_Paused = true;
//}

//void PauseMenu::Event(const SDL_Event& event)
//{
//	switch (event.type)
//	{
////		case SDL_EVENT_QUIT:
////		{
////			Application.Destroy(true);
////
////			while (Mix_Playing(-1))
////			{
////			} // wait until last sound is done playing
////			delete m_GameWindow;
////			break;
////		}
//		case SDL_EVENT_MOUSE_BUTTON_DOWN:
//			if (event.button.button == SDL_BUTTON_LEFT)
//			{
//				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
//				int x = event.button.x;
//				int y = event.button.y;
//				if (x >= m_ResumeButtonRect.x && x < m_ResumeButtonRect.x + m_ResumeButtonRect.w &&
//					y >= m_ResumeButtonRect.y && y < m_ResumeButtonRect.y + m_ResumeButtonRect.h)
//				{
//					Assets.GetSound("ui.pitch.low")->PlaySound();
//					m_Paused = false;
//					GameReference.World()->SetPaused(false);
//				}
//				else if (x >= m_BackToMenuButtonRect.x && x < m_BackToMenuButtonRect.x + m_BackToMenuButtonRect.w &&
//					y >= m_BackToMenuButtonRect.y && y < m_BackToMenuButtonRect.y + m_BackToMenuButtonRect.h)
//				{
//					Assets.GetSound("ui.pitch.mid")->PlaySound();
//					m_Paused = false;
//					GameReference.DeleteWorld();
//					GameReference.Menu()->Show();
//				}
//			}
//			break;
//		case SDL_EVENT_MOUSE_MOTION:
//		{
//			int x = event.motion.x;
//			int y = event.motion.y;
//			bool hovering = false;
//			if ((x >= m_ResumeButtonRect.x && x < m_ResumeButtonRect.x + m_ResumeButtonRect.w &&
//				y >= m_ResumeButtonRect.y && y < m_ResumeButtonRect.y + m_ResumeButtonRect.h) ||
//				(x >= m_BackToMenuButtonRect.x && x < m_BackToMenuButtonRect.x + m_BackToMenuButtonRect.w &&
//					y >= m_BackToMenuButtonRect.y && y < m_BackToMenuButtonRect.y + m_BackToMenuButtonRect.h))
//				hovering = true;
//			if (hovering)
//				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
//			else
//				SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
//		}
//			break;
//		case SDL_EVENT_WINDOW_RESIZED:
//		{
//			m_ResumeButtonRect = { float(Application.GetWidth2()) - 100,
//								   float(Application.GetHeight2()) - 150,
//								   200, 70 };
//			m_BackToMenuButtonRect = { float(Application.GetWidth2()) - 100,
//									   float(Application.GetHeight2()) + 50,
//									   200, 70 };
//			Application.GetDrawing()->UpdateWindow();
//			break;
//		}
//		case SDL_EVENT_KEY_DOWN:
//		{
//			SDL_Scancode ScancodeKey = event.key.scancode;
//			if (ScancodeKey == SDL_SCANCODE_ESCAPE)
//			{
//				Assets.GetSound("ui.pitch.low")->PlaySound();
//				m_Paused = false;
//				GameReference.World()->SetPaused(false);
//				return;
//			}
//			break;
//		}
//	}
//}
//
//void PauseMenu::Render()
//{
//	auto drawing = Application.GetDrawing();
//	drawing->RenderTexture(m_TextureResume->SDLTexture(), nullptr, m_ResumeButtonRect);
//	drawing->RenderTexture(m_TextureBack->SDLTexture(), nullptr, m_BackToMenuButtonRect);
//	drawing->UpdateWindow();
//}