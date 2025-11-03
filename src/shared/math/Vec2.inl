//
// Created by Matq on 04/06/2023.
//

#include <cmath>

template<class T>
Vec2<T>::Vec2() : x(T()), y(T()) { }

template<class T>
Vec2<T>::Vec2(T x, T y) : x(x), y(y) { }

template<class T>
Vec2<T>::Vec2(const Vec2& v) : x(v.x), y(v.y) { }

template<class T>
template<class U>
Vec2<T>::Vec2(const Vec2<U>& other) : x(static_cast<T>(other.x)), y(static_cast<T>(other.y)) { }

template<class T>
Vec2<T>& Vec2<T>::operator=(const Vec2& v)
{
	x = v.x;
	y = v.y;
	return *this;
}

template<class T>
Vec2<T> Vec2<T>::operator+(const Vec2& v) const
{
	return Vec2(x + v.x, y + v.y);
}

template<class T>
Vec2<T> Vec2<T>::operator-(const Vec2& v) const
{
	return Vec2(x - v.x, y - v.y);
}

template<class T>
Vec2<T> Vec2<T>::operator*(const Vec2& v) const
{
	return Vec2(x * v.x, y * v.y);
}

template<class T>
Vec2<T> Vec2<T>::operator/(const Vec2& v) const
{
	return Vec2(x / v.x, y / v.y);
}

template<class T>
Vec2<T>& Vec2<T>::operator+=(const Vec2& v)
{
	x += v.x;
	y += v.y;
	return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator-=(const Vec2& v)
{
	x -= v.x;
	y -= v.y;
	return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator*=(const Vec2& v)
{
	x *= v.x;
	y *= v.y;
}

template<class T>
Vec2<T>& Vec2<T>::operator/=(const Vec2& v)
{
	x /= v.x;
	y /= v.y;
}

template<class T>
Vec2<T> Vec2<T>::operator+(T scalar) const
{
	return Vec2(x + scalar, y + scalar);
}

template<class T>
Vec2<T> Vec2<T>::operator-(T scalar) const
{
	return Vec2(x - scalar, y - scalar);
}

template<class T>
Vec2<T> Vec2<T>::operator*(T scalar) const
{
	return Vec2(x * scalar, y * scalar);
}

template<class T>
Vec2<T> Vec2<T>::operator/(T scalar) const
{
	// Check for division by zero
	if (scalar != T())
	{
		return Vec2(x / scalar, y / scalar);
	}
	else
	{
		// Handle division by zero error
		// In this example, we simply return the original vector
		return *this;
	}
}

template<class T>
Vec2<T> Vec2<T>::operator-() const
{
	return { -x, -y };
}

template<class T>
Vec2<T>& Vec2<T>::operator+=(T scalar)
{
	x += scalar;
	y += scalar;
	return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator-=(T scalar)
{
	x -= scalar;
	y -= scalar;
	return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator*=(T scalar)
{
	x *= scalar;
	y *= scalar;
	return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator/=(T scalar)
{
	// Check for division by zero
	if (scalar != T())
	{
		x /= scalar;
		y /= scalar;
	}
	return *this;
}

template<class T>
Vec2<T> Vec2<T>::Rotate(double radians) const
{
	double c = cos(radians);
	double s = sin(radians);
	double tx = x * c - y * s;
	double ty = x * s + y * c;
	return { tx, ty };
}

template<class T>
Vec2<T> Vec2<T>::RotateF(float radians) const
{
	float c = cosf(radians);
	float s = sinf(radians);
	float tx = x * c - y * s;
	float ty = x * s + y * c;
	return { tx, ty };
}

template<class T>
Vec2<T> Vec2<T>::Normalize() const
{
	double len = Length();
	constexpr double epsilon = 1e-10;

	if (len > epsilon)
	{
		double inv_len = 1.0 / len;
		return {
			static_cast<T>(x * inv_len),
			static_cast<T>(y * inv_len)
		};
	}
	return { x, y };
}

template<class T>
Vec2<T> Vec2<T>::NormalizeF() const
{
	float len = LengthF();
	constexpr float epsilon = 1e-10f;

	if (len > epsilon)
	{
		float inv_len = 1.0f / len;
		return {
			static_cast<T>(x * inv_len),
			static_cast<T>(y * inv_len)
		};
	}
	return { x, y };
}

template<class T>
Vec2<T> Vec2<T>::SetLength(double length) const
{
	double len = Length();
	if (len != 0.0)
	{
		return {
			static_cast<T>(x / len * length),
			static_cast<T>(y / len * length)
		};
	}
	return { x, y };
}

template<class T>
Vec2<T> Vec2<T>::SetLengthF(float length) const
{
	float len = LengthF();
	if (len != 0.0f)
	{
		return {
			static_cast<T>(x / len * length),
			static_cast<T>(y / len * length)
		};
	}
	return { x, y };
}

template<class T>
bool Vec2<T>::operator==(const Vec2<T>& v)
{
	return x == v.x && y == v.y;
}

template<class T>
bool Vec2<T>::operator!=(const Vec2<T>& v)
{
	return x != v.x || y != v.y;
}

template<class T>
double Vec2<T>::Length() const
{
	return std::sqrt(
		static_cast<double>(x) * static_cast<double>(x) +
			static_cast<double>(y) * static_cast<double>(y)
	);
}

template<class T>
float Vec2<T>::LengthF() const
{
	return std::sqrt(
		static_cast<float>(x) * static_cast<float>(x) +
			static_cast<float>(y) * static_cast<float>(y)
	);
}

template<class T>
double Vec2<T>::LengthSquared() const
{
	return static_cast<double>(x) * static_cast<double>(x) +
		static_cast<double>(y) * static_cast<double>(y);

}

template<class T>
float Vec2<T>::LengthSquaredF() const
{
	return static_cast<float>(x) * static_cast<float>(x) +
		static_cast<float>(y) * static_cast<float>(y);
}

template<class T>
double Vec2<T>::Atan2() const
{
	return std::atan2(static_cast<double>(y), static_cast<double>(x));
}

template<class T>
float Vec2<T>::Atan2F() const
{
	return atan2f(static_cast<float>(y), static_cast<float>(x));
}

Vec2f FromAngleVec2f(float radians)
{
	return { cosf(radians), sinf(radians) };
}

Vec2d FromAngleVec2d(double radians)
{
	return { cos(radians), sin(radians) };
}
