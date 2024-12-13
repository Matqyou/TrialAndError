//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "../base/ErrorStatusEffect.h"

class ErrorDangerousRecoil : public ErrorStatusEffect {
private:
    static LoadedTexture sTextureDangerousRecoil;

public:
    explicit ErrorDangerousRecoil(ErrorStatuses* parent);

};
