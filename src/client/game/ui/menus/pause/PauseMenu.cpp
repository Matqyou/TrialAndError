//
// Created by mimmy
//

#include "PauseMenu.h"
#include "client/game/ui/elements/Button.h"
#include "client/game/ui/menus/Menus.h"
#include "client/game/GameReference.h"
#include "client/game/ui/CommonUI.h"
#include "client/core/Application.h"

static LinkTexture sTextureResume("interface.resume");
static LinkTexture sTextureBack("interface.back");

PauseMenu::PauseMenu()
{
	auto resume_button = (Button *)(new Button(sTextureResume, sTextureResume))
		->SetSize(Vec2i(200, 70))
		->SetAlign(Align::CENTER, Align::DONT)
		->SetDraw(DRAW_TEXTURE)
		->SetName("Resume");

	auto back_button = (Button *)(new Button(sTextureBack, sTextureBack))
		->SetSize(Vec2i(200, 70))
		->SetAlign(Align::CENTER, Align::DONT)
		->SetDraw(DRAW_TEXTURE)
		->SetName("Back");

	resume_button->SetCallback([]()
	{
		CommonUI::soundUiPitchLow.GetSound()->PlaySound();
		GameReference.World()->SetPaused(false);
		Menus.SetCurrentMenu(nullptr);
	});

	back_button->SetCallback([]()
	{
		CommonUI::soundUiPitchMid.GetSound()->PlaySound();
		GameReference.DeleteWorld();
		Menus.main_menu->SwitchToThisMenu();
	});

	auto frame = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::HEIGHT, 20)
		->SetAlign(Align::CENTER, Align::CENTER)
		->AddChildren({ resume_button, back_button });

	SetDraw(DONT_DRAW);
	SetName("PauseMenu");
	AddChildren({ frame });
}

PauseMenu::~PauseMenu() = default;

void PauseMenu::SwitchToThisMenu()
{
	GameReference.World()->SetPaused(true);
	Menus.SetCurrentMenu(this);
	RefreshMenu();
}

void PauseMenu::HandleEvent(const SDL_Event &sdl_event, EventContext &event_summary)
{
    if (sdl_event.type == SDL_EVENT_KEY_DOWN && sdl_event.key.scancode == SDL_SCANCODE_ESCAPE && !event_summary.rapid_context.event_captured)
    {
        event_summary.rapid_context.event_captured = true;
        CommonUI::soundUiPitchLow.GetSound()->PlaySound();
        GameReference.World()->SetPaused(false);
        Menus.SetCurrentMenu(nullptr);
        return;
    }
	HandleEventChildren(sdl_event, event_summary);
    FullscreenMenuEvent(sdl_event, event_summary);
}
void PauseMenu::Tick(double elapsed_seconds)
{
	TickChildren(elapsed_seconds);
}

void PauseMenu::Render()
{
	RenderChildren();
}
