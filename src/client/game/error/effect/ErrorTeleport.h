//
// Created by Matq on 13/12/2024.
//

#pragma once

#include <client/game/error/base/ErrorStatusEffect.h>

class ErrorTeleport : public ErrorStatusEffect
{
private:
	static LoadedTexture sTextureTeleport;

public:
	explicit ErrorTeleport(ErrorStatuses *parent);

};
