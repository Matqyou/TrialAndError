//
// Created by Matq on 13/12/2024.
//

#include "ErrorTeleport.h"

LinkTexture sTextureTeleport("icons.teleport");

ErrorTeleport::ErrorTeleport(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::TELEPORT,
						"Teleportation",
						sTextureTeleport.GetTexture(),
						3000)
{

}
