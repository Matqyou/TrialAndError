//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "effect/ErrorBulletFrenzy.h"
#include "effect/ErrorCantHear.h"
#include "effect/ErrorConfusingHealth.h"
#include "effect/ErrorDangerousRecoil.h"
#include "effect/ErrorDisoriented.h"
#include "effect/ErrorHealersParadise.h"
#include "effect/ErrorInvincible.h"
#include "effect/ErrorRangedFists.h"
#include "effect/ErrorSlowdown.h"
#include "effect/ErrorSpiky.h"
#include "effect/ErrorTeleport.h"
#include "../interface/Interface.h"

struct ErrorFrame {
    Vec2i Pos;
    Vec2i Size;
};

struct ErrorFrames {
    std::vector<ErrorFrame> Frames;

    ErrorFrames() = default;
    void CreateFrames(std::vector<ErrorStatusEffect*>& group);
    void CreateFramesUI(std::vector<ErrorStatusEffect*>& group);

};

class ErrorStatuses : public InterfaceElement {
private:
    Character* m_Parent;
    Drawing* m_Drawing;
    bool m_Gui; // temp

    std::vector<ErrorStatusEffect*> m_Effects;
    ErrorFrames m_Frames;

    static LoadedTexture sTextureErrorFrame;
    static LoadedTexture sTextureErrorBackground;

    static LoadedTexture sTextureFrame;

    void Draw1(); // temp
    void Draw2(); // temp

public:
    ErrorBulletFrenzy BulletFrenzy;
    ErrorCantHear CantHear;
    ErrorConfusingHealth ConfusingHealth;
    ErrorDangerousRecoil DangerousRecoil;
    ErrorDisoriented Disoriented;
    ErrorHealersParadise HealersParadise;
    ErrorInvincible Invincible;
    ErrorRangedFists RangedFists;
    ErrorSlowdown Slowdown;
    ErrorSpiky Spiky;
    ErrorTeleport Teleport;

    ErrorStatuses(Interface* interface, Character* parent, bool gui);

    // Getting
    [[nodiscard]] std::vector<ErrorStatusEffect*>& Group() { return m_Effects; }

    // Generating
    bool AnyActive(double from_seconds_ago);

    // Ticking
    void Tick();
    void Draw() override;

};
