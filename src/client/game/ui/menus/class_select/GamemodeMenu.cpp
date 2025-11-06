// src/game/interface/GameModeMenu.cpp

#include "GamemodeMenu.h"
#include "client/game/ui/advanced_elements/StarBackground.h"
#include "client/game/players/classes/PlayerClass.h"
#include "client/game/ui/elements/Button.h"
#include "client/game/ui/CommonUI.h"
#include "client/game/ui/menus/Menus.h"
#include "ClassSelectMenu.h"

static LoadTexture sTextureSandbox("ui.gamemodemenu.sandboxbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureCoop("ui.gamemodemenu.co-opbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureSolo("ui.gamemodemenu.solobutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureBack("ui.gamemodemenu.backbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTexturePvp("ui.gamemodemenu.pvpbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureTitle("ui.gamemodemenu.title", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sMenuTexture("interface.menu", AssetsClass::TexturePurpose::GUI_ELEMENT);

GamemodeMenu::GamemodeMenu()
	: FullscreenMenu()
{
	auto set_gamemode = [](Gamemode gamemode)
	{
//		GameReference.GetClassMenus().front()->SetGamemode(gamemode);
//		GameReference.GetClassMenus().front()->SwitchToThisMenu();
		Menus.classselect_menu->SetGamemode(gamemode);
		Menus.classselect_menu->SwitchToThisMenu();

//		Assets.PauseMusic();
		CommonUI::soundUiPitchLow.GetSound()->PlaySound();
	};

	auto solo = (Button *)(new Button(sTextureSolo, sTextureSolo))->SetSize(Vec2i(360, 80));
	auto pvp = (Button *)(new Button(sTexturePvp, sTexturePvp))->SetSize(Vec2i(360, 80));
	auto sandbox = (Button *)(new Button(sTextureSandbox, sTextureSandbox))->SetSize(Vec2i(360, 80));
	auto coop = (Button *)(new Button(sTextureCoop, sTextureCoop))->SetSize(Vec2i(360, 80));
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
	back->SetCallback([]() { Menus.main_menu->SwitchToThisMenu(); });

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
