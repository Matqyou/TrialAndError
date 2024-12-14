//
// Created by Matq on 13/12/2024.
//

#include "ErrorHealersParadise.h"

LoadedTexture ErrorHealersParadise::sTextureHealersParadise("icons.healers_paradise");

ErrorHealersParadise::ErrorHealersParadise(ErrorStatuses* parent)
  : ErrorStatusEffect(parent,
                      ErrorType::HEALERS_PARADISE,
                      "Healers Paradise",
                      sTextureHealersParadise.GetTexture(),
                      1500) {

}
