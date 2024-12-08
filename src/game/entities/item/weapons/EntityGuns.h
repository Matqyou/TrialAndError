//
// Created by Matq on 04/06/2023.
//

#pragma once

#include "../ItemEntity.h"
#include "../../../weapons/projectile/WeaponGlock.h"
#include "../../../weapons/projectile/WeaponShotgun.h"
#include "../../../weapons/projectile/WeaponBurst.h"
#include "../../../weapons/projectile/WeaponSniper.h"
#include "../../../weapons/projectile/WeaponMinigun.h"
#include "../../../weapons/projectile/PatersonNavy.h"

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

class EntitySniper : public ItemEntity {
private:
    WeaponSniper* m_Sniper;
    // Listening
    void EventPickup(Character& picker_char) override;

public:
    EntitySniper(GameWorld* world, Entity* dropper, WeaponSniper* sniper, const Vec2d& start_pos);
    ~EntitySniper();
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

class EntityPatersonNavy : public ItemEntity {
private:
    PatersonNavy* m_PatersonNavy;

    void EventPickup(Character& picker_char) override;

public:
    EntityPatersonNavy(GameWorld* world, Entity* dropper, PatersonNavy* paterson_navy, const Vec2d& start_pos);
    ~EntityPatersonNavy();

};