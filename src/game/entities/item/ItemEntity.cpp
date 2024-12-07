//
// Created by Matq on 03/06/2023.
//

#include "ItemEntity.h"
#include "../characters/character/Character.h"

LoadedTexture ItemEntity::sTextureGlock("entity.items.glock");
LoadedTexture ItemEntity::sTextureShotgun("entity.items.shotgun");
LoadedTexture ItemEntity::sTextureBurst("entity.items.burst");
LoadedTexture ItemEntity::sTextureSniper("entity.items.sniper");
LoadedTexture ItemEntity::sTexturesMinigun[4] = {
    LoadedTexture("entity.items.minigun1"),
    LoadedTexture("entity.items.minigun2"),
    LoadedTexture("entity.items.minigun3"),
    LoadedTexture("entity.items.minigun4"),
};

void ItemEntity::SetTexture(ItemType item_type) {
    switch (item_type) {
        case ITEMTYPE_GLOCK: {
            m_Texture = sTextureGlock.GetTexture();
            break;
        }
        case ITEMTYPE_SHOTGUN: {
            m_Texture = sTextureShotgun.GetTexture();
            break;
        }
        case ITEMTYPE_BURST: {
            m_Texture = sTextureBurst.GetTexture();
            break;
        }
        case ITEMTYPE_SNIPER: {
            m_Texture = sTextureSniper.GetTexture();
            break;
        }
        case ITEMTYPE_MINIGUN: {
            m_Texture = sTexturesMinigun[0].GetTexture(); // TODO depend on rotation
            break;
        }
    }
}

ItemEntity::ItemEntity(GameWorld* world,
                       ItemType item_type,
                       Entity* dropper,
                       const Vec2d& start_pos,
                       const Vec2d& start_size)
    : Entity(world,
             NORMAL_ENTITY,
             ITEM_ENTITY,
             start_pos,
             start_size,
             Vec2d(0.0, 0.0),
             0.95,
             false) {
    m_Dropper = dropper;
    m_DroppedSince = m_World->GetTick();
    m_PickupCooldown = (unsigned long long)(m_World->GameWindow()->Timer()->GetFramerate());
    m_ItemType = item_type;
    m_Texture = nullptr;
    //m_PickupRadius = 25.0;
    m_Rotation = 0.0;
    m_RotationalVelocity = 0;
    m_RotationalDamping = 0.95;

    SetTexture(item_type);
}

void ItemEntity::EventPickup(Character& picker_char) {
    m_Alive = false;
}

void ItemEntity::TickPickup() {
    if (!m_Alive)
        return;

    auto Char = m_World->FirstCharacter();
    for (; Char; Char = (Character*)Char->NextType()) {
        // The code below makes me think what if the m_Dropper entity was dead a long time ago and now
        // a new entity has been summoned with the exact same address... HMMMMMMMM
        // In a world where the pickup cooldown is infinite this entity is doomed to suffer
        // HE will never be able to pick up the item he always wanted........
        // Eh theres probably a way for that entity to reincarnate, but if there isn't there's nothing
        // really he can do about this situation
        // Todo: think of some connected/smart pointers cuz that is the next big thing i need to learn
        if (!Char->IsAlive() || (Char == m_Dropper && m_World->GetTick() - m_DroppedSince < m_PickupCooldown)) continue;
        double Distance = DistanceVec2(m_Core.Pos, Char->GetCore().Pos);
        if (Distance > m_Core.sizeRatio + Char->GetCore().sizeRatio) continue;

        EventPickup(*Char);
        break;
    }
}

void ItemEntity::SetRotation(double rotation) {
    m_Rotation = rotation;
}

void ItemEntity::AccelerateRotation(double acceleration) {
    m_RotationalVelocity += acceleration;
}

void ItemEntity::Tick() {
    TickPickup();
    TickVelocity();
    m_Rotation += m_RotationalVelocity;
    m_RotationalVelocity *= m_RotationalDamping;
}

void ItemEntity::Draw() {
    if (m_Texture == nullptr)
        return;

    Drawing* Render = m_World->GameWindow()->Render();

    SDL_Rect DrawRect = { int(m_Core.Pos.x - m_Core.Size.x / 2.0),
                          int(m_Core.Pos.y - m_Core.Size.y / 2.0),
                          int(m_Core.Size.x),
                          int(m_Core.Size.y) };

    Render->RenderTextureExCamera(m_Texture->SDLTexture(),
                                  nullptr,
                                  DrawRect,
                                  m_Rotation / M_PI * 180.0,
                                  nullptr,
                                  SDL_FLIP_NONE);
}
