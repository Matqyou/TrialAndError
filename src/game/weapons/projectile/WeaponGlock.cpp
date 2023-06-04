//
// Created by T450s on 01/04/2023.
//

#include "WeaponGlock.h"
#include "../../entities/character/Character.h"
#include "../../entities/Projectile.h"

Sound* WeaponGlock::ms_ShootSound = nullptr;
Sound* WeaponGlock::ms_ClickSound = nullptr;

WeaponGlock::WeaponGlock(Character* owner)
    : ProjectileWeapon(owner, WEAPON_GLOCK, 10, 15, 15 * 3, 35.0, false) {
    m_BaseRecoilForce = 3.0;
    m_RecoilForce = m_BaseRecoilForce;
    m_Damage = 7.5;
}

void WeaponGlock::Tick() {
    if (!m_Shooter->HasDangerousRecoil())m_RecoilForce = m_BaseRecoilForce;
    else if (m_RecoilForce != m_BaseRecoilForce * 3)m_RecoilForce = m_BaseRecoilForce * 3;
    TickTrigger();

    if (m_Shooter && m_Triggered) { // If want to trigger without an owner, need to save world somewhere
        GameWorld* World = m_Shooter->World();
        auto& ShooterCore = m_Shooter->GetDirectionalCore();
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
                           m_Shooter,
                           WEAPON_GLOCK,
                           m_Damage,
                           ShooterCore.Pos,
                           ProjectileVelocity);

            Vec2d Recoil = ShooterCore.Direction * -m_RecoilForce;
            m_Shooter->Accelerate(Recoil);
        } else {
            SoundHandler->PlaySound(ms_ClickSound);
        }
    }
}