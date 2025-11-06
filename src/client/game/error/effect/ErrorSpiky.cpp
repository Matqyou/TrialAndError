//
// Created by Matq on 13/12/2024.
//

#include "ErrorSpiky.h"

LoadTexture sTextureSpiky("icons.spiky", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorSpiky::ErrorSpiky(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::SPIKY,
						"Spiky",
						sTextureSpiky.GetTexture(),
						3000)
{

}
