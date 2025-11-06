//
// Created by Matq on 13/12/2024.
//

#include "ErrorCantHear.h"

LoadTexture sTextureCantHear("icons.cant_hear", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorCantHear::ErrorCantHear(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::I_CANT_HEAR,
						"I Can't Hear",
						sTextureCantHear.GetTexture(),
						3000)
{

}