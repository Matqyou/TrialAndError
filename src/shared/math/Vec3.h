//
// Created by Matq on 01/11/2025.
//

#pragma once

#include <cmath>

template<class T>
class Vec3
{
public:
	T x, y, z;

	Vec3();
	Vec3(T X, T Y, T Z);
	Vec3(const Vec3& v);

	template<class U>
	explicit Vec3(const Vec3<U>& other);

	// Returned math operations
	[[nodiscard]] Vec3 operator+(const Vec3& v) const;
	[[nodiscard]] Vec3 operator-(const Vec3& v) const;
	[[nodiscard]] Vec3 operator*(const Vec3& v) const;
	[[nodiscard]] Vec3 operator/(const Vec3& v) const;

	[[nodiscard]] Vec3 operator+(T scalar) const;
	[[nodiscard]] Vec3 operator-(T scalar) const;
	[[nodiscard]] Vec3 operator*(T scalar) const;
	[[nodiscard]] Vec3 operator/(T scalar) const;

	[[nodiscard]] Vec3 operator-() const;

	// Manipulative operations
	Vec3& operator=(const Vec3& v);
	Vec3& operator+=(const Vec3& v);
	Vec3& operator-=(const Vec3& v);
	Vec3& operator*=(const Vec3& v);
	Vec3& operator/=(const Vec3& v);

	Vec3& operator+=(T scalar);
	Vec3& operator-=(T scalar);
	Vec3& operator*=(T scalar);
	Vec3& operator/=(T scalar);

	// Generating
	[[nodiscard]] bool operator==(const Vec3& v);
	[[nodiscard]] bool operator!=(const Vec3& v);
	[[nodiscard]] double Length() const;
	[[nodiscard]] float LengthF() const;
	[[nodiscard]] double LengthSquared() const;
	[[nodiscard]] float LengthSquaredF() const;
//	[[nodiscard]] double Atan2() const;
//	[[nodiscard]] float Atan2F() const;
//	[[nodiscard]] Vec3 Rotate(double radians) const;
//	[[nodiscard]] Vec3 RotateF(float radians) const;
	[[nodiscard]] Vec3 Normalize() const;
	[[nodiscard]] Vec3 NormalizeF() const;
	[[nodiscard]] Vec3 SetLength(double length) const;
	[[nodiscard]] Vec3 SetLengthF(float length) const;

};

typedef Vec3<int> Vec3i;
typedef Vec3<float> Vec3f;
typedef Vec3<double> Vec3d;

template<class T>
[[nodiscard]] Vec3<T> CrossProductVec3(const Vec3<T>& v1, const Vec3<T>& v2);

template<class T>
[[nodiscard]] T DotProductVec3(const Vec3<T>& v1, const Vec3<T>& v2);

//[[nodiscard]] static float DistanceVec3i(const Vec3i& v1, const Vec3i& v2);
//[[nodiscard]] static float DistanceVec3f(const Vec3f& v1, const Vec3f& v2);
//[[nodiscard]] static double DistanceVec3d(const Vec3d& v1, const Vec3d& v2);
//
//[[nodiscard]] static Vec3f FromAngleVec3f(float radians);
//[[nodiscard]] static Vec3d FromAngleVec3d(double radians);

#include "Vec3.inl"
