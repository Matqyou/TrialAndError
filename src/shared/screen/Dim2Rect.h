//
// Created by Matq on 07/11/2025.
//

#pragma once

#include <shared/geometry/Rectangles.h>
#include <shared/screen/Dim2.h>

struct Dim2Rect
{
	Rect4f relative;
	Rect4f scaled;

	Dim2Rect()
	{
		relative = Rect4f();
		scaled = Rect4f();
	}
	Dim2Rect(const Rect4f& init_relative, const Rect4f& init_scaled)
	{
		relative = init_relative;
		scaled = init_scaled;
	}
};