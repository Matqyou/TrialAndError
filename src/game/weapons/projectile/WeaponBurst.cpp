//
// Created by T450s on 01/04/2023.
//

#include "WeaponBurst.h"
#include "../../entities/character/Character.h"
#include "../../entities/Bullets.h"
#include <cmath>

Sound* WeaponBurst::ms_ShootSound = nullptr;
Sound* WeaponBurst::ms_ClickSound = nullptr;

WeaponBurst::WeaponBurst(Character* owner)
 : ProjectileWeapon(owner, WEAPON_BURST, 30, 24, 24 * 3, 35.0, false) {
    m_BaseRecoilForce = 3.0;
    m_RecoilForce = m_BaseRecoilForce;
    m_Damage = 7.5;
    m_BurstCooldown = 6;
    m_BurstShots = 3;
    m_owner;
    // Will get rewritten by other code
    m_BurstTick = 0;
    m_BurstShotsLeft = 0;
}
//void WeaponBurst::Draw(){
//    GameWorld* World = m_owner->World();
//    Drawing* Render = World->GameWindow()->RenderClass();
//
//    auto ShooterCore = m_owner->GetCore();
//    auto ShooterInput = m_owner->GetInput();
//    double XLook = ShooterCore->m_x + ShooterInput.m_LookingX * 50.0;
//    double YLook = ShooterCore->m_y + ShooterInput.m_LookingY * 50.0;
//    double Angle = std::atan2(ShooterInput.m_LookingY * 50.0, ShooterInput.m_LookingX * 50.0) / M_PI * 180.0;
//    SDL_RendererFlip flip;
//    if(Angle > 90 || Angle < -90){
//        flip = SDL_FLIP_VERTICAL;
//    }
//    else flip = SDL_FLIP_VERTICAL;
//    SDL_FRect GunRect = {float(ShooterCore->m_x), float(ShooterCore->m_y),15 ,100 };
//    // TODO Seperate this into gun classes id say and give gun class a different texture and make bullets spawn from the gun
//    // and not the center of the player
//    SDL_FPoint GunPoint = {float(0), float(0)};
//    if(m_owner->GetCurrentWeapon())Render->RenderTextureExFWorld(Character::ms_TextureError->SDLTexture(),nullptr, GunRect, Angle-90,&GunPoint, flip);
//
//}
void WeaponBurst::Tick() {
    if(!m_Shooter->GetIfDangerousRecoil())m_RecoilForce = m_BaseRecoilForce;
    else if (m_RecoilForce != m_BaseRecoilForce*3)m_RecoilForce = m_BaseRecoilForce*3;
    TickTrigger();

    if (m_Shooter) {
        GameWorld* World = m_Shooter->World();
        auto ShooterCore = (LookingEntityCore*)m_Shooter->GetCore();
        SoundManager* SoundHandler = World->GameWindow()->Assets()->SoundHandler();
        auto CurrentTick = World->CurrentTick();
        if (m_BurstShotsLeft && CurrentTick - m_BurstTick > m_BurstCooldown) {
            m_BurstTick = CurrentTick;
            m_BurstShotsLeft--;
            if (m_Ammo) {
                m_Ammo--;
                SoundHandler->PlaySound(ms_ShootSound);

                double VelocityX = ShooterCore->m_xlook * m_ProjectileSpeed;
                double VelocityY = ShooterCore->m_ylook * m_ProjectileSpeed;
                new Bullets(World, m_Shooter, WEAPON_BURST, m_Damage, ShooterCore->m_x, ShooterCore->m_y, VelocityX, VelocityY);

                double RecoilX = ShooterCore->m_xlook * -m_RecoilForce;
                double RecoilY = ShooterCore->m_ylook * -m_RecoilForce;
                m_Shooter->Accelerate(RecoilX, RecoilY);
            } else {
                SoundHandler->PlaySound(ms_ClickSound);
            }
        }

        if (m_Triggered) { // If want to trigger without an owner, need to save world somewhere
            if (CurrentTick - m_LastShotAt <= m_TickCooldown)
                return;

            m_BurstTick = CurrentTick;
            m_BurstShotsLeft = m_BurstShots - 1;
            m_LastShot = m_Ammo == 1;
            if (m_Ammo) {
                m_Ammo--;
                m_LastShotAt = CurrentTick;
                SoundHandler->PlaySound(ms_ShootSound);

                double VelocityX = ShooterCore->m_xlook * m_ProjectileSpeed;
                double VelocityY = ShooterCore->m_ylook * m_ProjectileSpeed;
                new Bullets(World, m_Shooter, WEAPON_BURST, m_Damage, ShooterCore->m_x, ShooterCore->m_y, VelocityX, VelocityY);

                double RecoilX = ShooterCore->m_xlook * -m_RecoilForce;
                double RecoilY = ShooterCore->m_ylook * -m_RecoilForce;
                m_Shooter->Accelerate(RecoilX, RecoilY);
            } else {
                SoundHandler->PlaySound(ms_ClickSound);
            }
        }
    }
}
