//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "../base/ErrorStatusEffect.h"

class ErrorHealersParadise : public ErrorStatusEffect {
private:
    static LoadedTexture sTextureHealersParadise;

public:
    explicit ErrorHealersParadise(ErrorStatuses* parent);

};
