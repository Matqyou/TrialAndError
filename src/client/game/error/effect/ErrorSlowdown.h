//
// Created by Matq on 13/12/2024.
//

#pragma once

#include <client/game/error/base/ErrorStatusEffect.h>

class ErrorSlowdown : public ErrorStatusEffect
{
private:

public:
	explicit ErrorSlowdown(ErrorStatuses *parent);

};
