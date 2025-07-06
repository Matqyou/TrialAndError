//
// Created by Matq on 22/06/2025.
//

#pragma once

#include "client/game/powerups/PowerupManager.h"
#include "client/game/ui/elements/Button.h"

class PowerupButton : public Button
{
private:
	Powerup type;

	Texture* temp_texture;

public:
	explicit PowerupButton(Powerup powerup_type);
	~PowerupButton() override;

};
