//
// Created by Matq on 30/03/2023.
//

#pragma once

#include "../../GameWorld.h"

enum WeaponType {
    WEAPON_GLOCK,
    WEAPON_SHOTGUN,
    WEAPON_BURST,
    WEAPON_MINIGUN,
    WEAPON_SNIPER,
    WEAPON_PATERSONNAVY,
    NUM_WEAPONS
};

class DirectionalEntity;
class ProjectileWeapon {
protected:
    unsigned long long m_LastShotAt;
    unsigned long long m_TickCooldown;
    unsigned int m_Ammo, m_AmmoCapacity;
    unsigned int m_TrueAmmo, m_TrueAmmoCapacity;
    double m_ProjectileSpeed;
    bool m_Automatic;
    bool m_Triggered;
    bool m_LastShot;

    double m_RandomSpreadDivisor;
    double m_HalfRandomSpread;
    int m_FullRandomSpread;

    double m_RandomProjectileSpeedDivisor;
    double m_NegativeRandomProjectileSpeed;
    int m_FullRandomProjectileSpeed;

    DirectionalEntity* m_Parent;
    WeaponType m_Type;
    Texture* m_Texture;
    Sound* m_ReloadSound;
    Vec2d m_HoldPosition;
    Vec2d m_LeftHandPosition;
    Vec2d m_RightHandPosition;

    // Ticking
    virtual void TickTrigger();

    // Generating
    [[nodiscard]] double GenerateSpreadAngle() const;
    [[nodiscard]] double GenerateRandomProjectileSpeed() const;

public:
    static LoadedSound sNoAmmoSound;

    ProjectileWeapon(DirectionalEntity* parent,
                     WeaponType type,
                     Texture* texture,
                     Sound* reload_sound,
                     Vec2d* hold_position,
                     std::pair<Vec2d, Vec2d>* hand_positions,
                     int tick_cooldown,
                     int ammo_capacity,
                     int total_ammo_capacity,
                     double projectile_speed,
                     bool automatic);

    // Getting
    [[nodiscard]] WeaponType WepType() const { return m_Type; }
    [[nodiscard]] Texture* GetTexture() const { return m_Texture; }
    [[nodiscard]] bool IsAutomatic() const { return m_Automatic; }
    [[nodiscard]] unsigned int GetMagAmmo() const { return m_Ammo; }
    [[nodiscard]] unsigned int GetTrueAmmo() const { return m_TrueAmmo; }
    [[nodiscard]] unsigned int GetFullAmmo() const { return m_TrueAmmo + m_Ammo; }
    [[nodiscard]] unsigned int AmmoCap() const { return m_AmmoCapacity; }
    [[nodiscard]] unsigned long long TickCooldown() const { return m_TickCooldown; }
    [[nodiscard]] unsigned long long LastShot() const { return m_LastShotAt; }
    [[nodiscard]] Vec2d GetHoldPosition() const { return m_HoldPosition; }
    [[nodiscard]] Vec2d GetLeftHand() const { return m_LeftHandPosition; }
    [[nodiscard]] Vec2d GetRightHand() const { return m_RightHandPosition; }

    // Generating
    [[nodiscard]] unsigned int NeededTrueAmmo() const;

    // Setting
    void SetParent(DirectionalEntity* parent);
    void SetSpread(double degrees, int decimal_places);
    void SetRandomProjectileSpeed(double delta_speed, double delta_percentage_negative, int delta_decimal_places);

    // Manipulating
    void SetTexture(Texture* texture) { m_Texture = texture; }
    unsigned int AddMagAmmo(unsigned int count);
    unsigned int AddTrueAmmo(unsigned int count);
    virtual void Reload();
    virtual void OnSelected();

    // Ticking
    virtual void Tick();
};
