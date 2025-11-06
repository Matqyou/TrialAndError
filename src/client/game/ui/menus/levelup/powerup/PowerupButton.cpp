//
// Created by Matq on 22/06/2025.
//

#include "PowerupButton.h"
#include "client/utility/RenderPresets.h"

static LoadTexture sPowerupTextures[(size_t)Powerup::NUM_POWERUPS] = {
	LoadTexture("interface.permanenterrors.allstats", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("interface.permanenterrors.bombs", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("interface.permanenterrors.bossdamage", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("interface.permanenterrors.doubledamage", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("interface.permanenterrors.explosiveammo", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("interface.permanenterrors.speed", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("interface.permanenterrors.spiky", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("interface.permanenterrors.health", AssetsClass::TexturePurpose::GUI_ELEMENT),
	LoadTexture("interface.permanenterrors.infiniteglockammo", AssetsClass::TexturePurpose::GUI_ELEMENT),
};
static LoadTexture sTextureErrorOutline("interface.permanenterrors.regularoutlinefull", AssetsClass::TexturePurpose::GUI_ELEMENT);

PowerupButton::PowerupButton(Powerup powerup_type)
	: Button()
{
	type = powerup_type != Powerup::NUM_POWERUPS ? powerup_type : Powerup::ALL_STATS; // Default powerup

	auto powerup_texture = sPowerupTextures[(size_t)type];
	temp_texture = RenderPresets::Composition({ powerup_texture, sTextureErrorOutline }, { });

	SetSize(Vec2i(256, 256));
	SetDraw(DRAW_TEXTURE);
	SetTexture(temp_texture);
	SetName("Powerup");
}

PowerupButton::~PowerupButton()
{
	delete temp_texture;
}
