//
// Created by Matq on 04/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_

#include "ItemEntity.h"
#include "../../weapons/projectile/WeaponGlock.h"
#include "../../weapons/projectile/WeaponShotgun.h"
#include "../../weapons/projectile/WeaponBurst.h"
#include "../../weapons/projectile/WeaponMinigun.h"

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
    WeaponShotgun* m_Shotgun;
    void EventPickup(Character& picker_char) override;

public:
    EntityShotgun(GameWorld* world, Entity* dropper, WeaponShotgun* shotgun, const Vec2d& start_pos);
    ~EntityShotgun();
};

class EntityBurst : public ItemEntity {
private:
    WeaponBurst* m_Burst;
    // Listening
    void EventPickup(Character& picker_char) override;

public:
    EntityBurst(GameWorld* world, Entity* dropper, WeaponBurst* burst, const Vec2d& start_pos);
    ~EntityBurst();
};

class EntityMinigun : public ItemEntity {
private:
    WeaponMinigun* m_Minigun;
    // Listening
    void EventPickup(Character& picker_char) override;

public:
    EntityMinigun(GameWorld* world, Entity* dropper, WeaponMinigun* minigun, const Vec2d& start_pos);
    ~EntityMinigun();
};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEM_ENTITYGUNS_H_
