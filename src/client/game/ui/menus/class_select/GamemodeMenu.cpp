// src/game/interface/GameModeMenu.cpp

#include "GamemodeMenu.h"
#include "client/game/classes/PlayerClass.h"
#include "ClassSelectMenu.h"
#include "client/game/ui/elements/Button.h"
#include "client/game/ui/advanced_elements/StarBackground.h"
#include "client/game/ui/CommonUI.h"

static LinkTexture sMenuTexture("interface.menu");
static LinkTexture sTextureTitle("ui.gamemodemenu.title");
static LinkTexture sTextureSolo("ui.gamemodemenu.solobutton");
static LinkTexture sTexturePvp("ui.gamemodemenu.pvpbutton");
static LinkTexture sTextureSandbox("ui.gamemodemenu.sandboxbutton");
static LinkTexture sTextureCoop("ui.gamemodemenu.co-opbutton");
static LinkTexture sTextureBack("ui.gamemodemenu.backbutton");

GamemodeMenu::GamemodeMenu()
	: FullscreenMenu()
{
	auto set_gamemode = [](Gamemode gamemode)
	{
		GameReference.GetClassMenus().front()->SetGamemode(gamemode);
		GameReference.GetClassMenus().front()->SwitchToThisMenu();

		Assets.PauseMusic();
		Assets.GetSound("ui.pitch.low")->PlaySound();
	};

	auto solo = (Button *)(new Button(sTextureSolo, sTextureSolo))
		->SetSize(Vec2i(360, 80));
	auto pvp = (Button *)(new Button(sTexturePvp, sTexturePvp))
		->SetSize(Vec2i(360, 80));
	auto sandbox = (Button *)(new Button(sTextureSandbox, sTextureSandbox))
		->SetSize(Vec2i(360, 80));
	auto coop = (Button *)(new Button(sTextureCoop, sTextureCoop))
		->SetSize(Vec2i(360, 80));
	solo->SetCallback([&set_gamemode]()
					  {
						  CommonUI::soundUiPitchLow.GetSound()->PlaySound();
						  set_gamemode(Gamemode::Solo);
					  });
	pvp->SetCallback([&set_gamemode]()
					 {
						 CommonUI::soundUiPitchLow.GetSound()->PlaySound();
						 set_gamemode(Gamemode::PvP);
					 });
	sandbox->SetCallback([&set_gamemode]()
						 {
							 CommonUI::soundUiPitchLow.GetSound()->PlaySound();
							 set_gamemode(Gamemode::Sandbox);
						 });
	coop->SetCallback([&set_gamemode]()
					  {
						  CommonUI::soundUiPitchLow.GetSound()->PlaySound();
						  set_gamemode(Gamemode::Coop);
					  });

	auto back = (Button *)(new Button(sTextureBack, sTextureBack))
		->SetSize(Vec2i(160, 80))
		->SetAlign(Align::CENTER, Align::DONT);
	back->SetCallback([]() { GameReference.Menu()->SwitchToThisMenu(); });

	auto top_buttons = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::WIDTH, 30)
		->AddChildren({ solo, pvp });
	auto bottom_buttons = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::WIDTH, 30)
		->AddChildren({ sandbox, coop });

	auto classes_frame = (new Element())
		->SetAdaptive(true, true)
		->SetFlex(Flex::HEIGHT, 30)
		->AddChildren({ top_buttons, bottom_buttons });
	auto frame = (new Element())
		->SetAdaptive(true, true)
		->SetAlign(Align::CENTER, Align::CENTER)
		->SetFlex(Flex::HEIGHT, 20)
		->AddChildren({ classes_frame, back });

	SetName("GamemodeMenu");
	AddChildren({ new StarBackground(), frame });
}

GamemodeMenu::~GamemodeMenu()
{

}
