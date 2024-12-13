//
// Created by Matq on 13/12/2024.
//

#include "ErrorRangedFists.h"

LoadedTexture ErrorRangedFists::sTextureRangedFists("icons.ranged_fists");

ErrorRangedFists::ErrorRangedFists(ErrorStatuses* parent)
    : ErrorStatusEffect(ErrorType::RANGED,
                        sTextureRangedFists.GetTexture(),
                        parent,
                        3000) {

}
