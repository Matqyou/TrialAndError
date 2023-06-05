//
// Created by Matq on 04/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_

#include "ItemEntity.h"
#include "../../weapons/projectile/WeaponGlock.h"

class EntityGlock : public ItemEntity {
private:
    WeaponGlock* m_Glock;

    void EventPickup(Character& picker_char) override;

public:
    EntityGlock(GameWorld* world, Entity* dropper, WeaponGlock* glock, const Vec2d& start_pos);
    ~EntityGlock();

};

class EntityShotgun : public ItemEntity {
private:
    void EventPickup(Character& picker_char) override;

public:
    EntityShotgun(GameWorld* world, Entity* dropper, const Vec2d& start_pos);
};

class EntityBurst : public ItemEntity {
private:

    // Listening
    void EventPickup(Character& picker_char) override;

public:
    EntityBurst(GameWorld* world, Entity* dropper, const Vec2d& start_pos);
};

class EntityMinigun : public ItemEntity {
private:

    // Listening
    void EventPickup(Character& picker_char) override;

public:
    EntityMinigun(GameWorld* world, Entity* dropper, const Vec2d& start_pos);
};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_
