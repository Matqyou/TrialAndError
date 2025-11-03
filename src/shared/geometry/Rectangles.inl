//
// Created by Matq on 02/02/2025.
//

#pragma once

#include "shared/geometry/Rectangles.h"

template<class T>
Rect4<T>::Rect4() : x(T()), y(T()), w(T()), h(T()) { }

template<class T>
Rect4<T>::Rect4(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) { }

template<class T>
Rect4<T>::Rect4(const Rect4& r) : x(r.x), y(r.y), w(r.w), h(r.h) { }

template<class T>
template<class U>
Rect4<T>::Rect4(const Rect4<U>& other)
    : x(static_cast<T>(other.x)),
      y(static_cast<T>(other.y)),
      w(static_cast<T>(other.w)),
      h(static_cast<T>(other.h)) { }

template<class T>
Rect4<T>& Rect4<T>::operator=(const Rect4& v) {
    x = v.x;
    y = v.y;
    w = v.w;
    h = v.h;
    return *this;
}

namespace Rectangles {

}
