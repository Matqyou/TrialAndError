//
// Created by Matq on 04/06/2023.
//

#include "EntityGlock.h"
#include "../character/Character.h"

void EntityGlock::EventPickup(Character* picker_char) {
    picker_char->GiveWeapon(WEAPON_GLOCK);
    m_Alive = false;
}

EntityGlock::EntityGlock(GameWorld* world, double start_x, double start_y)
    : ItemEntity(world, ITEMTYPE_GLOCK,
                 start_x, start_y,
                 12*4, 8*4) {

}
