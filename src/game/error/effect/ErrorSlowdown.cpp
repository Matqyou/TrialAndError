//
// Created by Matq on 13/12/2024.
//

#include "ErrorSlowdown.h"

LoadedTexture ErrorSlowdown::sTextureSlowdown("icons.slowdown");

ErrorSlowdown::ErrorSlowdown(ErrorStatuses* parent)
 : ErrorStatusEffect(ErrorType::SLOW_DOWN,
                     sTextureSlowdown.GetTexture(),
                     parent,
                     1500) {

}
