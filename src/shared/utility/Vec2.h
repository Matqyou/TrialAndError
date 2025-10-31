//
// Created by Matq on 04/06/2023.
//

#pragma once

#include <cmath>

template<class T>
class Vec2
{
public:
	T x, y;

	Vec2();
	Vec2(T x, T y);
	Vec2(const Vec2& v);

	template<class U>
	explicit Vec2(const Vec2<U>& other);

	// Returned math operations
	[[nodiscard]] Vec2 operator+(const Vec2& v) const;
	[[nodiscard]] Vec2 operator-(const Vec2& v) const;
	[[nodiscard]] Vec2 operator*(const Vec2& v) const;
	[[nodiscard]] Vec2 operator/(const Vec2& v) const;

	[[nodiscard]] Vec2 operator+(T scalar) const;
	[[nodiscard]] Vec2 operator-(T scalar) const;
	[[nodiscard]] Vec2 operator*(T scalar) const;
	[[nodiscard]] Vec2 operator/(T scalar) const;

	[[nodiscard]] Vec2 operator-() const;

	// Manipulative operations
	Vec2& operator=(const Vec2& v);
	Vec2& operator+=(const Vec2& v);
	Vec2& operator-=(const Vec2& v);
	Vec2& operator*=(const Vec2& v);
	Vec2& operator/=(const Vec2& v);

	Vec2& operator+=(T scalar);
	Vec2& operator-=(T scalar);
	Vec2& operator*=(T scalar);
	Vec2& operator/=(T scalar);

	// Generating
	[[nodiscard]] bool operator==(const Vec2& v);
	[[nodiscard]] bool operator!=(const Vec2& v);
	[[nodiscard]] double Length() const;
	[[nodiscard]] float LengthF() const;
	[[nodiscard]] double Atan2() const;
	[[nodiscard]] float Atan2F() const;
	[[nodiscard]] Vec2 Rotate(double radians) const;
	[[nodiscard]] Vec2 RotateF(float radians) const;
	[[nodiscard]] Vec2 Normalize() const;
	[[nodiscard]] Vec2 NormalizeF() const;
	[[nodiscard]] Vec2 SetLength(double length) const;
	[[nodiscard]] Vec2 SetLengthF(float length) const;

};

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

[[nodiscard]] static float DistanceVec2i(const Vec2i& v1, const Vec2i& v2);
[[nodiscard]] static float DistanceVec2f(const Vec2f& v1, const Vec2f& v2);
[[nodiscard]] static double DistanceVec2d(const Vec2d& v1, const Vec2d& v2);

[[nodiscard]] static Vec2f FromAngleVec2f(float radians);
[[nodiscard]] static Vec2d FromAngleVec2d(double radians);

#include "Vec2.inl"
