//
// Created by Matq on 13/12/2024.
//

#include "ErrorDangerousRecoil.h"

LinkTexture sTextureDangerousRecoil("icons.dangerous_recoil");

ErrorDangerousRecoil::ErrorDangerousRecoil(ErrorStatuses *parent)
	: ErrorStatusEffect(parent,
						ErrorType::DANGEROUS_RECOIL,
						"Dangerous Recoil",
						sTextureDangerousRecoil.GetTexture(),
						3000)
{

}
