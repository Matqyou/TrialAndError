//
// Created by Matq on 13/12/2024.
//

#include "ErrorInvincible.h"

LoadTexture sTextureInvincible("icons.invincible", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorInvincible::ErrorInvincible(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::INVINCIBLE,
						"Invincible",
						sTextureInvincible.GetTexture(),
						1500)
{

}

