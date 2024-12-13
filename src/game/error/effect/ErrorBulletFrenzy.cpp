//
// Created by Matq on 13/12/2024.
//

#include "ErrorBulletFrenzy.h"

LoadedTexture ErrorBulletFrenzy::sTextureBulletFrenzy("icons.bullet_frenzy");

ErrorBulletFrenzy::ErrorBulletFrenzy(ErrorStatuses* parent)
 : ErrorStatusEffect(ErrorType::BULLET_FRENZY,
                     sTextureBulletFrenzy.GetTexture(),
                     parent,
                     3000) {

}
