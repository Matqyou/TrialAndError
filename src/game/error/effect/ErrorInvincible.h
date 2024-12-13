//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "../base/ErrorStatusEffect.h"

class ErrorInvincible : public ErrorStatusEffect {
private:
    static LoadedTexture sTextureInvincible;

public:
    explicit ErrorInvincible(ErrorStatuses* parent);

};
