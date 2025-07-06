//
// Created by Matq on 13/12/2024.
//

#include "ErrorConfusingHealth.h"

LinkTexture sTextureConfusingHealth("icons.confusing_health");

ErrorConfusingHealth::ErrorConfusingHealth(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::CONFUSING_HP,
						"Confusing Health",
						sTextureConfusingHealth.GetTexture(),
						1500)
{

}
