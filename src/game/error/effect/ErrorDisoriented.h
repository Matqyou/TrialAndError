//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "../base/ErrorStatusEffect.h"

class ErrorDisoriented : public ErrorStatusEffect {
private:
    static LoadedTexture sTextureDisoriented;

public:
    explicit ErrorDisoriented(ErrorStatuses* parent);

};
