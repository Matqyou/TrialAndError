//
// Created by Matq on 03/06/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_
#define TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_

#include "Entity.h"

class ItemEntity : public Entity {
private:
    Texture* m_Texture;

public:
    ItemEntity();

};

#endif //TRIALANDERROR_SRC_GAME_ENTITIES_ITEMENTITY_H_
