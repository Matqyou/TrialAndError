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

struct ErrorFrame {
    Vec2i Pos;
    Vec2i Size;
};

struct ErrorFrames {
    std::vector<ErrorFrame> Frames;

    ErrorFrames() = default;
    void CreateFrames(std::vector<ErrorStatusEffect*>& group) {
        auto num_elements = (int)group.size();
        const int columns = 2;
        const int rows = (int)(ceil((double)num_elements / columns));
        const int spacing = 1;
        const int frame_size = 20;
        const int iterate_size = frame_size + spacing;

        const int container_width = spacing + iterate_size * columns;
        const int container_height = spacing + iterate_size * rows;
        const int container_height2 = container_height / 2;

        int index = 0;
        for (auto effect : group) {
            int x = index % columns;
            int y = index / columns;

            ErrorFrame new_frame;
            new_frame.Pos = Vec2i(spacing + x * iterate_size, spacing + y * iterate_size - container_height2);
            new_frame.Size = Vec2i(frame_size, frame_size);
            Frames.push_back(new_frame);

            index++;
        }
    }

};

class ErrorStatuses {
private:
    GameWorld* m_World;
    Character* m_Parent;
    Drawing* m_Drawing;

    std::vector<ErrorStatusEffect*> m_Effects;
    ErrorFrames m_Frames;

    static LoadedTexture sTextureErrorFrame;
    static LoadedTexture sTextureErrorBackground;

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

    ErrorStatuses(GameWorld* world, Character* parent);

    // Getting
    [[nodiscard]] GameWorld* World() const { return m_World; }
    [[nodiscard]] std::vector<ErrorStatusEffect*>& Group() { return m_Effects; }

    // Generating
    bool AnyActive(double from_seconds_ago);

    // Ticking
    void Tick();
    void Draw();

};
