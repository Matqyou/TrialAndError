// src/game/interface/GameModeMenu.cpp

#include "ClassSelectMenu.h"
#include "client/game/ui/advanced_elements/StarBackground.h"
#include "client/game/players/classes/PlayerClass.h"
#include "client/game/ui/elements/Button.h"
#include "client/game/ui/menus/Menus.h"
#include "client/game/players/Player.h"
#include "client/game/ui/CommonUI.h"
#include "GamemodeMenu.h"

// TODO Update the class buttons to be icons of the class and have text descriptions on hover
static LoadTexture sTextureClassButton1("ui.classselectmenu.humanclassbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureClassButton2("ui.classselectmenu.zombieclassbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureClassButton3("ui.classselectmenu.cyborgclassbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureClassButton4("ui.classselectmenu.vampireclassbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);
static LoadTexture sTextureBack("ui.gamemodemenu.backbutton", AssetsClass::TexturePurpose::GUI_ELEMENT);

ClassSelectMenu::ClassSelectMenu()
	: FullscreenMenu()
{
	gamemode = Gamemode::UNDEFINED;

	// Lambda function for doing stuff when setting specified class type
	auto set_class = [](int player_class_type)
	{
		// Close menu
		Menus.SetCurrentMenu(nullptr);

//		Assets.PauseMusic();  // todo: mix
		Assets.GetSound("ui.pitch.low")->PlaySound();

		auto first_player_preferences = GameReference.GetPlayerPreferences(0);
		first_player_preferences.SetPlayerClass(player_class_type);
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
	back->SetCallback([]() { Menus.gamemode_menu->SwitchToThisMenu(); });

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
