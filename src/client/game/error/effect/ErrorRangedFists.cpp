//
// Created by Matq on 13/12/2024.
//

#include "ErrorRangedFists.h"

LoadTexture sTextureRangedFists("icons.ranged_fists", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorRangedFists::ErrorRangedFists(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::RANGED,
						"Ranged Fists",
						sTextureRangedFists.GetTexture(),
						3000)
{

}
