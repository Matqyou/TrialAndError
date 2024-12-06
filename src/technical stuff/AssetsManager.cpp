//
// Created by Matq on 14/04/2023.
//

#include "AssetsManager.h"

AssetsManager::AssetsManager() {
    m_TextHandler = new TextManager();

    m_ImagesInitialized = true;
}

AssetsManager::~AssetsManager() {
    DeinitializeImages();
}

void AssetsManager::DeinitializeImages() {
    if (m_ImagesInitialized) {
        m_ImagesInitialized = false;
        delete m_TextHandler;
    }
}
