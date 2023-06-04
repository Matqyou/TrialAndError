//
// Created by Matq on 04/06/2023.
//

#include "EntityShotgun.h"
#include "../character/Character.h"

void EntityShotgun::EventPickup(Character* picker_char) {
    picker_char->GiveWeapon(WEAPON_SHOTGUN);
    m_Alive = false;
}

EntityShotgun::EntityShotgun(GameWorld* world, double start_x, double start_y)
    : ItemEntity(world, ITEMTYPE_SHOTGUN,
                 start_x, start_y,
                 21*4, 6*4) {

}