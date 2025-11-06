//
// Created by Matq on 13/12/2024.
//

#include "ErrorDisoriented.h"

LoadTexture sTextureDisoriented("icons.disoriented", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorDisoriented::ErrorDisoriented(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::DISORIENTED,
						"Disoriented",
						sTextureDisoriented.GetTexture(),
						1500)
{

}
