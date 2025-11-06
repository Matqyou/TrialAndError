//
// Created by Matq on 13/12/2024.
//

#include "ErrorTeleport.h"

LoadTexture sTextureTeleport("icons.teleport", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorTeleport::ErrorTeleport(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::TELEPORT,
						"Teleportation",
						sTextureTeleport.GetTexture(),
						3000)
{

}
