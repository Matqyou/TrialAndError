//
// Created by Matq on 03/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_

#include "../Entity.h"

enum ItemType {
    ITEMTYPE_GLOCK,
    ITEMTYPE_SHOTGUN,
    ITEMTYPE_BURST,
    ITEMTYPE_MINIGUN,
    NUM_ITEMTYPES
};

class ItemEntity : public Entity {
protected:
    Entity* m_Dropper;
    ItemType m_ItemType;
    Texture* m_Texture;
    double m_PickupRadius;
    unsigned long long m_DroppedSince;
    unsigned long long m_PickupCooldown;

    // Setting & Initializing
    void SetTexture(ItemType item_type);


    // Listening & Ticking
    virtual void EventPickup(Character* picker_char);
    void TickPickup();

public:
    static Texture* ms_TextureGlock;
    static Texture* ms_TextureShotgun;
    static Texture* ms_TextureBurst;
    static Texture* ms_TexturesMinigun[4];

    ItemEntity(GameWorld* world,
               ItemType item_type,
               Entity* dropper,
               const Vec2d& start_pos,
               const Vec2d& start_size);

    // Ticking
    void Tick() override;
    void Draw() override;
};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_
