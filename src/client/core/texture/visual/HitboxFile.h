//
// Created by Matq on 28/03/2025.
//

#pragma once

#include "shared/utility/Rectangles.h"
#include <string>

class HitboxFile {
private:
    Rect4i hitbox;
    bool uses_hitbox;
    bool loaded;

public:
    HitboxFile();
    HitboxFile(const Rect4i& hitbox, bool uses_hitbox);
    ~HitboxFile();

    // Getting
    [[nodiscard]] const Rect4i& Hitbox() const { return hitbox; }
    [[nodiscard]] bool UsesHitbox() const { return uses_hitbox; }
    [[nodiscard]] bool IsPopulated() const { return loaded; }

    // Manipulating
    void Clear();
    bool SaveFile(const std::string& directory, const std::string& entry_name) const;
    bool OpenFile(const std::string& directory, const std::string& entry_name);
    void SetHitbox(const Rect4i& hitbox, bool uses_hitbox);

};
