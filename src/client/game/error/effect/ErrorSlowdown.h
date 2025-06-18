//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "../base/ErrorStatusEffect.h"

class ErrorSlowdown : public ErrorStatusEffect
{
private:
	static LoadedTexture sTextureSlowdown;

public:
	explicit ErrorSlowdown(ErrorStatuses *parent);

};
