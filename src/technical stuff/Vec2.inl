//
// Created by Matq on 04/06/2023.
//

#include "Vec2.h"

template<class T>
Vec2<T>::Vec2() : x(T()), y(T()) {}

template<class T>
Vec2<T>::Vec2(T x, T y) : x(x), y(y) {}

template<class T>
Vec2<T>::Vec2(const Vec2& v) : x(v.x), y(v.y) {}

template<class T>
Vec2<T>& Vec2<T>::operator=(const Vec2& v) {
    x = v.x;
    y = v.y;
    return *this;
}

template<class T>
Vec2<T> Vec2<T>::operator+(const Vec2& v) const {
    return Vec2(x + v.x, y + v.y);
}

template<class T>
Vec2<T> Vec2<T>::operator-(const Vec2& v) const {
    return Vec2(x - v.x, y - v.y);
}

template<class T>
Vec2<T>& Vec2<T>::operator+=(const Vec2& v) {
    x += v.x;
    y += v.y;
    return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator-=(const Vec2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

template<class T>
Vec2<T> Vec2<T>::operator+(T scalar) const {
    return Vec2(x + scalar, y + scalar);
}

template<class T>
Vec2<T> Vec2<T>::operator-(T scalar) const {
    return Vec2(x - scalar, y - scalar);
}

template<class T>
Vec2<T> Vec2<T>::operator*(T scalar) const {
    return Vec2(x * scalar, y * scalar);
}

template<class T>
Vec2<T> Vec2<T>::operator/(T scalar) const {
    // Check for division by zero
    if (scalar != T()) {
        return Vec2(x / scalar, y / scalar);
    } else {
        // Handle division by zero error
        // In this example, we simply return the original vector
        return *this;
    }
}

template<class T>
Vec2<T>& Vec2<T>::operator+=(T scalar) {
    x += scalar;
    y += scalar;
    return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator-=(T scalar) {
    x -= scalar;
    y -= scalar;
    return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator*=(T scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

template<class T>
Vec2<T>& Vec2<T>::operator/=(T scalar) {
    // Check for division by zero
    if (scalar != T()) {
        x /= scalar;
        y /= scalar;
    }
    return *this;
}

template<class T>
void Vec2<T>::Rotate(double radians) {
    double c = cos(radians);
    double s = sin(radians);
    double tx = x * c - y * s;
    double ty = x * s + y * c;
    x = tx;
    y = ty;
}

template<class T>
Vec2<T>& Vec2<T>::Normalize() {
    double len = Length();
    if (len != 0) {
        x /= len;
        y /= len;
    }
    return *this;
}

template<class T>
bool Vec2<T>::operator==(const Vec2<T>& v) {
    return x == v.x && y == v.y;
}

template<class T>
bool Vec2<T>::operator!=(const Vec2<T>& v) {
    return x != v.x || y != v.y;
}

template<class T>
double Vec2<T>::Length() const {
    return std::sqrt(static_cast<double>(x) * static_cast<double>(x) +
        static_cast<double>(y) * static_cast<double>(y));
}

template<class T>
double Vec2<T>::Atan2() const {
    return std::atan2(y, x);
}

template<class T>
Vec2<T> Vec2<T>::Ortho() const {
    return Vec2(y, -x);
}

template<class T>
double Vec2<T>::DotProduct(const Vec2& v1, const Vec2& v2) {
    return static_cast<double>(v1.x) * static_cast<double>(v2.x) +
        static_cast<double>(v1.y) * static_cast<double>(v2.y);
}

template<class T>
double Vec2<T>::CrossProduct(const Vec2& v1, const Vec2& v2) {
    return (static_cast<double>(v1.x) * static_cast<double>(v2.y)) -
        (static_cast<double>(v1.y) * static_cast<double>(v2.x));
}

template<class T>
double DistanceVec2(const Vec2<T>& v1, const Vec2<T>& v2)  {
    double dx = static_cast<double>(v1.x) - static_cast<double>(v2.x);
    double dy = static_cast<double>(v1.y) - static_cast<double>(v2.y);
    return std::sqrt(dx * dx + dy * dy);
}

inline Vec2<double> AngleVec2d(double radians) {
    return { std::cos(radians), std::sin(radians) };
}