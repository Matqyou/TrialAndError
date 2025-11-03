// src/game/interface/MainMenu.cpp

#include "MainMenu.h"
#include "client/game/ui/advanced_elements/StarBackground.h"
#include "client/game/ui/elements/Button.h"
#include "client/core/Application.h"
#include "client/game/GameReference.h"
#include "client/game/ui/CommonUI.h"
#include "client/game/ui/menus/Menus.h"

static LinkMusic sElevatorMusic("intro");
static LinkTexture sMenuTexture("interface.menu");
static LinkTexture sTextureTitle("ui.main.title3");
static LinkTexture sTexturePlay("ui.main.playbutton2");
static LinkTexture sTextureTraining("ui.main.trainingbutton");
static LinkTexture sTextureExit("ui.main.exit2");

MainMenu::MainMenu()
	: FullscreenMenu()
{

	auto title = (new Element())
		->SetSize(Vec2i(700, 250))
		->SetAlign(Align::CENTER, Align::TOP)
		->SetDraw(DRAW_TEXTURE)
		->SetTexture(sTextureTitle)
		->SetName("Title");

	auto play_button = (Button *)(new Button(sTexturePlay, sTexturePlay))
		->SetSize(Vec2i(360, 80))
		->SetDraw(DRAW_TEXTURE)
		->SetName("Play");

	auto training_button = (Button *)(new Button(sTextureTraining, sTextureTraining))
		->SetSize(Vec2i(360, 80))
		->SetDraw(DRAW_TEXTURE)
		->SetName("Training");

	auto exit_button = (Button *)(new Button(sTextureExit, sTextureExit))
		->SetSize(Vec2i(360, 80))
		->SetAlign(Align::CENTER, Align::DONT)
		->SetDraw(DRAW_TEXTURE)
		->SetName("Exit");

	play_button->SetCallback([]()
							 {
//								 Assets.PauseMusic();
								 CommonUI::soundUiPitchLow.GetSound()->PlaySound();
								 Menus.gamemode_menu->SwitchToThisMenu();
							 });
	training_button->SetCallback([]()
							 {
								// TODO: Once merged update this to start a planetaryTrial
							 });
	exit_button->SetCallback([]()
							 {
								 CommonUI::soundUiPitchLow.GetSound()->PlaySound();
								 GameReference.ExitApplication();
							 });

	auto top_buttons = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::WIDTH, 40)
		->AddChildren({ play_button, training_button });
	auto frame = (new Element())
		->SetAdaptive(true, true)
		->SetAlign(Align::CENTER, Align::TOP)
		->SetFlex(Flex::HEIGHT, 40)
		->AddChildren({ title, top_buttons, exit_button });

	m_Opened = std::chrono::steady_clock::now();
	m_Intro = true;

	SetDraw(DONT_DRAW);
	SetName("MainMenu");
	AddChildren({ new StarBackground(), frame });
}

MainMenu::~MainMenu()
{

}

void MainMenu::SwitchToThisMenu()
{
	if (!Mix_PausedMusic() && !Mix_PlayingMusic())
	{
		sElevatorMusic.GetMusic()->PlayMusic(-1);
		if (!m_Intro) Mix_SetMusicPosition(16);
	} else { Mix_ResumeMusic(); }

	Menus.SetCurrentMenu(this);
	RefreshMenu();
}

void MainMenu::HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary)
{
	if (!m_Intro)
	{
		HandleEventChildren(sdl_event, event_summary);
		FullscreenMenuEvent(sdl_event, event_summary);
		return;
	}
	else
	{
		FullscreenMenuEvent(sdl_event, event_summary);
	}

	if (sdl_event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && sdl_event.button.button == SDL_BUTTON_LEFT)
	{
		m_Intro = false;
		Mix_SetMusicPosition(16);
		RefreshMenu();
	}

}

void MainMenu::Tick(double elapsed_seconds)
{
	if (m_Intro)
	{
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_Opened).count() >= 15500)
			m_Intro = false;

		return;
	}

	TickChildren(elapsed_seconds);
}

void MainMenu::Render()
{
	if (!m_Intro)
	{
		RenderChildren();
		return;
	}

	auto drawing = Application.GetDrawing();
	drawing->SetColor(0, 0, 0, 255);
	drawing->Clear();

	double duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_Opened).count();
	int centerX = (int)Application.GetWidth2();
	int centerY = (int)Application.GetHeight2();
	int radius = (int)(duration * 0.05);
	int color = (int)(duration / 15500 * 255);

	drawing->SetColor(color, color, color, 255);
	for (int y = -radius; y <= radius; ++y)
	{
		int dx = static_cast<int>(std::sqrt(radius * radius - y * y)); // Horizontal distance for this vertical offset
		int startX = centerX - dx;
		int endX = centerX + dx;

		// Draw a horizontal line for the current row
		drawing->DrawLine(Vec2f(startX, centerY + y), Vec2f(endX, centerY + y));
	}

	if (duration >= 15000)
	{
		drawing->SetColor(0, 0, 0, 255);
		drawing->Clear();
	}

}