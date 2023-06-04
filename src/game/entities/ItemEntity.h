//
// Created by Matq on 03/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_

#include "Entity.h"

enum ItemType {
    ITEM_GLOCK
};

class ItemEntity : public Entity {
private:
    ItemType m_ItemType;
    Texture* m_Texture;

    // Setting & Initializing
    void SetTexture(ItemType item_type);

public:
    static Texture* ms_TextureGlock;

    ItemEntity(GameWorld* world,
               ItemType item_type,
               double start_x, double start_y,
               double start_w, double start_h);

    // Ticking
    void Tick() override;
    void Draw() override;
};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_
