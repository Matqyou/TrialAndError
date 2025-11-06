//
// Created by Matq on 13/12/2024.
//

#include "ErrorBulletFrenzy.h"

LoadTexture sTextureBulletFrenzy("icons.bullet_frenzy", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorBulletFrenzy::ErrorBulletFrenzy(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::BULLET_FRENZY,
						"Bullet frenzy",
						sTextureBulletFrenzy.GetTexture(),
						3000)
{

}
