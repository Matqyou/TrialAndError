//
// Created by Matq on 13/12/2024.
//

#include "ErrorSlowdown.h"

LoadedTexture ErrorSlowdown::sTextureSlowdown("icons.slowdown");

ErrorSlowdown::ErrorSlowdown(ErrorStatuses* parent)
 : ErrorStatusEffect(parent,
                     ErrorType::SLOW_DOWN,
                     "Slowdown",
                     sTextureSlowdown.GetTexture(),
                     1500) {

}
