//
// Created by Mim on 5/8/2023.
//
#include "characters/character/Character.h"
#include "Crate.h"
#include <iostream>
Texture* Crate::ms_TextureBox = nullptr;
Texture* Crate::ms_TextureBreakingBox1 = nullptr;
Texture* Crate::ms_TextureBreakingBox2 = nullptr;
Sound* Crate::ms_HitSound = nullptr;
Sound* Crate::ms_BoxSound = nullptr;

Crate::Crate(GameWorld* world,
             const Vec2d& start_pos,
             double Health,
             DropType RandomDrop)
    : Entity(world,
             ENTFORM_NORMAL,
             ENTTYPE_CRATE,
             start_pos,
             Vec2d(50, 50),
             Vec2d(0.0, 0.0),
             0.95) {
    m_World = world;
    m_Health = Health;
    m_Alive = true;
    m_Type = RandomDrop;
    m_Texture = &ms_TextureBox;

    auto Random = m_World->GameWindow()->Random();
    float RandomFloat = Random->PercentageFloat();
    if (RandomFloat <= 1 / 8.0f) typeID = DISORIANTED;
    else if (RandomFloat <= 2 / 8.0f) { typeID = SPIKY; }
    else if (RandomFloat <= 3 / 8.0f) { typeID = CONFUSING_HP; }
    else if (RandomFloat <= 4 / 8.0f) { typeID = INVINCIBLE; }
    else if (RandomFloat <= 5 / 8.0f) { typeID = HEALERS_PARADISE; }
    else if (RandomFloat <= 6 / 8.0f) { typeID = RANGED; }
    else if (RandomFloat <= 7 / 8.0f) { typeID = SLOW_DOWN; }
    else if (RandomFloat <= 8 / 8.0f) { typeID = DANGEROUS_RECOIL; }
}

Crate::~Crate()
{
    Character *Char = m_World->FirstCharacter();
    for (; Char; Char = (Character *)Char->NextType())
    {
        Hook *TargetHook = Char->GetHook();
        if (TargetHook->m_GrabbedEntity == this)
            TargetHook->Unhook();
    }
}

void Crate::Damage(double value) {
    Sound* BoxHitSound = ms_BoxSound;
    m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(BoxHitSound);
    m_Health -= value;
    if (m_Health < 10) m_Texture = &ms_TextureBreakingBox2;
    else if (m_Health < 20) m_Texture = &ms_TextureBreakingBox1;
}

void Crate::Tick() {
    TickWalls();

    if (m_Health <= 0) {
        m_Alive = false;
        m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(ms_HitSound);
        if (m_Type != ERROR) {
            auto Ammo_type = m_World->GameWindow()->Random()->UnsignedInt() % 4;
            new AmmoBox(m_World, AmmoType(Ammo_type), m_Core.Pos, 20);
        } else {
            new Error(m_World,
                      m_Core.Pos,
                      typeID); // To change the drop just change typeID the enum value of whatever ERROR is needed
        }
    }
}

void Crate::Draw() {
    
    Drawing* Render = m_World->GameWindow()->Render();

    SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2.0),
                           float(m_Core.Pos.y) - float(m_Core.Size.y / 2.0),
                           float(m_Core.Size.x),
                           float(m_Core.Size.y) };

    Render->RenderTextureFCamera((*m_Texture)->SDLTexture(), nullptr, DrawRect);
}
