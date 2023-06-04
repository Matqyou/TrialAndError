//
// Created by Matq on 04/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYSHOTGUN_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYSHOTGUN_H_

#include "ItemEntity.h"

class EntityShotgun : public ItemEntity {
private:

    // Listening
    void EventPickup(Character* picker_char) override;

public:
    EntityShotgun(GameWorld* world, double start_x, double start_y);

};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYSHOTGUN_H_
