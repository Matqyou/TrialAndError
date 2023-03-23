//
// Created by Matq on 16/02/2023.
//

#ifndef CUSTOMTHREEDEESDL_COLORS_H_
#define CUSTOMTHREEDEESDL_COLORS_H_

#include <SDL.h>

struct ColorHSL {
    double h, s, l; // 0-360, 0-100, 0-100
};

SDL_Color HSLtoRGB(const ColorHSL& color_hsl);

#endif //CUSTOMTHREEDEESDL_COLORS_H_
