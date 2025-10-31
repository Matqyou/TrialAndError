//
// Created by Matq on 13/12/2024.
//

#include "ErrorDisoriented.h"

LinkTexture sTextureDisoriented("icons.disoriented");

ErrorDisoriented::ErrorDisoriented(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::DISORIENTED,
						"Disoriented",
						sTextureDisoriented.GetTexture(),
						1500)
{

}
