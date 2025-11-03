//
// Created by Matq on 01/11/2025.
//

#include <cmath>
//#include "Vec3.h"

template<class T>
Vec3<T>::Vec3() : x(T()), y(T()), z(T()) { }

template<class T>
Vec3<T>::Vec3(T x, T y, T z) : x(x), y(y), z(z) { }

template<class T>
Vec3<T>::Vec3(const Vec3& v) : x(v.x), y(v.y), z(v.z) { }

template<class T>
template<class U>
Vec3<T>::Vec3(const Vec3<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)) { }

template<class T>
Vec3<T>& Vec3<T>::operator=(const Vec3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

template<class T>
Vec3<T> Vec3<T>::operator+(const Vec3& v) const
{
	return Vec3(x + v.x, y + v.y, z + v.z);
}

template<class T>
Vec3<T> Vec3<T>::operator-(const Vec3& v) const
{
	return Vec3(x - v.x, y - v.y, z - v.z);
}

template<class T>
Vec3<T> Vec3<T>::operator*(const Vec3& v) const
{
	return Vec3(x * v.x, y * v.y, z * v.z);
}

template<class T>
Vec3<T> Vec3<T>::operator/(const Vec3& v) const
{
	return Vec3(x / v.x, y / v.y, z / v.z);
}

template<class T>
Vec3<T>& Vec3<T>::operator+=(const Vec3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

template<class T>
Vec3<T>& Vec3<T>::operator-=(const Vec3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

template<class T>
Vec3<T>& Vec3<T>::operator*=(const Vec3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

template<class T>
Vec3<T>& Vec3<T>::operator/=(const Vec3& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

template<class T>
Vec3<T> Vec3<T>::operator+(T scalar) const
{
	return Vec3(x + scalar, y + scalar, z + scalar);
}

template<class T>
Vec3<T> Vec3<T>::operator-(T scalar) const
{
	return Vec3(x - scalar, y - scalar, z - scalar);
}

template<class T>
Vec3<T> Vec3<T>::operator*(T scalar) const
{
	return Vec3(x * scalar, y * scalar, z * scalar);
}

template<class T>
Vec3<T> Vec3<T>::operator/(T scalar) const
{
	// Check for division by zero
	if (scalar != T())
	{
		return Vec3(x / scalar, y / scalar, z / scalar);
	}
	else
	{
		// Handle division by zero error
		// In this example, we simply return the original vector
		return *this;
	}
}

template<class T>
Vec3<T> Vec3<T>::operator-() const
{
	return { -x, -y, -z };
}

template<class T>
Vec3<T>& Vec3<T>::operator+=(T scalar)
{
	x += scalar;
	y += scalar;
	z += scalar;
	return *this;
}

template<class T>
Vec3<T>& Vec3<T>::operator-=(T scalar)
{
	x -= scalar;
	y -= scalar;
	z -= scalar;
	return *this;
}

template<class T>
Vec3<T>& Vec3<T>::operator*=(T scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

template<class T>
Vec3<T>& Vec3<T>::operator/=(T scalar)
{
	// Check for division by zero
	if (scalar != T())
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
	}
	return *this;
}

//template<class T>
//Vec3<T> Vec3<T>::Rotate(double radians) const
//{
//	double c = cos(radians);
//	double s = sin(radians);
//	double tx = x * c - y * s;
//	double ty = x * s + y * c;
//	return { tx, ty };
//}

//template<class T>
//Vec3<T> Vec3<T>::RotateF(float radians) const
//{
//	float c = cosf(radians);
//	float s = sinf(radians);
//	float tx = x * c - y * s;
//	float ty = x * s + y * c;
//	return { tx, ty };
//}

template<class T>
Vec3<T> Vec3<T>::Normalize() const
{
	double len = Length();
	constexpr double epsilon = 1e-10;

	if (len > epsilon)
	{
		double inv_len = 1.0 / len;
		return {
			static_cast<T>(x * inv_len),
			static_cast<T>(y * inv_len),
			static_cast<T>(z * inv_len)
		};
	}
	return { x, y, z };
}

template<class T>
Vec3<T> Vec3<T>::NormalizeF() const
{
	float len = LengthF();
	constexpr float epsilon = 1e-10f;

	if (len > epsilon)
	{
		float inv_len = 1.0f / len;
		return {
			static_cast<T>(x * inv_len),
			static_cast<T>(y * inv_len),
			static_cast<T>(z * inv_len),
		};
	}
	return { x, y, z };
}

template<class T>
Vec3<T> Vec3<T>::SetLength(double length) const
{
	double len = Length();
	if (len != 0.0)
	{
		return {
			static_cast<T>(x / len * length),
			static_cast<T>(y / len * length),
			static_cast<T>(z / len * length)
		};
	}
	return { x, y, z };
}

template<class T>
Vec3<T> Vec3<T>::SetLengthF(float length) const
{
	float len = LengthF();
	if (len != 0.0f)
	{
		return {
			static_cast<T>(x / len * length),
			static_cast<T>(y / len * length),
			static_cast<T>(z / len * length),
		};
	}
	return { x, y, z };
}

template<class T>
bool Vec3<T>::operator==(const Vec3<T>& v)
{
	return x == v.x && y == v.y && z == v.z;
}

template<class T>
bool Vec3<T>::operator!=(const Vec3<T>& v)
{
	return x != v.x || y != v.y || z != v.z;
}

template<class T>
double Vec3<T>::Length() const
{
	return std::sqrt(
		static_cast<double>(x) * static_cast<double>(x) +
			static_cast<double>(y) * static_cast<double>(y) +
			static_cast<double>(z) * static_cast<double>(z)
	);
}

template<class T>
float Vec3<T>::LengthF() const
{
	return std::sqrt(
		static_cast<float>(x) * static_cast<float>(x) +
			static_cast<float>(y) * static_cast<float>(y) +
			static_cast<float>(z) * static_cast<float>(z)
	);
}

template<class T>
double Vec3<T>::LengthSquared() const
{
	return static_cast<double>(x) * static_cast<double>(x) +
		static_cast<double>(y) * static_cast<double>(y) +
		static_cast<double>(z) * static_cast<double>(z);
}

template<class T>
float Vec3<T>::LengthSquaredF() const
{
	return static_cast<float>(x) * static_cast<float>(x) +
		static_cast<float>(y) * static_cast<float>(y) +
		static_cast<float>(z) * static_cast<float>(z);
}

template<class T>
[[nodiscard]] Vec3<T> CrossProductVec3(const Vec3<T>& v1, const Vec3<T>& v2)
{
	return Vec3<T>(
		v1.y * v2.z - v1.z * v2.y,   // x component
		v1.z * v2.x - v1.x * v2.z,   // y component
		v1.x * v2.y - v1.y * v2.x    // z component
	);
}

template<class T>
[[nodiscard]] T DotProductVec3(const Vec3<T>& v1, const Vec3<T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

//template<class T>
//double Vec3<T>::Atan2() const
//{
//	return std::atan2(static_cast<double>(y), static_cast<double>(x));
//}

//template<class T>
//float Vec3<T>::Atan2F() const
//{
//	return atan2f(static_cast<float>(y), static_cast<float>(x));
//}

//float DistanceVec3i(const Vec3i& v1, const Vec3i& v2)
//{
//	return (v1 - v2).LengthF();
//}
//
//float DistanceVec3f(const Vec3f& v1, const Vec3f& v2)
//{
//	return (v1 - v2).LengthF();
//}
//
//double DistanceVec3d(const Vec3d& v1, const Vec3d& v2)
//{
//	return (v1 - v2).Length();
//}

//Vec3f FromAngleVec3f(float radians)
//{
//	return { cosf(radians), sinf(radians) };
//}
//
//Vec3d FromAngleVec3d(double radians)
//{
//	return { cos(radians), sin(radians) };
//}
