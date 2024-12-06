//
// Created by Mim on 5/13/2023.
//

#include "Error.h"
#include <random>
#include <string>

LoadedTexture Error::sTextureErrorDisorianted("entities.disorianted");
LoadedTexture Error::sTextureErrorSpiky("entities.cactus");
LoadedTexture Error::sTextureErrorConfusingHP("entities.confusion");
LoadedTexture Error::sTextureErrorInvincible("entities.invincible");
LoadedTexture Error::sTextureErrorHealersParadise("entities.healer");
LoadedTexture Error::sTextureErrorRanged("entities.ranged");
LoadedTexture Error::sTextureErrorSlowDown("entities.clock");
LoadedTexture Error::sTextureErrorDangerousRecoil("entities.golden_apple");
LoadedSound Error::ms_PickupSounds[7] = {
    LoadedSound("errorpickupsound"),
    LoadedSound("errorpickupsound"),
    LoadedSound("errorpickupsound"),
    LoadedSound("errorpickupsound"),
    LoadedSound("errorpickupsound"),
    LoadedSound("errorpickupsound"),
    LoadedSound("errorpickupsound"),
};

Error::Error(GameWorld* world, const Vec2d& start_pos, int typeID)
    : Entity(world,
             NORMAL_ENTITY,
             ERROR_ENTITY,
             start_pos,
             Vec2d(45, 45),
             Vec2d(0.0, 0.0),
             0.95,
             false) {

    m_Type = static_cast<ErrorType>(typeID);
    if (m_Type == SPIKY) m_Texture = sTextureErrorSpiky.GetTexture();
    else if (m_Type == INVINCIBLE) m_Texture = sTextureErrorInvincible.GetTexture();
    else if (m_Type == SLOW_DOWN) m_Texture = sTextureErrorSlowDown.GetTexture();
    else if (m_Type == HEALERS_PARADISE) m_Texture = sTextureErrorHealersParadise.GetTexture();
    else if (m_Type == DISORIANTED) m_Texture = sTextureErrorDisorianted.GetTexture();
    else if (m_Type == CONFUSING_HP) m_Texture = sTextureErrorConfusingHP.GetTexture();
    else if (m_Type == RANGED) m_Texture = sTextureErrorRanged.GetTexture();
    else m_Texture = sTextureErrorDangerousRecoil.GetTexture();
}

void Error::TickPickup(double x, double y) {
    auto Char = m_World->FirstCharacter();
    for (; Char; Char = (Character*)(Char->NextType())) {
        auto& CharCore = Char->GetDirectionalCore();
        bool Collides = (CharCore.Pos.x - CharCore.Size.x/2-m_Core.sizeRatio < x) &&
            (CharCore.Pos.x + CharCore.Size.x/2+m_Core.sizeRatio > x) &&
            (CharCore.Pos.y - CharCore.Size.y/2-m_Core.sizeRatio < y) &&
            (CharCore.Pos.y + CharCore.Size.y/2+m_Core.sizeRatio > y);

        if (!Collides)
            continue;

        if (m_Type == DISORIANTED) {
            if (Char->IsNPC()) { Char->ReverseMovement(); }
            else {
                auto Plr = m_World->FirstCharacter();
                for (; Plr; Plr = (Character*)(Plr->NextType())) {
                    if (!Plr->IsNPC()) continue;
                    Plr->ReverseMovement();
                }
            }
        } else if (m_Type == CONFUSING_HP) {
            auto Plr = m_World->FirstCharacter();
            for (; Plr; Plr = (Character*)(Plr->NextType()))
                Plr->ConfuseHP();
        } else if (m_Type == INVINCIBLE) { Char->MakeInvincible(); }
        else if (m_Type == SPIKY) { Char->MakeSpiky(); }
        else if (m_Type == HEALERS_PARADISE) {
            if (Char->IsNPC()) { Char->MakeHealer(); }
            else {
                auto Plr = m_World->FirstCharacter();
                for (; Plr; Plr = (Character*)(Plr->NextType())) {
                    if (Plr->IsNPC()) continue;
                    Plr->MakeHealer();
                }
            }
        } else if (m_Type == RANGED) { Char->MakeRanged(); }
        else if (m_Type == SLOW_DOWN) {
            if (Char->IsNPC()) { Char->SlowDown(); }
            else {
                auto Plr = m_World->FirstCharacter();
                for (; Plr; Plr = (Character*)(Plr->NextType())) {
                    if (!Plr->IsNPC()) continue;
                    Plr->SlowDown();
                }
            }
        } else if (m_Type == DANGEROUS_RECOIL) { Char->ActivateDangerousRecoil(); }
        m_Alive = false;
    }
}

void Error::Tick() {
    TickPickup(m_Core.Pos.x, m_Core.Pos.y);
    TickWalls();
}

void Error::Draw() {
    Drawing* Render = m_World->GameWindow()->Render();

    SDL_FRect DrawRect = { float(m_Core.Pos.x) - float(m_Core.Size.x / 2.0),
                           float(m_Core.Pos.y) - float(m_Core.Size.y / 2.0),
                           float(m_Core.Size.x),
                           float(m_Core.Size.y) };

    Render->RenderTextureFCamera(m_Texture->SDLTexture(), nullptr, DrawRect);

}