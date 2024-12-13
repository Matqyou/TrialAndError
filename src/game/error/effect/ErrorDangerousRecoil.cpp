//
// Created by Matq on 13/12/2024.
//

#include "ErrorDangerousRecoil.h"

LoadedTexture ErrorDangerousRecoil::sTextureDangerousRecoil("icons.dangerous_recoil");

ErrorDangerousRecoil::ErrorDangerousRecoil(ErrorStatuses* parent)
    : ErrorStatusEffect(ErrorType::DANGEROUS_RECOIL,
                        sTextureDangerousRecoil.GetTexture(),
                        parent,
                        3000) {

}