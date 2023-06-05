//
// Created by Matq on 04/06/2023.
//

#include "EntityGuns.h"
#include "../character/Character.h"

void EntityGlock::EventPickup(Character& picker_char) {
    picker_char.GiveWeapon(m_Glock);
    m_Glock = nullptr;
    m_Alive = false;
}

EntityGlock::EntityGlock(GameWorld* world, Entity* dropper, WeaponGlock* glock, const Vec2d& start_pos)
    : ItemEntity(world, ITEMTYPE_GLOCK, dropper, start_pos, Vec2d(12 * 4, 8 * 4)) {
    m_Glock = glock ? glock : new WeaponGlock(nullptr);
}

EntityGlock::~EntityGlock() {
    delete m_Glock;
}

void EntityShotgun::EventPickup(Character& picker_char) {
    picker_char.GiveWeapon(new WeaponShotgun(nullptr));
    m_Alive = false;
}

EntityShotgun::EntityShotgun(GameWorld* world, Entity* dropper, const Vec2d& start_pos)
    : ItemEntity(world, ITEMTYPE_SHOTGUN, dropper, start_pos, Vec2d(21 * 4, 6 * 4)) {

}

void EntityBurst::EventPickup(Character& picker_char) {
    picker_char.GiveWeapon(new WeaponBurst(nullptr));
    m_Alive = false;
}

EntityBurst::EntityBurst(GameWorld* world, Entity* dropper, const Vec2d& start_pos)
    : ItemEntity(world, ITEMTYPE_BURST, dropper, start_pos, Vec2d(20 * 4, 8 * 4)) {

}

void EntityMinigun::EventPickup(Character& picker_char) {
    picker_char.GiveWeapon(new WeaponMinigun(nullptr));
    m_Alive = false;
}

EntityMinigun::EntityMinigun(GameWorld* world, Entity* dropper, const Vec2d& start_pos)
    : ItemEntity(world, ITEMTYPE_MINIGUN, dropper, start_pos, Vec2d(17 * 4, 8 * 4)) {

}