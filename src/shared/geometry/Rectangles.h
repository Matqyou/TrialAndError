//
// Created by Matq on 10/01/2025.
//

#pragma once

#include "shared/math/Vec2.h"
#include "SDL3/SDL_rect.h"

template<class T>
struct Rect4 {
    T x, y, w, h;

    Rect4();
    Rect4(T X, T Y, T W, T H);
    Rect4(const Rect4& r);

    template<class U>
    explicit Rect4(const Rect4<U>& other);

    // Getting
    [[nodiscard]] Vec2<T> Position() const { return Vec2<T>(x, y); }
    [[nodiscard]] Vec2<T> Size() const { return Vec2<T>(w, h); }

    // Manipulative operations
    Rect4& operator=(const Rect4& v);

};

typedef Rect4<int> Rect4i;
typedef Rect4<float> Rect4f;
typedef Rect4<double> Rect4d;

namespace Rectangles {
template<typename T>
Rect4<T> PointsToRect(const Vec2<T>& point_a, const Vec2<T>& point_b) {
    Vec2<T> low, high;
    if (point_a.x < point_b.x) {
        low.x = point_a.x;
        high.x = point_b.x;
    } else {
        low.x = point_b.x;
        high.x = point_a.x;
    }

    if (point_a.y < point_b.y) {
        low.y = point_a.y;
        high.y = point_b.y;
    } else {
        low.y = point_b.y;
        high.y = point_a.y;
    }

    return Rect4<T>(low.x, low.y, high.x - low.x, high.y - low.y);
}

template<typename T>
Vec2<T> ScaleByWidth(const Vec2<T>& original, T new_width) {
    double height_to_width = static_cast<double>(original.y) / static_cast<double>(original.x);
    return { new_width, static_cast<T>(height_to_width * new_width) };
}

template<typename T>
Vec2<T> ScaleByHeight(const Vec2<T>& original, T new_height) {
    double width_to_height = static_cast<double>(original.x) / static_cast<double>(original.y);
    return { static_cast<T>(width_to_height * new_height), new_height };
}

template<typename T>
Vec2<T> ScaleToBounds(const Vec2<T>& original, const Vec2<T>& bounds) {
    double original_ratio = static_cast<double>(original.x) / static_cast<double>(original.y);
    double bounds_ratio = static_cast<double>(bounds.y) / static_cast<double>(bounds.x);

    if (original_ratio < bounds_ratio) { return ScaleByHeight(original, bounds.y); }
    else { return ScaleByWidth(original, bounds.x); }
}

template<typename T>
SDL_FRect CenterRelative(const Vec2<T>& original, const Vec2<T>& container) {
    return { static_cast<float>(container.x - original.x) / 2.0f,
             static_cast<float>(container.y - original.y) / 2.0f,
             static_cast<float>(original.x),
             static_cast<float>(original.y) };
}

inline SDL_FRect ClipBounds(const SDL_FRect& original, const SDL_FRect& container) {
	SDL_FRect clipped = original;

	if (clipped.x < container.x) {
		float overflow = container.x - clipped.x;
		clipped.x = container.x;
		clipped.w -= overflow;  // Reduce width by the amount we moved right
	}

	if (clipped.y < container.y) {
		float overflow = container.y - clipped.y;
		clipped.y = container.y;
		clipped.h -= overflow;  // Reduce height by the amount we moved down
	}

	if (clipped.x + clipped.w > container.x + container.w) {
		clipped.w = (container.x + container.w) - clipped.x;
	}

	if (clipped.y + clipped.h > container.y + container.h) {
		clipped.h = (container.y + container.h) - clipped.y;
	}

	if (clipped.w < 0) clipped.w = 0;
	if (clipped.h < 0) clipped.h = 0;

	return clipped;
}

template<typename T>
bool PointCollides(T x, T y, T rect_x, T rect_y, T rect_x2, T rect_y2) {
    return !(x < rect_x || x > rect_x2 || y < rect_y|| y > rect_y2);
}

}

#include "Rectangles.inl"
