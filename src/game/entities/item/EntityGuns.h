//
// Created by Matq on 04/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_

#include "ItemEntity.h"

class EntityGlock : public ItemEntity {
private:
    void EventPickup(Character* picker_char) override;

public:
    EntityGlock(GameWorld* world, Entity* dropper, const Vec2d& start_pos);
};

class EntityShotgun : public ItemEntity {
private:
    void EventPickup(Character* picker_char) override;

public:
    EntityShotgun(GameWorld* world, Entity* dropper, const Vec2d& start_pos);
};

class EntityBurst : public ItemEntity {
private:

    // Listening
    void EventPickup(Character* picker_char) override;

public:
    EntityBurst(GameWorld* world, Entity* dropper, const Vec2d& start_pos);
};

class EntityMinigun : public ItemEntity {
private:

    // Listening
    void EventPickup(Character* picker_char) override;

public:
    EntityMinigun(GameWorld* world, Entity* dropper, const Vec2d& start_pos);
};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_
