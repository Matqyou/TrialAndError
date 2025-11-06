//
// Created by Matq on 13/12/2024.
//

#include "ErrorDangerousRecoil.h"

LoadTexture sTextureDangerousRecoil("icons.dangerous_recoil", AssetsClass::TexturePurpose::GUI_ELEMENT);

ErrorDangerousRecoil::ErrorDangerousRecoil(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::DANGEROUS_RECOIL,
						"Dangerous Recoil",
						sTextureDangerousRecoil.GetTexture(),
						3000)
{

}
