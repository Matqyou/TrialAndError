//
// Created by Matq on 07/11/2025.
//

#pragma once

#include <shared/math/Vec2.h>

struct Dim2
{
	Vec2f relative;
	Vec2f scaled;

	Dim2()
	{
		relative = Vec2f(0, 0);
		scaled = Vec2f(0, 0);
	};
	Dim2(const Vec2f& start_relative, const Vec2f& start_scaled)
	{
		relative = start_relative;
		scaled = start_scaled;
	}
};