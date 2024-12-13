//
// Created by Matq on 13/12/2024.
//

#include "ErrorHealersParadise.h"

LoadedTexture ErrorHealersParadise::sTextureHealersParadise("icons.healers_paradise");

ErrorHealersParadise::ErrorHealersParadise(ErrorStatuses* parent)
  : ErrorStatusEffect(ErrorType::HEALERS_PARADISE,
                      sTextureHealersParadise.GetTexture(),
                      parent,
                      1500) {

}
