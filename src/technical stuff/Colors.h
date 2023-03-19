//
// Created by Matq on 16/02/2023.
//

#ifndef CUSTOMTHREEDEESDL_COLORS_H_
#define CUSTOMTHREEDEESDL_COLORS_H_

struct ColorRGB {
    int r, g, b; // 0-255, 0-255, 0-255
};

struct ColorHSL {
    double h, s, l; // 0-360, 0-100, 0-100
};

ColorRGB HSVtoRGB(const ColorHSL& color_hsl);

#endif //CUSTOMTHREEDEESDL_COLORS_H_
