//
// Created by T450s on 01/04/2023.
//

#include "WeaponSniper.h"
#include "../../entities/characters/character/Character.h"
#include "../../entities/Projectile.h"

Sound* WeaponSniper::ms_ShootSound = nullptr;
Sound* WeaponSniper::ms_ClickSound = nullptr;

WeaponSniper::WeaponSniper(DirectionalEntity* parent)
    : ProjectileWeapon(parent, WEAPON_SNIPER, 50, 1, 16, 100.0, false) {
    m_BaseRecoilForce = 12.0;
    m_RecoilForce = m_BaseRecoilForce;
    m_Damage = 70;
}

void WeaponSniper::Tick() {
    if (m_Parent->GetType() != ENTTYPE_CHARACTER) {
        std::printf("Warning: Weapon holder is not a characters (no support for error powerups)");
        return;
    }

    // TODO: recoil force shouldn't change every tick (make like an event function, call when timer starts and ends to update recoil force)
    // Do this for the remaining weapons aswell
    if (!((Character*)m_Parent)->HasDangerousRecoil()) m_RecoilForce = m_BaseRecoilForce;
    else if (m_RecoilForce != m_BaseRecoilForce * 3) m_RecoilForce = m_BaseRecoilForce * 3;
    TickTrigger();

    if (m_Parent && m_Triggered) { // If want to trigger without an owner, need to save world somewhere
        GameWorld* World = m_Parent->World();
        auto& ShooterCore = m_Parent->GetDirectionalCore();
        auto CurrentTick = World->GetTick();
        if (CurrentTick - m_LastShotAt <= m_TickCooldown)
            return;

        SoundManager* SoundHandler = World->GameWindow()->Assets()->SoundHandler();

        m_LastShot = m_Ammo == 1;
        if (m_Ammo) {
            m_Ammo--;
            m_LastShotAt = CurrentTick;
            SoundHandler->PlaySound(ms_ShootSound);

            Vec2d ProjectileVelocity = ShooterCore.Direction * m_ProjectileSpeed;
            new Projectile(World,
                           m_Parent,
                           WEAPON_GLOCK,
                           m_Damage,
                           ShooterCore.Pos,
                           ProjectileVelocity);

            Vec2d Recoil = ShooterCore.Direction * -m_RecoilForce;
            m_Parent->Accelerate(Recoil);
        } else {
            SoundHandler->PlaySound(ms_ClickSound);
        }
    }
}