//
// Created by Matq on 14/04/2023.
//

#pragma once

#include "TextManager.h"

class AssetsManager {
private:
    TextManager* m_TextHandler;

    bool m_ImagesInitialized;

public:
    AssetsManager();
    ~AssetsManager();

    // Getting
    [[nodiscard]] TextManager* TextHandler() const { return m_TextHandler; }

    // Manipulating
    void DeinitializeImages();

};
