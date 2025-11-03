//
// Created by Matq on 02/11/2025.
//

#pragma once

#include "Vec2.h"

namespace Mapping
{
Vec2f SquareToCircle(const Vec2f& square)
{
	float x2 = square.x * square.x;
	float y2 = square.y * square.y;

	float new_x = square.x * sqrtf(1.0f - y2 / 2.0f);
	float new_y = square.y * sqrtf(1.0f - x2 / 2.0f);

	return { new_x, new_y };
}
}
