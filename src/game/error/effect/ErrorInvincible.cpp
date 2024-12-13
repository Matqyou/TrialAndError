//
// Created by Matq on 13/12/2024.
//

#include "ErrorInvincible.h"

LoadedTexture ErrorInvincible::sTextureInvincible("icons.invincible");

ErrorInvincible::ErrorInvincible(ErrorStatuses* parent)
 : ErrorStatusEffect(ErrorType::INVINCIBLE,
                     sTextureInvincible.GetTexture(),
                     parent,
                     1500) {

}

