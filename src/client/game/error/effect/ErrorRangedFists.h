//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "../base/ErrorStatusEffect.h"

class ErrorRangedFists : public ErrorStatusEffect
{
private:
	static LoadedTexture sTextureRangedFists;

public:
	explicit ErrorRangedFists(ErrorStatuses *parent);

};
