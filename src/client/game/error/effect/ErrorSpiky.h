//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "../base/ErrorStatusEffect.h"

class ErrorSpiky : public ErrorStatusEffect
{
private:
	static LoadedTexture sTextureSpiky;

public:
	explicit ErrorSpiky(ErrorStatuses *parent);

};
