//
// Created by Matq on 13/12/2024.
//

#include "ErrorHealersParadise.h"

LoadTexture sTextureHealersParadise("icons.healers_paradise", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorHealersParadise::ErrorHealersParadise(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::HEALERS_PARADISE,
						"Healers Paradise",
						sTextureHealersParadise.GetTexture(),
						1500)
{

}
