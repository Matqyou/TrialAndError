//
// Created by Matq on 13/12/2024.
//

#pragma once

#include <client/game/error/base/ErrorStatusEffect.h>

class ErrorInvincible : public ErrorStatusEffect
{
private:

public:
	explicit ErrorInvincible(ErrorStatuses *parent);

};
