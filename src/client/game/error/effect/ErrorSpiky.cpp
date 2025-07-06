//
// Created by Matq on 13/12/2024.
//

#include "ErrorSpiky.h"

LinkTexture sTextureSpiky("icons.spiky");

ErrorSpiky::ErrorSpiky(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::SPIKY,
						"Spiky",
						sTextureSpiky.GetTexture(),
						3000)
{

}
