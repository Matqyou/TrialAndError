//
// Created by Matq on 23/05/2025.
//

#pragma once

#include "client/core/texture/visual/VisualTexture.h"
#include <string>
#include <vector>

class RenderPresets
{
public:
	static Texture *Composition(std::initializer_list<Texture*> textures, std::initializer_list<SDL_Color> colors);
};
