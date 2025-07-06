// src/game/interface/GameModeMenu.cpp

#include "ClassSelectMenu.h"
#include "client/game/classes/PlayerClass.h"
#include "GamemodeMenu.h"
#include "client/game/ui/elements/Button.h"
#include "client/game/ui/advanced_elements/StarBackground.h"
#include "client/game/ui/CommonUI.h"

static LinkTexture sMenuTexture("interface.menu");
static LinkTexture sTextureTitle("ui.classselectmenu.title");

// TODO Update the class buttons to be icons of the class and have text descriptions on hover
static LinkTexture sTextureClassButton1("ui.classselectmenu.humanclassbutton");
static LinkTexture sTextureClassButton2("ui.classselectmenu.zombieclassbutton");
static LinkTexture sTextureClassButton3("ui.classselectmenu.cyborgclassbutton");
static LinkTexture sTextureClassButton4("ui.classselectmenu.vampireclassbutton");
static LinkTexture sTextureBack("ui.gamemodemenu.backbutton");

ClassSelectMenu::ClassSelectMenu()
	: FullscreenMenu()
{
	gamemode = Gamemode::UNDEFINED;
//
//	float screenW = (float)Application.GetWidth();
//	float screenH = (float)Application.GetHeight();
//
//	float centerX = screenW / 2;
//	float centerY = screenH / 2;
//
//	float buttonWidth = screenW / 4;
//	float buttonHeight = screenH / 10;
//	float hSpacing = buttonWidth / 6;
//	float vSpacing = buttonHeight;
//
//	float totalWidth = buttonWidth * 2 + hSpacing;
//	float totalHeight = buttonHeight * 2 + vSpacing;
//	float originX = centerX - totalWidth / 2;
//	float originY = centerY - totalHeight / 2;
//
//	m_ClassButtonRect1 = { originX, originY, buttonWidth, buttonHeight };
//	m_ClassButtonRect2 = { originX + buttonWidth + hSpacing, originY, buttonWidth, buttonHeight };
//	m_ClassButtonRect3 = { originX, originY + buttonHeight + vSpacing, buttonWidth, buttonHeight };
//	m_ClassButtonRect4 = { originX + buttonWidth + hSpacing, originY + buttonHeight + vSpacing, buttonWidth, buttonHeight };
//
//	m_BackButtonRect = { centerX - buttonHeight, originY + 3 * (buttonHeight) + vSpacing, buttonHeight * 2, buttonHeight };
//
//	m_TitleRect = { centerX - (buttonWidth * 2 / 3), originY - 3 * buttonHeight, buttonWidth * 4 / 3, buttonHeight * 2 };

	// Lambda function for doing stuff when setting specified class type
	auto set_class = [](int player_class_type)
	{
		// Close menu
		GameReference.SetCurrentMenu(nullptr);

		Assets.PauseMusic();
		Assets.GetSound("ui.pitch.low")->PlaySound();

		GameReference.AddPendingClass(PlayerClass::CreateClass(player_class_type));
		if (GameReference.GetClassMenus().empty())
			GameReference.InitializeInfinite();
	};

	auto human = (Button *)(new Button())
		->SetSize(Vec2i(320, 72))
		->SetDraw(DRAW_TEXTURE)
		->SetTexture(sTextureClassButton1);
	auto zombie = (Button *)(new Button())
		->SetSize(Vec2i(320, 72))
		->SetDraw(DRAW_TEXTURE)
		->SetTexture(sTextureClassButton2);
	auto cyborg = (Button *)(new Button())
		->SetSize(Vec2i(320, 72))
		->SetDraw(DRAW_TEXTURE)
		->SetTexture(sTextureClassButton3);
	auto vampire = (Button *)(new Button())
		->SetSize(Vec2i(320, 72))
		->SetDraw(DRAW_TEXTURE)
		->SetTexture(sTextureClassButton4);
	human->SetCallback([&set_class]()
					   {
						   CommonUI::soundUiPitchLow.GetSound()->PlaySound();
						   set_class(PLAYERCLASS_HUMAN);
					   });
	zombie->SetCallback([&set_class]()
						{
							CommonUI::soundUiPitchLow.GetSound()->PlaySound();
							set_class(PLAYERCLASS_ZOMBIE);
						});
	cyborg->SetCallback([&set_class]()
						{
							CommonUI::soundUiPitchLow.GetSound()->PlaySound();
							set_class(PLAYERCLASS_CYBORG);
						});
	vampire->SetCallback([&set_class]()
						 {
							 CommonUI::soundUiPitchLow.GetSound()->PlaySound();
							 set_class(PLAYERCLASS_VAMPIRE);
						 });

	auto back = (Button *)(new Button(sTextureBack, sTextureBack))
		->SetSize(Vec2i(160, 80))
		->SetAlign(Align::CENTER, Align::DONT);
	back->SetCallback([]() { GameReference.GameSelectMenu()->SwitchToThisMenu(); });

	auto top_buttons = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::WIDTH, 10)
		->AddChildren({ human, zombie });
	auto bottom_buttons = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::WIDTH, 10)
		->AddChildren({ cyborg, vampire });

	auto classes_frame = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::HEIGHT, 10)
		->AddChildren({ top_buttons, bottom_buttons });
	auto frame = (new Element())
		->SetAdaptive(true, true)
		->SetAlign(Align::CENTER, Align::CENTER)
		->SetFlex(Flex::HEIGHT, 10)
		->AddChildren({ classes_frame, back });

	SetName("ClassMenu");
	AddChildren({ new StarBackground(), frame });
}

ClassSelectMenu::~ClassSelectMenu()
{
}

//void ClassSelectMenu::Show()
//{
//	SetAsCurrentMenu();
//
////	bool running = true;
////	bool menuOpen = true;
////
////	while (menuOpen)
////	{
////		Tick();
////		Render();
////		SDL_ShowCursor();
////		SDL_Event currentEvent;
////		while (SDL_PollEvent(&currentEvent))
////		{
////			Application.Event(currentEvent);
////			Event(currentEvent, running, menuOpen, mode);
////		}
////		Application.GetDrawing()->UpdateWindow();
////	}
////
////	Application.GetDrawing()->Clear();
//}

//void ClassSelectMenu::Event(const SDL_Event& event)
//{
//	switch (event.type)
//	{
//		case SDL_EVENT_MOUSE_BUTTON_DOWN:
//		{
//			SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
//
//			if (event.button.button == SDL_BUTTON_LEFT)
//			{
//				int x = event.button.x;
//				int y = event.button.y;
//
//				if (x >= m_ClassButtonRect1.x && x < m_ClassButtonRect1.x + m_ClassButtonRect1.w &&
//					y >= m_ClassButtonRect1.y && y < m_ClassButtonRect1.y + m_ClassButtonRect1.h)
//				{
//					// Close menu
//					GameReference.SetCurrentMenu(nullptr);
//
//					Assets.PauseMusic();
//					Assets.GetSound("ui.pitch.low")->PlaySound();
//
//					GameReference.AddPendingClass(PlayerClass::CreateClass(PLAYERCLASS_HUMAN));
//					if (GameReference.GetClassMenus().empty())
//						GameReference.InitializeInfinite();
//				}
//				else if (x >= m_ClassButtonRect2.x && x < m_ClassButtonRect2.x + m_ClassButtonRect2.w &&
//					y >= m_ClassButtonRect2.y && y < m_ClassButtonRect2.y + m_ClassButtonRect2.h)
//				{
//					// Close menu
//					GameReference.SetCurrentMenu(nullptr);
//
//					Assets.PauseMusic();
//					Assets.GetSound("ui.pitch.low")->PlaySound();
//					GameReference.AddPendingClass(PlayerClass::CreateClass(PLAYERCLASS_ZOMBIE));
//
//					if (GameReference.GetClassMenus().empty())
//						GameReference.InitializeInfinite();
//				}
//				else if (x >= m_ClassButtonRect3.x && x < m_ClassButtonRect3.x + m_ClassButtonRect3.w &&
//					y >= m_ClassButtonRect3.y && y < m_ClassButtonRect3.y + m_ClassButtonRect3.h)
//				{
//					// Close menu
//					GameReference.SetCurrentMenu(nullptr);
//
//					Assets.PauseMusic();
//					Assets.GetSound("ui.pitch.low")->PlaySound();
//					GameReference.AddPendingClass(PlayerClass::CreateClass(PLAYERCLASS_CYBORG));
//					if (GameReference.GetClassMenus().empty())
//						GameReference.InitializeInfinite();
//				}
//				else if (x >= m_ClassButtonRect4.x && x < m_ClassButtonRect4.x + m_ClassButtonRect4.w &&
//					y >= m_ClassButtonRect4.y && y < m_ClassButtonRect4.y + m_ClassButtonRect4.h)
//				{
//					// Close menu
//					GameReference.SetCurrentMenu(nullptr);
//
//					Assets.PauseMusic();
//					Assets.GetSound("ui.pitch.low")->PlaySound();
//					GameReference.AddPendingClass(PlayerClass::CreateClass(PLAYERCLASS_VAMPIRE));
//					if (GameReference.GetClassMenus().empty())
//						GameReference.InitializeInfinite();
//				}
//				else if (x >= m_BackButtonRect.x && x < m_BackButtonRect.x + m_BackButtonRect.w &&
//					y >= m_BackButtonRect.y && y < m_BackButtonRect.y + m_BackButtonRect.h)
//					GameReference.GameSelectMenu()->Show();
//			}
//
//			break;
//		}
//
//		case SDL_EVENT_MOUSE_MOTION:
//		{
//			int x = event.motion.x;
//			int y = event.motion.y;
//			bool hovering = false;
//
//			hovering |= (x >= m_ClassButtonRect1.x && x < m_ClassButtonRect1.x + m_ClassButtonRect1.w &&
//				y >= m_ClassButtonRect1.y && y < m_ClassButtonRect1.y + m_ClassButtonRect1.h);
//			hovering |= (x >= m_ClassButtonRect2.x && x < m_ClassButtonRect2.x + m_ClassButtonRect2.w &&
//				y >= m_ClassButtonRect2.y && y < m_ClassButtonRect2.y + m_ClassButtonRect2.h);
//			hovering |= (x >= m_ClassButtonRect3.x && x < m_ClassButtonRect3.x + m_ClassButtonRect3.w &&
//				y >= m_ClassButtonRect3.y && y < m_ClassButtonRect3.y + m_ClassButtonRect3.h);
//			hovering |= (x >= m_ClassButtonRect4.x && x < m_ClassButtonRect4.x + m_ClassButtonRect4.w &&
//				y >= m_ClassButtonRect4.y && y < m_ClassButtonRect4.y + m_ClassButtonRect4.h);
//			hovering |= (x >= m_BackButtonRect.x && x < m_BackButtonRect.x + m_BackButtonRect.w &&
//				y >= m_BackButtonRect.y && y < m_BackButtonRect.y + m_BackButtonRect.h);
//
//			SDL_SetCursor(SDL_CreateSystemCursor(hovering ? SDL_SYSTEM_CURSOR_POINTER : SDL_SYSTEM_CURSOR_DEFAULT));
//			break;
//		}
//		case SDL_EVENT_WINDOW_RESIZED:
//		{
//			float screenW = Application.GetWidth();
//			float screenH = Application.GetHeight();
//
//			float centerX = screenW / 2;
//			float centerY = screenH / 2;
//
//			float buttonWidth = screenW / 4;
//			float buttonHeight = screenH / 10;
//			float hSpacing = buttonWidth / 6;
//			float vSpacing = buttonHeight;
//
//			float totalWidth = buttonWidth * 2 + hSpacing;
//			float totalHeight = buttonHeight * 2 + vSpacing;
//			float originX = centerX - totalWidth / 2;
//			float originY = centerY - totalHeight / 2;
//
//			m_ClassButtonRect1 = { originX, originY, buttonWidth, buttonHeight };
//			m_ClassButtonRect2 = { originX + buttonWidth + hSpacing, originY, buttonWidth, buttonHeight };
//			m_ClassButtonRect3 = { originX, originY + buttonHeight + vSpacing, buttonWidth, buttonHeight };
//			m_ClassButtonRect4 = { originX + buttonWidth + hSpacing, originY + buttonHeight + vSpacing, buttonWidth,
//								   buttonHeight };
//
//			m_BackButtonRect = {
//				centerX - buttonHeight,
//				originY + 3 * (buttonHeight) + vSpacing,
//				buttonHeight * 2,
//				buttonHeight };
//
//			m_TitleRect = {
//				centerX - (buttonWidth * 2 / 3),
//				originY - 3 * buttonHeight,
//				buttonWidth * 4 / 3,
//				buttonHeight * 2 };
//
//			Application.GetDrawing()->Clear();
//			Application.GetDrawing()->UpdateWindow();
//			break;
//		}
//	}
//}

//void ClassSelectMenu::Tick()
//{
//	for (int i = 0; i < 3; i++)
//	{
//		auto random_position = Vec2d(rand() % Application.GetWidth(), rand() % Application.GetHeight());
//		auto duration = 1500.0;
//		m_Stars.emplace_back(random_position, Vec2d(0.0, 0.0), duration);
//	}
//
//	Vec2f MousePosition;
//	SDL_GetMouseState(&MousePosition.x, &MousePosition.y);
//	auto MousePositionD = Vec2d(MousePosition.x, MousePosition.y);
//
//	for (int i = m_Stars.size() - 1; i >= 0; --i)
//	{
//		auto& [position, velocity, duration] = m_Stars[i];
//
//		auto direction = position - MousePositionD;
//		double distance = direction.Length();
//		velocity += direction.Normalize() / distance * 0.15;
//		velocity.x += (sin((position.x + position.y * 2) / 50)) * 0.0015;
//
//		velocity *= 0.98;
//
//		position.x += velocity.x;
//		position.y += velocity.y;
//		if (position.x > Application.GetWidth())
//			position.x -= Application.GetWidth();
//		if (position.y > Application.GetHeight())
//			position.y -= Application.GetHeight();
//
//		duration -= 1;
//		if (duration <= 0.0)
//			m_Stars.erase(m_Stars.begin() + i);
//	}
//}

//void ClassSelectMenu::Render()
//{
//	auto drawing = Application.GetDrawing();
//
//	drawing->SetColor(0, 0, 50, 255);
//	drawing->Clear();
//
//	drawing->SetColor(200, 200, 200, 255);
//	for (int i = m_Stars.size() - 1; i >= 0; --i)
//	{
//		auto& [position, velocity, duration] = m_Stars[i];
//
//		auto size = (int)duration / 750.0;
//		for (int j = 0; j < size; j++)
//		{
//			for (int k = 0; k < size; k++)
//			{
//				int draw_x = (int)(position.x - size / 2 + j);
//				int draw_y = (int)(position.y - size / 2 + k);
//
//				SDL_RenderPoint(drawing->Renderer(), draw_x, draw_y);
//			}
//		}
//	}
//
//	drawing->RenderTexture(sTextureTitle.GetTexture()->SDLTexture(), nullptr, m_TitleRect);
//	drawing->RenderTexture(sTextureClassButton1.GetTexture()->SDLTexture(), nullptr, m_ClassButtonRect1);
//	drawing->RenderTexture(sTextureClassButton2.GetTexture()->SDLTexture(), nullptr, m_ClassButtonRect2);
//	drawing->RenderTexture(sTextureClassButton3.GetTexture()->SDLTexture(), nullptr, m_ClassButtonRect3);
//	drawing->RenderTexture(sTextureClassButton4.GetTexture()->SDLTexture(), nullptr, m_ClassButtonRect4);
//	drawing->RenderTexture(sTextureBack.GetTexture()->SDLTexture(), nullptr, m_BackButtonRect);
//}