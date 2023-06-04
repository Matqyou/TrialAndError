//
// Created by Matq on 03/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_

#include "../Entity.h"

enum ItemType {
    ITEMTYPE_GLOCK,
    ITEMTYPE_SHOTGUN,
    NUM_ITEMTYPES
};

class ItemEntity : public Entity {
protected:
    ItemType m_ItemType;
    Texture* m_Texture;
    double m_PickupRadius;

    // Setting & Initializing
    void SetTexture(ItemType item_type);


    // Listening & Ticking
    virtual void EventPickup(Character* picker_char);
    void TickPickup();

public:
    static Texture* ms_TextureGlock;
    static Texture* ms_TextureShotgun;

    ItemEntity(GameWorld* world,
               ItemType item_type,
               double start_x, double start_y,
               double start_w, double start_h);

    // Ticking
    void Tick() override;
    void Draw() override;
};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_
