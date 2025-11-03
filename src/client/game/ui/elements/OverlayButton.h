//
// Created by Matq on 13/01/2025.
//

#pragma once

#include "Button.h"

class OverlayButton : public Button {
protected:
    VisualTextureInstance overlay_texture_instance;

public:
    OverlayButton(Texture* texture, Texture* pressed_texture, Texture* overlay_texture);

    // Options
    Button* SetOverlayTexture(Texture* texture) {
        this->overlay_texture_instance.ChangeTexture(texture);
        return this;
    }

    // Ticking
    void PostRefresh() override;
    void Render() override;
    void UpdateOverlayTexture();

};