//
// Created by Matq on 04/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGLOCK_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGLOCK_H_

#include "ItemEntity.h"

class EntityGlock : public ItemEntity {
private:

    // Listening
    void EventPickup(Character* picker_char) override;

public:
    EntityGlock(GameWorld* world, double start_x, double start_y);

};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGLOCK_H_
