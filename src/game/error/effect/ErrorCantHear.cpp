//
// Created by Matq on 13/12/2024.
//

#include "ErrorCantHear.h"

LoadedTexture ErrorCantHear::sTextureCantHear("icons.cant_hear");

ErrorCantHear::ErrorCantHear(ErrorStatuses* parent)
 : ErrorStatusEffect(ErrorType::I_CANT_HEAR,
                     sTextureCantHear.GetTexture(),
                     parent,
                     3000) {

}