//
// Created by Matq on 13/12/2024.
//

#include "ErrorSpiky.h"

LoadedTexture ErrorSpiky::sTextureSpiky("icons.spiky");

ErrorSpiky::ErrorSpiky(ErrorStatuses* parent)
 : ErrorStatusEffect(ErrorType::SPIKY,
                     sTextureSpiky.GetTexture(),
                     parent,
                     3000) {

}
