//
// Created by Matq on 13/12/2024.
//

#include "ErrorConfusingHealth.h"

LoadedTexture ErrorConfusingHealth::sTextureConfusingHealth("icons.confusing_health");

ErrorConfusingHealth::ErrorConfusingHealth(ErrorStatuses* parent)
 : ErrorStatusEffect(ErrorType::CONFUSING_HP,
                     sTextureConfusingHealth.GetTexture(),
                     parent,
                     1500) {

}
