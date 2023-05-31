//
// Created by 11dpjgailis on 16.03.2023.
//
#include "Character.h"
#include <cmath>
#include <iostream>
#include "../Bullets.h"
#include <vector>

#ifndef M_SQRT1_2
#define M_SQRT1_2 0.70710678118654752440
#endif

CharacterInput::CharacterInput() {
    m_Shooting = false;
    m_Reloading = false;
    m_Hooking = false;
    m_Sneaking = false;
    m_NextItem = false;
    m_PrevItem = false;
    m_DeselectItem = false;
    m_GoingX = 0.0;
    m_GoingY = 0.0;
    m_GoingLength = 0.0;
    m_LookingX = 1.0;
    m_LookingY = 0.0;
    m_LookingLength = 0.0;
}
Texture* Character::ms_TextureErrorDisorianted = nullptr;
Texture* Character::ms_TextureErrorSpiky = nullptr;
Texture* Character::ms_TextureErrorConfusingHP = nullptr;
Texture* Character::ms_TextureErrorInvincible = nullptr;
Texture* Character::ms_TextureErrorHealersParadise = nullptr;
Texture* Character::ms_TextureErrorRanged = nullptr;
Texture* Character::ms_TextureErrorSlowDown = nullptr;
Texture* Character::ms_TextureErrorDangerousRecoil = nullptr;
Texture* Character::ms_TextureError = nullptr;
Texture* Character::ms_TextureGlock = nullptr;
Texture* Character::ms_TextureShotgun = nullptr;
Texture* Character::ms_TextureBurst = nullptr;
Texture* Character::ms_TexturesMinigun[2] = { nullptr, nullptr };


Texture* Character::ms_Texture = nullptr;
Sound* Character::ms_HitSounds[3] = { nullptr, nullptr, nullptr };
Sound* Character::ms_DeathSound = nullptr;
Sound* Character::ms_AmmoPickupSound = nullptr;
TextSurface* Character::ms_BotNamePlate = nullptr;
// TODO: see if we can make a little system that tells us if the textures/sounds are unitialized

const int Character::ms_DefaultControls[NUM_CONTROLS] = {SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_LSHIFT };


Character::Character(GameWorld* world, Player* player, double max_health,
                     double start_x, double start_y, double start_xvel, double start_yvel)
 : LookingEntity(world, GameWorld::ENTTYPE_CHARACTER, start_x, start_y, 50, 50, start_xvel, start_yvel, 1.0, 0.0, 0.93),
   m_BaseAcceleration(0.45),
   m_Hands(this, 40.0, 10.0, 10.0),
   m_Hook(this),
   m_HealthBar(world->GameWindow(), &m_Health, &m_MaxHealth, 75, 15, 2, 2),
   m_Input(),
   m_LastInput() {
    m_Player = player;
    if (m_Player) m_Player->SetCharacter(this);

    // Initialises all timers as 0
    m_IsReverseTimer = 0;
    m_ConfusingHPTimer = 0;
    m_InvincibleTimer = 0;
    m_SpikyTimer = 0;
    m_HealersParadiseTimer = 0;
    m_RangedTimer= 0;

    m_ColorHue = double(rand()%360);

    // Initialises all ERROR abilities to be false
    IsReversed = false;
    ConfusingHP = false;
    Ranged = false;
    Invincible = false;
    Spiky = false;
    HealersParadise = false;
    Ranged = false;
    IsSlow = false;
    DangerousRecoil = false;

    // Initialises all MSG drawing of ERRORS to false
    ReverseMSG = false;
    ConfusingHPMSG = false;
    InvincibleMSG = false;
    SpikyMSG = false;
    HealersMSG = false;
    RangedMSG = false;
    IsSlowMSG = false;
    RecoilMSG = false;

    // Yes yes, dont question my intelligence
    DrawErrorIsReversed = {-1000};
    DrawErrorConfusingHP = {-1000};
    DrawErrorInvincible = {-1000};
    DrawErrorSpiky = {-1000};
    DrawErrorHealersParadise = {-1000};
    DrawErrorRanged = {-1000};
    DrawErrorIsSlow = {-1000};
    DrawErrorDangerousRecoil = {-1000};
    Displacement = 0; // Sets the base displacement to 0, so when an error gets picked up, the icon spawns in the lowest spot

    m_CurrentWeapon = nullptr; // Start by holding nothing
    memset(m_Weapons, 0, sizeof(m_Weapons));

    // But this is Latvia and we give the character free guns
    m_Weapons[WEAPON_GLOCK] = new WeaponGlock(this);
    m_Weapons[WEAPON_BURST] = new WeaponBurst(this);
    m_Weapons[WEAPON_SHOTGUN] = new WeaponShotgun(this);
    m_Weapons[WEAPON_MINIGUN] = new WeaponMinigun(this);

    m_MaxHealth = max_health;
    m_Health = m_MaxHealth;
    m_PassiveRegeneration = 0.01; // health per tick when in combat
    m_ActiveRegeneration = 0.1; // health per tick out of combat
    m_TicksOfCombatUntilRegeneration = 10 * 60; // seconds * 60ticks per second
    m_LastInCombat = 0;

    m_SelectedWeaponIndex = -1;
    m_GameController = nullptr;
    for (bool& State : m_Movement)
        State = false;

    m_NPC = false;

    TextManager* TextHandler = world->GameWindow()->Assets()->TextHandler();
    TTF_Font* Font = TextHandler->FirstFont();
    m_AmmoCount = new TextSurface(m_World->GameWindow()->Assets(),
                                  Font,
                                  "0", { 255, 255, 255 });
    char msg[32];
    std::snprintf(msg, sizeof(msg), "Spawned [%ix, %iy]", (int)start_x, (int)start_y);
    m_CoordinatePlate = new TextSurface(m_World->GameWindow()->Assets(),
                                        Font,
                                        msg, { 255, 255, 255 });

    m_HealthInt = new TextSurface(m_World->GameWindow()->Assets(),
                                  m_World->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                  "0", { 0, 0, 0 });
    m_HitTicks = 0;
    m_CharacterColor = { 255, 255, 255, 255 };
    m_HookColor = { 255, 255, 255, 255 };
    m_HealthbarColor = { 255, 255, 255, 255 };
    m_HandColor = { 255, 255, 255, 255 };
    m_NameplateColor = { 255, 255, 255, 255 };
    m_HealthRed = {255, 0, 0, 255};
    m_HealthBlack = {0,0,0,255};
}

Character::~Character() {
    delete m_CoordinatePlate;

    Character* Char = m_World->FirstPlayer();
    for (; Char; Char = (Character*)Char->NextType()) {
        Hook* TargetHook = Char->GetHook();
        if (TargetHook->m_GrabbedEntity == this)
            TargetHook->Unhook();
    }
}

// Set the controller for this character,
// use `nullptr` for no controller
void Character::SetGameController(GameController* gameController) {
    m_GameController = gameController;
}

void Character::Damage(double damage, bool combat_tag) {
    if(!Invincible) {
        if(HealersParadise)m_Health += damage;
        else m_Health -= damage;
        m_HitTicks = 7;

        Sound *HurtSound = ms_HitSounds[rand() % 3];
        m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(HurtSound);
    }
    if (combat_tag) m_LastInCombat = m_World->CurrentTick();
}

void Character::ReverseMovement() {
    IsReversed = true;
    ReverseMSG = true; // In addition to setting reversed to true itself, also sets MSG to be drawn
    m_IsReverseTimer = 1000;
}

void Character::ConfuseHP() {
    ConfusingHP = true;
    ConfusingHPMSG = true;
    m_ConfusingHPTimer = 1000;
}

void Character::MakeInvincible() {
    Invincible = true;
    InvincibleMSG = true;
    m_InvincibleTimer = 500;
}

void Character::MakeSpiky() {
    Spiky = true;
    SpikyMSG = true;
    m_SpikyTimer = 500;
}

void Character::MakeHealer(){
    HealersParadise = true;
    HealersMSG = true;
    m_HealersParadiseTimer = 1000;
}

void Character::MakeRanged(){
    Ranged = true;
    RangedMSG = true;
    m_RangedTimer = 166;
}

void Character::SlowDown(){
    IsSlow = true;
    IsSlowMSG= true;
    m_IsSlowTimer = 1000;
}

void Character::ActivateDangerousRecoil(){
    DangerousRecoil = true;
    RecoilMSG = true;
    m_DangerousRecoilTimer = 1000;
}

void Character::TickTimer(){
    if(HealersParadise)m_HealersParadiseTimer -= 1;
    if(Spiky) m_SpikyTimer -= 1;
    if(Invincible) m_InvincibleTimer -= 1;
    if(ConfusingHP) m_ConfusingHPTimer -= 1;
    if(IsReversed) m_IsReverseTimer -= 1;
    if(Ranged) m_RangedTimer -= 1;
    if(IsSlow) m_IsSlowTimer -= 1;
    if(DangerousRecoil) m_DangerousRecoilTimer -=1;
    if((HealersParadise)||(Spiky)||(Invincible)||(ConfusingHP)||(IsReversed)||(Ranged)||(IsSlow)||(DangerousRecoil)) {
        if(m_IsReverseTimer <= 880)ReverseMSG = false; // Changes the MSG drawing to false if its above 2 seconds
                                                        // of active time
        if (m_IsReverseTimer <= 0 && IsReversed){
            IsReversed = false;
            Displacement = DrawErrorIsReversed.y;
            DrawErrorIsReversed = {-1000};

        }
        if(m_ConfusingHPTimer <= 880) ConfusingHPMSG = false;
        if (m_ConfusingHPTimer <= 0 && ConfusingHP){
            ConfusingHP = false;
            Displacement = DrawErrorConfusingHP.y; // Sets it so the next ERROR icon will be shown where the last one ended
            DrawErrorConfusingHP = {-1000};

        }
        if(m_InvincibleTimer <= 380) InvincibleMSG = false;
        if (m_InvincibleTimer <= 0 && Invincible){
            Invincible = false;
            Displacement = DrawErrorInvincible.y;
            DrawErrorInvincible = {-1000};

        }
        if(m_SpikyTimer <= 380) SpikyMSG = false;
        if (m_SpikyTimer <= 0 && Spiky){
            Spiky = false;
            Displacement = DrawErrorSpiky.y;
            DrawErrorSpiky = {-1000};

        }
        if(m_HealersParadiseTimer  <= 880) HealersMSG = false;
        if (m_HealersParadiseTimer <= 0 && HealersParadise) {
            HealersParadise = false;
            Displacement = DrawErrorHealersParadise.y;
            DrawErrorHealersParadise = {-1000};
        }
        if(m_RangedTimer <= 49) RangedMSG = false;
        if(m_RangedTimer <= 0 && Ranged) {
            Ranged = false;
            Displacement = DrawErrorRanged.y;
            DrawErrorRanged = {-1000};
        }
        if(m_IsSlowTimer <= 880) IsSlowMSG = false;
        if(m_IsSlowTimer <= 0 && IsSlow) {
            IsSlow = false;
            Displacement = DrawErrorIsSlow.y;
            DrawErrorIsSlow = {-1000};

        }
        if(m_DangerousRecoilTimer <= 880) RecoilMSG = false;
        if(m_DangerousRecoilTimer <= 0 && DangerousRecoil) {
            DangerousRecoil = false;
            Displacement = DrawErrorDangerousRecoil.y;
            DrawErrorDangerousRecoil = {-1000};
        }
    }
    else Displacement = 0;
}
void Character::SwitchWeapon(WeaponType type) {
    if (!m_Weapons[type] ||
        m_CurrentWeapon == m_Weapons[type]) {
        m_CurrentWeapon = nullptr;
    } else {
        m_CurrentWeapon = m_Weapons[type];
        m_AmmoCount->FlagForUpdate();
    }
}

void Character::AmmoPickup(Ammo* ammo_box){
    WeaponType ReloadWeapon;
    if (ammo_box->Type() == AMMO_GLOCK) ReloadWeapon = WEAPON_GLOCK;
    else if (ammo_box->Type() == AMMO_SHOTGUN) ReloadWeapon = WEAPON_SHOTGUN;
    else if (ammo_box->Type() == AMMO_BURST) ReloadWeapon = WEAPON_BURST;
    else if (ammo_box->Type() == AMMO_MINIGUN) ReloadWeapon = WEAPON_MINIGUN;
    else return; // Ammo type has no matching gun type

    if (!m_Weapons[ReloadWeapon]) return;

    auto AmmoNeeded = m_Weapons[ReloadWeapon]->NeededAmmo();
    auto TakenAmmo = ammo_box->TakeAmmo(AmmoNeeded);
    m_Weapons[ReloadWeapon]->AddTrueAmmo(TakenAmmo);
    if(TakenAmmo > 0){
        m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(ms_AmmoPickupSound);
    }

    if (m_CurrentWeapon == m_Weapons[ReloadWeapon]) m_AmmoCount->FlagForUpdate();
}

void Character::TickKeyboardControls() { // TODO: move to characterInput class
    bool Horizontal = m_Movement[CONTROL_LEFT] ^ m_Movement[CONTROL_RIGHT];
    bool Vertical = m_Movement[CONTROL_UP] != m_Movement[CONTROL_DOWN];
    double Unit = Horizontal && Vertical ? M_SQRT1_2 : 1.0;

    if (Horizontal || Vertical)
        m_Input.m_GoingLength = 1.0;

    if (Horizontal) {
        if (m_Movement[CONTROL_LEFT]) m_Input.m_GoingX = -Unit;
        else m_Input.m_GoingX = Unit;
    } else {
        m_Input.m_GoingX = 0.0;
    }

    if (Vertical) {
        if (m_Movement[CONTROL_UP]) m_Input.m_GoingY = -Unit;
        else m_Input.m_GoingY = Unit;
    } else {
        m_Input.m_GoingY = 0.0;
    }

    // RequestUpdate look direction
    int XMouse, YMouse;
    SDL_GetMouseState(&XMouse, &YMouse);

    m_Input.m_LookingX = m_World->CameraX() - m_Core->m_x + XMouse - m_World->GameWindow()->Width() / 2.0;
    m_Input.m_LookingY = m_World->CameraY() - m_Core->m_y + YMouse - m_World->GameWindow()->Height() / 2.0;
    m_Input.m_LookingLength = std::sqrt(std::pow(m_Input.m_LookingX, 2) + std::pow(m_Input.m_LookingY, 2));

    if (m_Input.m_LookingLength != 0.0) {
        m_Input.m_LookingX /= m_Input.m_LookingLength;
        m_Input.m_LookingY /= m_Input.m_LookingLength;
    } else {
        m_Input.m_LookingX = 1.0;
        m_Input.m_LookingY = 0.0;
    }

    auto MouseState = SDL_GetMouseState(nullptr, nullptr);
    m_Input.m_Shooting = MouseState & SDL_BUTTON(SDL_BUTTON_LEFT);  // If clicked, shoot = true
    m_Input.m_Hooking = MouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
    m_Input.m_Sneaking = m_Movement[CONTROL_SHIFT];
}

void Character::TickGameControllerControls() {
    // Sneaking
    m_Input.m_Sneaking = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_LEFTSTICK);

    // Check for current joystick values
    m_GameController->GetJoystick1(m_Input.m_GoingX, m_Input.m_GoingY);

    // AxisX**2 + AxisY**2 <= 1 (keep direction length of 1)
    m_Input.m_GoingLength = std::sqrt(std::pow(m_Input.m_GoingX, 2) + std::pow(m_Input.m_GoingY, 2));
    m_Input.m_GoingX /= m_Input.m_GoingLength;
    m_Input.m_GoingY /= m_Input.m_GoingLength;

    // RequestUpdate look direction
    double LookingX, LookingY;
    m_GameController->GetJoystick2(LookingX, LookingY);

    m_Input.m_LookingLength = std::sqrt(std::pow(LookingX, 2) + std::pow(LookingY, 2));
    if (m_Input.m_LookingLength >= 0.6) {
        m_Input.m_LookingX = LookingX / m_Input.m_LookingLength;
        m_Input.m_LookingY = LookingY / m_Input.m_LookingLength;
    }

    // Shooting
    m_Input.m_Shooting = m_GameController->GetRightTrigger() > 0.7;
    m_Input.m_Hooking = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    m_Input.m_Reloading = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_X);

    // Switch weapons
    m_Input.m_NextItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    m_Input.m_PrevItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT) && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    m_Input.m_DeselectItem = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_UP) && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN);
}

// When in combat heal differently than out of combat
void Character::TickHealth() {
    auto CurrentTick = m_World->CurrentTick();
    bool ActiveRegeneration = CurrentTick - m_LastInCombat < m_TicksOfCombatUntilRegeneration;
    m_Health += ActiveRegeneration ? m_PassiveRegeneration : m_ActiveRegeneration;

    if (m_Health > m_MaxHealth)
        m_Health = m_MaxHealth;
}

void Character::TickControls() {
    if (m_GameController) TickGameControllerControls();
    else TickKeyboardControls();

    if (m_Input.m_LookingLength <= 0.6 && m_Input.m_GoingLength > 0.2) {
        m_Input.m_LookingX = m_Input.m_GoingX;
        m_Input.m_LookingY = m_Input.m_GoingY;
    }
}

void Character::ProcessControls() {
    if (m_Input.m_GoingLength > 0.2) {  // Fix controller drifting
        // Checks if player is shifting (holding left stick)
        // TODO: bool Shifting = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_LEFTSTICK);

        m_Acceleration = (m_Input.m_Sneaking ? m_BaseAcceleration/3 : m_BaseAcceleration) * (IsReversed ? -1 : 1) * (IsSlow ? 0.25 : 1) * (bool(m_CurrentWeapon) ? 0.75 : 1.0);

        // Accelerate in that direction
        m_Core->m_xvel += m_Input.m_GoingX * m_Acceleration;
        m_Core->m_yvel += m_Input.m_GoingY * m_Acceleration;
    }

    if (m_Input.m_LookingLength > 0.6) {
        m_LookingCore->m_xlook = m_Input.m_LookingX;
        m_LookingCore->m_ylook = m_Input.m_LookingY;
    }

    if (m_Input.m_NextItem ^ m_Input.m_PrevItem ^ m_Input.m_DeselectItem) { // I hope this works as intended, only 1 at a time | ignore if multiple inputs at the same time
        if (m_Input.m_DeselectItem) { m_SelectedWeaponIndex = -1; }
        else if (m_Input.m_NextItem) {
            m_SelectedWeaponIndex++;
            if (m_SelectedWeaponIndex == NUM_WEAPONS)
                m_SelectedWeaponIndex = -1;
        } else {
            m_SelectedWeaponIndex--;
            if (m_SelectedWeaponIndex == -2)
                m_SelectedWeaponIndex = NUM_WEAPONS - 1;
        }

        if (m_SelectedWeaponIndex == -1) { m_CurrentWeapon = nullptr; }
        else { SwitchWeapon((WeaponType)m_SelectedWeaponIndex); } // yeaaaaaaa
    }
}

void Character::TickHook() {
    m_Hook.Tick();
}

void Character::TickCollision() {
    auto Char = m_World->FirstPlayer();
    for (; Char; Char = (Character*)(Char->NextType())) {
        if (Char == this) continue;

        EntityCore* EntCore = Char->GetCore();
        double XDistance = m_Core->m_x - EntCore->m_x;
        double YDistance = m_Core->m_y - EntCore->m_y;
        double Distance = std::sqrt(std::pow(XDistance, 2) + std::pow(YDistance, 2));

        if (Distance > 40) continue;
        else if (Distance == 0.0) {
            double Radians = (rand()%360) / 180.0 * M_PI;
            XDistance = cos(Radians);
            YDistance = sin(Radians);
            Distance = 1.0;
        }

        // TODO make push stronger when closer to character not when further....
        double XPush = XDistance / Distance * 0.5;
        double YPush = YDistance / Distance * 0.5;
        m_Core->Accelerate(XPush, YPush);
        EntCore->Accelerate(-XPush, -YPush);
        if(Spiky) Char->Damage(3, true);
    }
    auto Crate = m_World->FirstCrate();
    for (; Crate; Crate = (Crates*)(Crate->NextType())) {
        EntityCore* CrateCore = Crate->GetCore();
        double XDistance = m_Core->m_x - CrateCore->m_x;
        double YDistance = m_Core->m_y - CrateCore->m_y;
        double Distance = std::sqrt(std::pow(XDistance, 2) + std::pow(YDistance, 2));

        if (Distance > 40) continue;
        else if (Distance < 0.0) {
            double Radians = (rand()%360) / 180.0 * M_PI;
            XDistance = cos(Radians);
            YDistance = sin(Radians);
            Distance = 1.0;
        }
        double XPush = XDistance / Distance* 2;
        double YPush = YDistance / Distance* 2;
        Accelerate(XPush, YPush);
}
}

void Character::TickCurrentWeapon() {
    if (m_CurrentWeapon) {
        auto TempAmmo = m_CurrentWeapon->Ammo();
        if (m_Input.m_Reloading && !m_LastInput.m_Reloading)
            m_CurrentWeapon->Reload();

        m_CurrentWeapon->Tick();
        auto CurrentAmmo = m_CurrentWeapon->Ammo();
        if (TempAmmo != CurrentAmmo) {
            m_AmmoCount->FlagForUpdate();
            if (!CurrentAmmo && TempAmmo) { m_AmmoCount->SetColor({ 255, 0, 0 }); }
            else { m_AmmoCount->SetColor( { 255, 255, 255 } ); }
        }
    }
}
// Function to draw icons for error pickup
void Character::DrawErrorIcons(){
    SDL_FRect DrawRectError = {float(m_Core->m_x) - float(m_Core->m_w / 2.0) + 50,
                     float(m_Core->m_y) - float(m_Core->m_h / 2.0) + 50,
                     float(20),
                     float(20)};

    Drawing* Render = m_World->GameWindow()->RenderClass();
    // Goes through all active ERRORS
    if(IsReversed){
        if(DrawErrorIsReversed.x == -1000){ // If is the first time drawing it
            Render->RenderTextureFWorld(ms_TextureErrorDisorianted->SDLTexture(), nullptr, DrawRectError);
            DrawErrorIsReversed = DrawRectError; // Need this so the .x value isnt -1000 after this
            DrawErrorIsReversed.y = Displacement; // Saves the current displacement value in the .y position
            Displacement -= 20; // Changes the displacement by -20 so the next one spawns above it
        }
        else{// When its not the first time, but repeat drawing of the same instance of ERROR
            DrawRectError.y += DrawErrorIsReversed.y;  // Changes the y by the displacement when picked up
            Render->RenderTextureFWorld(ms_TextureErrorDisorianted->SDLTexture(), nullptr, DrawRectError);
            DrawRectError.y -= DrawErrorIsReversed.y; // Changes it back
            //Then i do that for EVERY SINGLE ERROR!!
        }
    }
    if(ConfusingHP){
        if(DrawErrorConfusingHP.x == -1000){
            Render->RenderTextureFWorld(ms_TextureErrorConfusingHP->SDLTexture(), nullptr, DrawRectError);
            DrawErrorConfusingHP = DrawRectError;
            DrawErrorConfusingHP.y = Displacement;
            Displacement -= 20;
        }
        else {
            DrawRectError.y += DrawErrorConfusingHP.y;
            Render->RenderTextureFWorld(ms_TextureErrorConfusingHP->SDLTexture(), nullptr, DrawRectError);
            DrawRectError.y -= DrawErrorConfusingHP.y;
        }
    }
    if(Invincible) {
        if (DrawErrorInvincible.x == -1000){
            Render->RenderTextureFWorld(ms_TextureErrorInvincible->SDLTexture(), nullptr, DrawRectError);
            DrawErrorInvincible = DrawRectError;
            DrawErrorInvincible.y = Displacement;
            Displacement -= 20;
        }
        else {
            DrawRectError.y += DrawErrorInvincible.y;
            Render->RenderTextureFWorld(ms_TextureErrorInvincible->SDLTexture(), nullptr, DrawRectError);
            DrawRectError.y -= DrawErrorInvincible.y;
        }
    }

    if(Spiky) {
        if (DrawErrorSpiky.x == -1000) {
            Render->RenderTextureFWorld(ms_TextureErrorSpiky->SDLTexture(), nullptr, DrawRectError);
            DrawErrorSpiky = DrawRectError;
            DrawErrorSpiky.y = Displacement;
            Displacement -= 20;
        }
        else {
            DrawRectError.y += DrawErrorSpiky.y;
            Render->RenderTextureFWorld(ms_TextureErrorSpiky->SDLTexture(), nullptr, DrawRectError);
            DrawRectError.y -= DrawErrorSpiky.y;
        }
    }

    if(HealersParadise){
        if(DrawErrorHealersParadise.x == -1000) {
            Render->RenderTextureFWorld(ms_TextureErrorHealersParadise->SDLTexture(), nullptr, DrawRectError);
            DrawErrorHealersParadise = DrawRectError;
            DrawErrorHealersParadise.y = Displacement;
            Displacement -= 20;
        }
        else {
            DrawRectError.y += DrawErrorHealersParadise.y;
            Render->RenderTextureFWorld(ms_TextureErrorHealersParadise->SDLTexture(), nullptr, DrawRectError);
            DrawRectError.y -= DrawErrorHealersParadise.y;
        }
    }
    if(Ranged){
        if(DrawErrorRanged.x == -1000){
            Render->RenderTextureFWorld(ms_TextureErrorRanged->SDLTexture(), nullptr, DrawRectError);
            DrawErrorRanged = DrawRectError;
            DrawErrorRanged.y = Displacement;
            Displacement -= 20;
        }
        else {
            DrawRectError.y += DrawErrorRanged.y;
            Render->RenderTextureFWorld(ms_TextureErrorRanged->SDLTexture(), nullptr, DrawRectError);
            DrawRectError.y -= DrawErrorRanged.y;
        }
    }
    if(DangerousRecoil){
        if(DrawErrorDangerousRecoil.x == -1000){
            Render->RenderTextureFWorld(ms_TextureError->SDLTexture(), nullptr, DrawRectError);
            DrawErrorDangerousRecoil = DrawRectError;
            DrawErrorDangerousRecoil.y = Displacement;
            Displacement -= 20;
        }
        else {
            DrawRectError.y +=DrawErrorDangerousRecoil.y;
            Render->RenderTextureFWorld(ms_TextureError->SDLTexture(), nullptr, DrawRectError);
            DrawRectError.y -= DrawErrorDangerousRecoil.y;
        }
    }
    if(IsSlow){
        if(DrawErrorIsSlow.x == -1000){
            Render->RenderTextureFWorld(ms_TextureErrorSlowDown->SDLTexture(), nullptr, DrawRectError);
            DrawErrorIsSlow = DrawRectError;
            DrawErrorIsSlow.y = Displacement;
            Displacement -= 20;
        }
        else {
            DrawRectError.y += DrawErrorIsSlow.y;
            Render->RenderTextureFWorld(ms_TextureErrorSlowDown->SDLTexture(), nullptr, DrawRectError);
            DrawRectError.y -= DrawErrorIsSlow.y;
        }
    }
}

void Character::DrawCharacter() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_FRect DrawRect = {float(m_Core->m_x) - float(m_Core->m_w / 2.0),
                          float(m_Core->m_y) - float(m_Core->m_h / 2.0),
                          float(m_Core->m_w),
                          float(m_Core->m_h)};

   // if(m_HitTicks > 0) Render->SetColor(255, 0, 0, 255);
   // else { Render->SetColor(m_CharacterColor.r, m_CharacterColor.g, m_CharacterColor.b, 255); }
    // Render->FillRectFWorld(DrawRect);
    // Render->RenderTextureFWorld(ms_Texture->SDLTexture(), nullptr,DrawRect);
    ms_Texture->SetColorMod(m_CharacterColor.r, m_CharacterColor.g, m_CharacterColor.b);

    double Angle = std::atan2(m_Core->m_yvel, m_Core->m_xvel) / M_PI * 180.0;
    SDL_RendererFlip flip = Angle > 90 || Angle < -90 ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE;
    Render->RenderTextureExFWorld(ms_Texture->SDLTexture(), nullptr, DrawRect, Angle, nullptr, flip);
}

void Character::DrawHook() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    // Draw hook
    if (m_Hook.m_Deployed) {
        Render->SetColor(m_HookColor.r, m_HookColor.g, m_HookColor.b, 255);
        Render->LineWorld(m_Core->m_x, m_Core->m_y, m_Hook.m_x, m_Hook.m_y);
    }
}


void Character::DrawHealthbar() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    // Render health bar
    if (m_Health != m_MaxHealth) {
        m_HealthBar.SetColor(m_HealthbarColor.r, m_HealthbarColor.g, m_HealthbarColor.b, m_HealthbarColor.a);
        Texture *HealthPlate;
        if(!ConfusingHP) {
            HealthPlate = m_HealthBar.UpdateTexture();
        }
        else HealthPlate = m_HealthBar.GetTexture();

        int healthplate_w, healthplate_h;
        HealthPlate->Query(nullptr, nullptr, &healthplate_w, &healthplate_h);
        SDL_Rect HealthplateRect = { int(m_Core->m_x - healthplate_w / 2.0), int(m_Core->m_y + m_Core->m_h / 2.0), healthplate_w, healthplate_h };
        char msg[64];
        if(!ConfusingHP){
        std::snprintf(msg, sizeof(msg), "%i/%i", int(m_Health), int(m_MaxHealth));
        }
        else std::snprintf(msg, sizeof(msg), "%i/%i", int(rand()%999), int(rand()%999));
        m_HealthInt->SetText(msg);

        if(m_Health < 50) m_HealthInt->SetColor(m_HealthRed);
        else m_HealthInt->SetColor(m_HealthBlack);
        Texture* HealthTexture = m_HealthInt->RequestUpdate();
        HealthTexture->Query(nullptr, nullptr, &healthplate_w, &healthplate_h);
        SDL_Rect HealthIntRect = { int(m_Core->m_x - healthplate_w/2 / 2.0) ,int(m_Core->m_y + m_Core->m_h/2+healthplate_h/4), healthplate_w/2, healthplate_h/2 };

        Render->RenderTextureWorld(HealthPlate->SDLTexture(), nullptr, HealthplateRect);
        Render->RenderTextureWorld(HealthTexture->SDLTexture(), nullptr, HealthIntRect);
    }
}

void Character::DrawHands() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

     double XLook = m_Input.m_LookingX * 15.0;
     double YLook = m_Input.m_LookingY * 15.0;

    ProjectileWeapon* CurrentWeapon = m_CurrentWeapon;
    if (m_CurrentWeapon) {
        Texture* WeaponTexture;
        switch (m_CurrentWeapon->Type()) {
            case WEAPON_GLOCK: {
                WeaponTexture = ms_TextureGlock;
            } break;
            case WEAPON_BURST: {
                WeaponTexture = ms_TextureBurst;
            } break;
            case WEAPON_SHOTGUN: {
                WeaponTexture = ms_TextureShotgun;
            } break;
            case WEAPON_MINIGUN: {
                WeaponTexture = ms_TexturesMinigun[0];
            } break;
        }

        SDL_Rect WeaponRect;
        WeaponTexture->Query(nullptr, nullptr, &WeaponRect.w, &WeaponRect.h);
        WeaponRect.w *= 4;
        WeaponRect.h *= 4;
        WeaponRect.x = int(XLook + m_Core->m_x - float(WeaponRect.w) / 2.0);
        WeaponRect.y = int(YLook + m_Core->m_y);
        SDL_Point WeaponPivot = { int(float(WeaponRect.w) / 2.0), 0 };

        double Angle = std::atan2(m_Input.m_LookingY * 50.0, m_Input.m_LookingX * 50.0) / M_PI * 180.0;
        // TODO Seperate this into gun classes id say and give gun class a different texture and make bullets spawn from the gun
        // and not the center of the player
        Render->RenderTextureExWorld(WeaponTexture->SDLTexture(), nullptr, WeaponRect, Angle - 90, &WeaponPivot, SDL_FLIP_VERTICAL);
    }

    m_Hands.SetColor(m_HandColor);
    m_Hands.Draw();
}

void Character::DrawNameplate() {
    if (m_World->NamesShown() <= 0.05)  // Visibility under 5% - don't render the texts
        return;

    Drawing* Render = m_World->GameWindow()->RenderClass();

    int Opacity = int(m_World->NamesShown() * 255.0);

    Texture* NamePlateTexture;
    if (m_Player) { NamePlateTexture = m_Player->GetNamePlate()->RequestUpdate(); }
    else { NamePlateTexture = ms_BotNamePlate->GetTexture(); }

    int nameplate_w, nameplate_h;
    NamePlateTexture->Query(nullptr, nullptr, &nameplate_w, &nameplate_h);
    SDL_Rect NamePlateRect = { int(m_Core->m_x - nameplate_w / 2.0), int(m_Core->m_y - m_Core->m_h / 2.0 - nameplate_h),
                      nameplate_w, nameplate_h };

    SDL_SetTextureAlphaMod(NamePlateTexture->SDLTexture(), Opacity);
    Render->RenderTextureWorld(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect);


    char msg[64];
    std::snprintf(msg, sizeof(msg), "%ix, %iy", int(m_Core->m_x), int(m_Core->m_y));
    m_CoordinatePlate->SetText(msg);
    m_CoordinatePlate->SetColor(m_NameplateColor);
    Texture* CoordinateTexture = m_CoordinatePlate->RequestUpdate();

    int coordinate_w, coordinate_h;
    CoordinateTexture->Query(nullptr, nullptr, &coordinate_w, &coordinate_h);
    SDL_Rect CoordinateRect = { int(m_Core->m_x - coordinate_w / 2.0), int(NamePlateRect.y - coordinate_h), coordinate_w, coordinate_h };
    SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), Opacity);
    Render->RenderTextureWorld(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect);
}

// TODO when switching guns ammo text renders again, to prevent this save each ammo count texture on the gun
void Character::DrawAmmo(){
    Drawing* Render = m_World->GameWindow()->RenderClass();
    char msg[64];
    std::snprintf(msg, sizeof(msg), "%u/%u", m_CurrentWeapon->Ammo(), m_CurrentWeapon->TrueAmmo());
    m_AmmoCount->SetText(msg);
    if (m_CurrentWeapon->Ammo() == 0) m_AmmoCount->SetColor( {255, 0, 0} );
    else { m_AmmoCount->SetColor( {255, 255, 255} ); }
    Texture* AmmoTexture = m_AmmoCount->RequestUpdate();

    int Ammo_w, Ammo_h;
    AmmoTexture->Query(nullptr, nullptr, &Ammo_w, &Ammo_h);
    SDL_Rect AmmoRect = { int(m_Core->m_x - Ammo_w / 2.0) ,int(m_Core->m_y + m_Core->m_h/2+20), Ammo_w, Ammo_h };
    Render->RenderTextureWorld(AmmoTexture->SDLTexture(), nullptr, AmmoRect);
}


void Character::DrawErrorName() {
    Drawing* Render = m_World->GameWindow()->RenderClass();
    char msg[64];
    // Changes the "msg" to whatever Error has been picked up( not else if's cuz then it wouldnt update on new pickup
    // aka, this is so it overrides the last msg too)
    if(ReverseMSG)std::snprintf(msg, sizeof(msg), "ERROR activated \"Reverse Movement\"");
    if(ConfusingHPMSG) std::snprintf(msg, sizeof(msg), "ERROR activated \"Confusing HP\"");
    if(InvincibleMSG) std::snprintf(msg, sizeof(msg), "ERROR activated \"Invincible\"");
    if(SpikyMSG)std::snprintf(msg, sizeof(msg), "ERROR activated \"Spiky\"");
    if(HealersMSG)std::snprintf(msg, sizeof(msg), "ERROR activated \"Healers paradise\"");
    if(RangedMSG) std::snprintf(msg, sizeof(msg), "ERROR activated \"Ranged\"");
    if (IsSlowMSG)std::snprintf(msg, sizeof(msg), "ERROR activated \"Slow down\"");
    if (RecoilMSG)std::snprintf(msg, sizeof(msg), "ERROR activated \"Dangerous recoil\"");

    m_ErrorText = new TextSurface(m_World->GameWindow()->Assets(),
                                  m_World->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                  msg, { 0, 0, 0 });
    m_ErrorText->SetText(msg);
    Texture* ErrorTexture = m_ErrorText->RequestUpdate();
    int Text_h = 5;
    int Text_w = 1000;
    ErrorTexture->Query(nullptr, nullptr, &Text_h, &Text_w);
    // Draws it in the top middle of the screen
    SDL_Rect ErrorRect = { int(m_World->CameraX() -100), int(m_World->CameraY()-200), Text_h, Text_w};
    Render->RenderTextureWorld(ErrorTexture->SDLTexture(), nullptr, ErrorRect);
}

void Character::Event(const SDL_Event& currentEvent) {
    if (m_NPC || m_GameController)
        return;

    if (currentEvent.type == SDL_KEYDOWN && currentEvent.key.repeat == 0 ||
        currentEvent.type == SDL_KEYUP) { // TODO: Put this into CharacterInput struct
        bool State = currentEvent.type == SDL_KEYDOWN;
        if (State) {
            int KeyCode = currentEvent.key.keysym.scancode;
            if (KeyCode == SDL_SCANCODE_GRAVE) { m_CurrentWeapon = nullptr; }
            else if (KeyCode == SDL_SCANCODE_1) { SwitchWeapon(WEAPON_GLOCK); }
            else if (KeyCode == SDL_SCANCODE_2) { SwitchWeapon(WEAPON_SHOTGUN); }
            else if (KeyCode == SDL_SCANCODE_3) { SwitchWeapon(WEAPON_BURST); }
            else if (KeyCode == SDL_SCANCODE_4) { SwitchWeapon(WEAPON_MINIGUN); }
        }

        // Reloads weapon on keyboard player with R button press
        if (currentEvent.key.keysym.scancode == SDL_SCANCODE_R)
            m_Input.m_Reloading = State;

        for (int i = 0; i < NUM_CONTROLS; i++) {
            if (currentEvent.key.keysym.scancode == ms_DefaultControls[i])
                m_Movement[i] = State;
        }
    }
}

void Character::Tick() {
    // TODO: Make controls come from m_Player/m_AI rather than m_GameController
    TickHealth();
    TickControls(); // Parse the inputs of each device keyboard/controller/AI
    ProcessControls(); // Do stuff depending on the current held buttons
    TickHook();  // Move hook and or player etc.
    TickCurrentWeapon(); // Shoot accelerate reload etc.
    m_Hands.Tick();
    TickTimer(); // Ticks timer for errors
    // Need every character to get here..
    // then we apply the accelerations of all
    // hooks and continue with the code below v v v
    // like collisions and velocity tick

    TickCollision();
    TickVelocity();  // Move the character entity
    TickWalls();  // Check if colliding with walls

    if ((int)(m_Health) != (int)(m_LastHealth)) m_HealthInt->FlagForUpdate();
    if (m_World->NamesShown() > 0.05 &&
        ((int)(m_Core->m_x) != (int)(m_LastCore->m_x) ||
        (int)(m_Core->m_y) != (int)(m_LastCore->m_y)))
        m_CoordinatePlate->FlagForUpdate();

    m_LastHealth = m_Health;
    TickLastCore();
    memcpy(&m_LastInput, &m_Input, sizeof(CharacterInput));

    m_HitTicks -= 1;
    if (m_HitTicks < 0)
        m_HitTicks = 0;

    if (m_Health <= 0.0) {
        m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(ms_DeathSound);
        m_Alive = false;
    }
}

void Character::Draw() {
    auto CurrentTick = m_World->CurrentTick();
    double Light = 0.5 + (std::sin(double(CurrentTick - m_ExistsSince)/60.0) + 1.0) / 4.0;

    double Hue = m_HitTicks ? 0.0 : m_ColorHue;
    m_CharacterColor = HSLtoRGB({ Hue, Light, 1.0 });
    m_HookColor = HSLtoRGB({ Hue, 1.0, Light });
    m_HealthbarColor = m_CharacterColor;
    m_HandColor = HSLtoRGB({ Hue, 1.0 - Light, 1.0 });
    m_NameplateColor = m_HandColor;
    DrawHook();
    DrawHands();
    DrawCharacter();
    DrawHealthbar();
    DrawNameplate();
    DrawErrorIcons();
    if(m_CurrentWeapon) DrawAmmo();
    // Only draws the Error names, if the timers havent been going down for any more than 2 seconds
    // 1000(Most Error activity time)-120(2 seconds)
    if(m_IsReverseTimer>880 || m_ConfusingHPTimer>880 || m_InvincibleTimer>380 || m_SpikyTimer>380 ||
    m_HealersParadiseTimer>880 || m_RangedTimer>46)
        DrawErrorName();
}
