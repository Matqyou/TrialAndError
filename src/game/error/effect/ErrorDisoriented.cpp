//
// Created by Matq on 13/12/2024.
//

#include "ErrorDisoriented.h"

LoadedTexture ErrorDisoriented::sTextureDisoriented("icons.disoriented");

ErrorDisoriented::ErrorDisoriented(ErrorStatuses* parent)
    : ErrorStatusEffect(ErrorType::DISORIANTED,
                        sTextureDisoriented.GetTexture(),
                        parent,
                        1500) {

}
