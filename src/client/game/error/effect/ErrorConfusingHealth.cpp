//
// Created by Matq on 13/12/2024.
//

#include "ErrorConfusingHealth.h"

LoadTexture sTextureConfusingHealth("icons.confusing_health", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorConfusingHealth::ErrorConfusingHealth(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::CONFUSING_HP,
						"Confusing Health",
						sTextureConfusingHealth.GetTexture(),
						1500)
{

}
