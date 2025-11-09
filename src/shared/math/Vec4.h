//
// Created by Matq on 03/11/2025.
//

#pragma once

#include <cmath>

template<class T>
class Vec4
{
public:
	T x, y, z, w;

	Vec4();
	Vec4(T X, T Y, T Z, T W);
	Vec4(const Vec4& v);

	template<class U>
	explicit Vec4(const Vec4<U>& other);

	// Returned math operations
	[[nodiscard]] Vec4 operator+(const Vec4& v) const;
	[[nodiscard]] Vec4 operator-(const Vec4& v) const;
	[[nodiscard]] Vec4 operator*(const Vec4& v) const;
	[[nodiscard]] Vec4 operator/(const Vec4& v) const;

	[[nodiscard]] Vec4 operator+(T scalar) const;
	[[nodiscard]] Vec4 operator-(T scalar) const;
	[[nodiscard]] Vec4 operator*(T scalar) const;
	[[nodiscard]] Vec4 operator/(T scalar) const;

	[[nodiscard]] Vec4 operator-() const;

	// Manipulative operations
	Vec4& operator=(const Vec4& v);
	Vec4& operator+=(const Vec4& v);
	Vec4& operator-=(const Vec4& v);
	Vec4& operator*=(const Vec4& v);
	Vec4& operator/=(const Vec4& v);

	Vec4& operator+=(T scalar);
	Vec4& operator-=(T scalar);
	Vec4& operator*=(T scalar);
	Vec4& operator/=(T scalar);

	// Generating
	[[nodiscard]] bool operator==(const Vec4& v);
	[[nodiscard]] bool operator!=(const Vec4& v);
	[[nodiscard]] double Length() const;
	[[nodiscard]] float LengthF() const;
	[[nodiscard]] double LengthSquared() const;
	[[nodiscard]] float LengthSquaredF() const;
	[[nodiscard]] Vec4 Normalize() const;
	[[nodiscard]] Vec4 NormalizeF() const;
	[[nodiscard]] Vec4 SetLength(double length) const;
	[[nodiscard]] Vec4 SetLengthF(float length) const;

};

typedef Vec4<int> Vec4i;
typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;


#include "Vec4.inl"
