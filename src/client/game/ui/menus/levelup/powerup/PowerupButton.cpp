//
// Created by Matq on 22/06/2025.
//

#include "PowerupButton.h"
#include "client/utility/RenderPresets.h"

static LinkTexture sPowerupTextures[(size_t)Powerup::NUM_POWERUPS] = {
	LinkTexture("interface.permanenterrors.allstats"),
	LinkTexture("interface.permanenterrors.bombs"),
	LinkTexture("interface.permanenterrors.bossdamage"),
	LinkTexture("interface.permanenterrors.doubledamage"),
	LinkTexture("interface.permanenterrors.explosiveammo"),
	LinkTexture("interface.permanenterrors.speed"),
	LinkTexture("interface.permanenterrors.spiky"),
	LinkTexture("interface.permanenterrors.health"),
	LinkTexture("interface.permanenterrors.infiniteglockammo"),
};
static LinkTexture sTextureErrorOutline("interface.permanenterrors.regularoutlinefull");

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
