//
// Created by Matq on 02/11/2025.
//

#pragma once

#include "shared/math/Vec3.h"
#include <cmath>

namespace Geometry
{
inline Vec3f TriangleNormal(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2)
{
	Vec3f edge1 = v1 - v0;
	Vec3f edge2 = v2 - v0;
	return CrossProductVec3(edge1, edge2).NormalizeF();
}
}