//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "../base/ErrorStatusEffect.h"

class ErrorCantHear : public ErrorStatusEffect {
private:
    static LoadedTexture sTextureCantHear;

public:
    explicit ErrorCantHear(ErrorStatuses* parent);

};