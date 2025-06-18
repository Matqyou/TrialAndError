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
	[[nodiscard]] Vec2 operator+(T scalar) const;
	[[nodiscard]] Vec2 operator-(T scalar) const;
	[[nodiscard]] Vec2 operator*(T scalar) const;
	[[nodiscard]] Vec2 operator/(T scalar) const;

	// Manipulative operations
	Vec2& operator=(const Vec2& v);
	Vec2& operator+=(const Vec2& v);
	Vec2& operator-=(const Vec2& v);
	Vec2& operator+=(T scalar);
	Vec2& operator-=(T scalar);
	Vec2& operator*=(T scalar);
	Vec2& operator/=(T scalar);

	// Generating
	[[nodiscard]] bool operator==(const Vec2& v);
	[[nodiscard]] bool operator!=(const Vec2& v);
	[[nodiscard]] double Length() const;
	[[nodiscard]] double Atan2() const;
	[[nodiscard]] Vec2 Ortho() const;
	[[nodiscard]] static double DotProduct(const Vec2& v1, const Vec2& v2);
	[[nodiscard]] static double CrossProduct(const Vec2& v1, const Vec2& v2);

	// Manipulating
	void Rotate(double radians);
	Vec2& Normalize();
	Vec2& SetLength(double length);

};

template<class T>
[[nodiscard]] double DistanceVec2d(const Vec2<T>& v);
template<class T>
[[nodiscard]] float DistanceVec2f(const Vec2<T>& v);

template<class T>
[[nodiscard]] Vec2<T> ClampMax(const Vec2<T>& v, const Vec2<T>& max);

typedef Vec2<int> Vec2i;
typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;

[[nodiscard]] Vec2<double> AngleVec2d(double radians);
[[nodiscard]] Vec2<float> AngleVec2f(float radians);

#include "Vec2.inl"
