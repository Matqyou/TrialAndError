//
// Created by Matq on 03/11/2025.
//

#include <cmath>
#include "Vec4.h"

template<class T>
Vec4<T>::Vec4() : x(T()), y(T()), z(T()), w(T()) { }

template<class T>
Vec4<T>::Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }

template<class T>
Vec4<T>::Vec4(const Vec4& v) : x(v.x), y(v.y), z(v.z), w(v.w) { }

template<class T>
template<class U>
Vec4<T>::Vec4(const Vec4<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)), z(static_cast<T>(other.z)), w(static_cast<T>(other.w)) { }

template<class T>
Vec4<T>& Vec4<T>::operator=(const Vec4& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

template<class T>
Vec4<T> Vec4<T>::operator+(const Vec4& v) const
{
	return Vec4(x + v.x, y + v.y, z + v.z, w + v.w);
}

template<class T>
Vec4<T> Vec4<T>::operator-(const Vec4& v) const
{
	return Vec4(x - v.x, y - v.y, z - v.z, w - v.w);
}

template<class T>
Vec4<T> Vec4<T>::operator*(const Vec4& v) const
{
	return Vec4(x * v.x, y * v.y, z * v.z, w * v.w);
}

template<class T>
Vec4<T> Vec4<T>::operator/(const Vec4& v) const
{
	return Vec4(x / v.x, y / v.y, z / v.z, w / v.w);
}

template<class T>
Vec4<T>& Vec4<T>::operator+=(const Vec4& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

template<class T>
Vec4<T>& Vec4<T>::operator-=(const Vec4& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

template<class T>
Vec4<T>& Vec4<T>::operator*=(const Vec4& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	w *= v.w;
}

template<class T>
Vec4<T>& Vec4<T>::operator/=(const Vec4& v)
{
	x /= v.x;
	y /= v.y;
	z /= v.z;
	w /= v.w;
}

template<class T>
Vec4<T> Vec4<T>::operator+(T scalar) const
{
	return Vec4(x + scalar, y + scalar, z + scalar, w + scalar);
}

template<class T>
Vec4<T> Vec4<T>::operator-(T scalar) const
{
	return Vec4(x - scalar, y - scalar, z - scalar, w - scalar);
}

template<class T>
Vec4<T> Vec4<T>::operator*(T scalar) const
{
	return Vec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

template<class T>
Vec4<T> Vec4<T>::operator/(T scalar) const
{
	// Check for division by zero
	if (scalar != T())
	{
		return Vec4(x / scalar, y / scalar, z / scalar, w / scalar);
	}
	else
	{
		// Handle division by zero error
		// In this example, we simply return the original vector
		return *this;
	}
}

template<class T>
Vec4<T> Vec4<T>::operator-() const
{
	return { -x, -y, -z, -w };
}

template<class T>
Vec4<T>& Vec4<T>::operator+=(T scalar)
{
	x += scalar;
	y += scalar;
	z += scalar;
	w += scalar;
	return *this;
}

template<class T>
Vec4<T>& Vec4<T>::operator-=(T scalar)
{
	x -= scalar;
	y -= scalar;
	z -= scalar;
	w -= scalar;
	return *this;
}

template<class T>
Vec4<T>& Vec4<T>::operator*=(T scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

template<class T>
Vec4<T>& Vec4<T>::operator/=(T scalar)
{
	// Check for division by zero
	if (scalar != T())
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		w /= scalar;
	}
	return *this;
}

template<class T>
Vec4<T> Vec4<T>::Normalize() const
{
	double len = Length();
	constexpr double epsilon = 1e-10;

	if (len > epsilon)
	{
		double inv_len = 1.0 / len;
		return {
			static_cast<T>(x * inv_len),
			static_cast<T>(y * inv_len),
			static_cast<T>(z * inv_len),
			static_cast<T>(w * inv_len)
		};
	}
	return { x, y, z, w };
}

template<class T>
Vec4<T> Vec4<T>::NormalizeF() const
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
			static_cast<T>(w * inv_len),
		};
	}
	return { x, y, z, w };
}

template<class T>
Vec4<T> Vec4<T>::SetLength(double length) const
{
	double len = Length();
	if (len != 0.0)
	{
		return {
			static_cast<T>(x / len * length),
			static_cast<T>(y / len * length),
			static_cast<T>(z / len * length),
			static_cast<T>(w / len * length),
		};
	}
	return { x, y, z, w };
}

template<class T>
Vec4<T> Vec4<T>::SetLengthF(float length) const
{
	float len = LengthF();
	if (len != 0.0f)
	{
		return {
			static_cast<T>(x / len * length),
			static_cast<T>(y / len * length),
			static_cast<T>(z / len * length),
			static_cast<T>(w / len * length),
		};
	}
	return { x, y, z, w };
}

template<class T>
bool Vec4<T>::operator==(const Vec4<T>& v)
{
	return x == v.x && y == v.y && z == v.z && w == v.w;
}

template<class T>
bool Vec4<T>::operator!=(const Vec4<T>& v)
{
	return x != v.x || y != v.y || z != v.z || w != v.w;
}

template<class T>
double Vec4<T>::Length() const
{
	return std::sqrt(LengthSquared());
}

template<class T>
float Vec4<T>::LengthF() const
{
	return std::sqrt(LengthSquaredF());
}

template<class T>
double Vec4<T>::LengthSquared() const
{
	return static_cast<double>(x) * static_cast<double>(x) +
		static_cast<double>(y) * static_cast<double>(y) +
		static_cast<double>(z) * static_cast<double>(z) +
		static_cast<double>(w) * static_cast<double>(w);
}

template<class T>
float Vec4<T>::LengthSquaredF() const
{
	return static_cast<float>(x) * static_cast<float>(x) +
		static_cast<float>(y) * static_cast<float>(y) +
		static_cast<float>(z) * static_cast<float>(z) +
		static_cast<float>(w) * static_cast<float>(w);
}
