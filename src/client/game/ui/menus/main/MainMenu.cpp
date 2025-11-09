// src/game/interface/MainMenu.cpp

#include "MainMenu.h"
#include "client/game/ui/advanced_elements/StarBackground.h"
#include "client/game/ui/elements/Button.h"
#include "client/core/Application.h"
#include "client/game/GameReference.h"
#include "client/game/ui/CommonUI.h"
#include "client/game/ui/menus/Menus.h"

static LinkSound sElevatorMusic("intro");
static LoadTexture sTexturePlay("ui.main.playbutton2", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureTitle("ui.main.title2", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureExit("ui.main.exit2", AssetsClass::TexturePurpose::GUI_ELEMENT);

const Uint32 NUM_MAIN_MENU_QUADS = 1;
MainMenu::MainMenu()
	: FullscreenMenu(),
	  render(Quad::NUM_VERTICES * NUM_MAIN_MENU_QUADS, Quad::NUM_INDICES * NUM_MAIN_MENU_QUADS),
	  render_circles(Quad::NUM_VERTICES, Quad::NUM_INDICES)
{
	Dim2Rect background_rect(
		Rect4f(0, 0, 0, 0),
		Rect4f(0, 0, 1, 1)
	);
	background.Bind(render, background_rect, { 0, 0, 0, 255 });

	Dim2Rect circle_rect(
		Rect4f(0, 0, 100, 100),
		Rect4f(0, 0, 0, 0)
	);
	intro_circle.Bind(render_circles, circle_rect);

	Application.GetPreRenderEvent().Subscribe(
		[this]()
		{
			render.UpdateGPU();
			render.SetTexture(nullptr);

			render_circles.UpdateGPU();
			render_circles.SetDrawAsCircles(true);
		});

	auto title = (new Element())
		->SetSize(Vec2i(600, 300))
		->SetAlign(Align::CENTER, Align::DONT)
		->SetDraw(DRAW_TEXTURE)
		->SetTexture(sTextureTitle)
		->SetName("Title");

	auto play_button = (Button *)(new Button(sTexturePlay, sTexturePlay))
		->SetSize(Vec2i(360, 80))
		->SetAlign(Align::CENTER, Align::DONT)
		->SetDraw(DRAW_TEXTURE)
		->SetName("Play");
	auto exit_button = (Button *)(new Button(sTextureExit, sTextureExit))
		->SetSize(Vec2i(360, 80))
		->SetAlign(Align::CENTER, Align::DONT)
		->SetDraw(DRAW_TEXTURE)
		->SetName("Exit");

	play_button->SetCallback(
		[]()
		{
//			Assets.PauseMusic();
			CommonUI::soundUiPitchLow.GetSound()->PlaySound();
			Menus.gamemode_menu->SwitchToThisMenu();
		});
	exit_button->SetCallback(
		[]()
		{
			CommonUI::soundUiPitchLow.GetSound()->PlaySound();
			GameReference.ExitApplication();
		});

	auto frame = (new Element())
		->SetAdaptive(true, true)
		->SetAlign(Align::CENTER, Align::CENTER)
		->SetFlex(Flex::HEIGHT, 10)
		->AddChildren({ title, play_button, exit_button });

	opened_at = std::chrono::steady_clock::now();
	intro = true;

	SetDraw(DONT_DRAW);
	SetName("MainMenu");
	AddChildren({ new StarBackground(), frame });
}

MainMenu::~MainMenu()
{

}

void MainMenu::SwitchToThisMenu()
{
	// todo: mix
//	if (!Mix_PausedMusic() && !Mix_PlayingMusic())
//	{
//		sElevatorMusic.GetMusic()->PlayMusic(-1);
//		if (!m_Intro) Mix_SetMusicPosition(16);
//	}
//	else
//	{ Mix_ResumeMusic(); }

	Menus.SetCurrentMenu(this);
	RefreshMenu();
}

void MainMenu::HandleEvent(const SDL_Event& sdl_event, EventContext& event_summary)
{
	if (!intro)
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
		intro = false;
//		MIX_SetTrackPlaybackPosition(Application.GetTrack(), 16); // this prob doesnt work cause tracks can play 1 sound at once?
//		Mix_SetMusicPosition(16);  // todo: mix
		RefreshMenu();
	}

}

void MainMenu::Tick(double elapsed_seconds)
{
	if (intro)
	{
		if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - opened_at).count() >= 15500)
			intro = false;

		return;
	}

	TickChildren(elapsed_seconds);
}

void MainMenu::PreRender()
{
	if (!intro)
	{
		PreRenderChildren();
		return;
	}

	double duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - opened_at).count();
	int centerX = (int)Application.GetWidth2();
	int centerY = (int)Application.GetHeight2();
	int radius = (int)(duration * 0.05);
	Uint8 circle_color = (Uint8)(duration / 15500 * 255);

	Dim2Rect circle_rect = Dim2Rect(
		Rect4f(centerX - radius, centerY - radius, radius * 2, radius * 2),
		Rect4f(0, 0, 0, 0)
	);
	intro_circle.UpdateRect(circle_rect);
	intro_circle.UpdateColor({ circle_color, circle_color, circle_color, 255 });

	render_circles.UpdateGPU(); // todo: batch gpu updates instead of doing this *sips tea* :3
}

void MainMenu::Render()
{

	if (!intro)
	{
		RenderChildren();
		return;
	}

	render.Draw();

	double duration = (double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - opened_at).count();
	if (duration < 15000) // shocking disappearance of the circle before cutscene ends
		render_circles.Draw();
}