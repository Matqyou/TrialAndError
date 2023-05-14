//
// Created by 11dpjgailis on 16.03.2023.
//
#include "Character.h"
#include <cmath>
#include <iostream>
#include "../Bullets.h"
#include <vector>

Texture* Character::ms_Texture = nullptr;
Texture* Character::ms_FistTexture = nullptr;
Sound* Character::ms_HitSounds[3] = { nullptr, nullptr, nullptr };
Sound* Character::ms_DeathSound = nullptr;
Sound* Character::ms_AmmoPickupSound = nullptr;
TextSurface* Character::ms_BotNamePlate = nullptr;
// TODO: see if we can make a little system that tells us if the textures/sounds are unitialized

static double sDiagonalLength = 1.0 / std::sqrt(2.0);
const int Character::ms_DefaultControls[NUM_CONTROLS] = {SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_LSHIFT };

Character::Character(GameWorld* world, Player* player, double max_health,
                     double start_x, double start_y, double start_xvel, double start_yvel,
                     bool bot_player)
 : Entity(world, GameWorld::ENTTYPE_CHARACTER, start_x, start_y, 50, 50, 0.93),
   m_HandSpacing(40.0 / 180.0 * M_PI),
   m_FistingAnimationDuration(10.0),
   m_BaseAcceleration(0.75),
   m_Hook(this),
   m_HealthBar(world->GameWindow(), &m_Health, &m_MaxHealth, 75, 15, 2, 2) {
    m_Player = player;
    if (m_Player) m_Player->SetCharacter(this);

    m_ColorHue = double(rand()%360);
    IsReversed = false;
    ConfusingHP = false;
    m_Using = false;
    m_LastFisted = 0;
    m_LastFistedL = 0;
    m_LastFistedR = 0;
    m_LastUsing = false;
    m_Reloading = false;
    m_LastReloading = false;

    m_CurrentWeapon = nullptr; // Start by holding nothing

    // Always set to null, cuz player should spawn without weapons
    memset(m_Weapons, 0, sizeof(m_Weapons));

    // But this is Latvia and we give the character free guns
    m_Weapons[WEAPON_GLOCK] = new WeaponGlock(this);
    m_Weapons[WEAPON_BURST] = new WeaponBurst(this);
    m_Weapons[WEAPON_SHOTGUN] = new WeaponShotgun(this);
    m_Weapons[WEAPON_MINIGUN] = new WeaponMinigun(this);

    m_FistingRadius = 10.0;
    m_MaxHealth = max_health;
    m_Health = m_MaxHealth;
    m_PassiveRegeneration = 0.01; // health per tick when in combat
    m_ActiveRegeneration = 0.1; // health per tick out of combat
    m_RegenerationCooldown = 10 * 60; // seconds * 60ticks per second
    m_LastInCombat = 0;

    m_SelectedWeaponIndex = -1;
    m_GameController = nullptr;
    for (bool& State : m_Movement)
        State = false;

    m_Controllable = !bot_player;

    m_xLast = m_x;
    m_yLast = m_y;
    m_xvel = start_xvel;
    m_yvel = start_yvel;
    m_xLook = 1.0;
    m_yLook = 0.0;

    m_Hooking = false;
    m_LastHooking = false;

    TextManager* TextHandler = world->GameWindow()->Assets()->TextHandler();
    TTF_Font* Font = TextHandler->FirstFont();
    m_AmmoCount = new TextSurface(m_World->GameWindow()->Assets(),
                                  Font,
                                  "0", { 255, 255, 255 });
    char msg[32];
    std::snprintf(msg, sizeof(msg), "Spawned [%ix, %iy]", (int)m_x, (int)m_y);
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

    Character* Player = m_World->FirstPlayer();
    for (; Player; Player = (Character*)Player->NextType()) {
        Hook* TargetHook = Player->GetHook();
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
    if(IsReversed)IsReversed = false;
    else {
        IsReversed = true;
        m_Timer = 1000;
    }
}

void Character::ConfuseHP() {
    if(ConfusingHP)ConfusingHP = false;
    else {
        ConfusingHP = true;
        m_Timer = 1000;
    }
}

void Character::MakeInvincible() {
    if(Invincible)Invincible = false;
    else {
        Invincible = true;
        m_Timer = 500;
    }

}

void Character::MakeSpiky() {
    if(Spiky)Spiky = false;
    else {
        Spiky = true;
        m_Timer = 500;
    }
}

void Character::MakeHealer(){
    if(HealersParadise)HealersParadise = false;
    else {
        HealersParadise = true;
        m_Timer = 1000;
    }
}

void Character::TickTimer(){
    if((HealersParadise)||(Spiky)||(Invincible)||(ConfusingHP)||(IsReversed)) {
        m_Timer -= 1;
        if (m_Timer <= 0) {
            if (IsReversed)IsReversed = false;
            else if (ConfusingHP)ConfusingHP = false;
            else if (Invincible)Invincible = false;
            else if (Spiky)Spiky = false;
            else if (HealersParadise) HealersParadise = false;
        }
    }
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

void Character::SetPlayer(Player* player) {
    m_Player = player;
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

void Character::TickKeyboardControls() {
    // Check if buttons are held
    bool MoveUp = m_Movement[CONTROL_UP];
    bool MoveRight = m_Movement[CONTROL_RIGHT];
    bool MoveDown = m_Movement[CONTROL_DOWN];
    bool MoveLeft = m_Movement[CONTROL_LEFT];
    bool Shifting = m_Movement[CONTROL_SHIFT];

    bool Horizontally = MoveLeft != MoveRight;
    bool Vertically = MoveUp != MoveDown;

    // Depending on if shift is held, change accelaration value
    m_Acceleration = Shifting ? m_BaseAcceleration/3 : m_BaseAcceleration;

    // Accelerate when buttons are held
    double LengthPerAxis = (Horizontally && Vertically) ? sDiagonalLength : 1.0;
    double SpeedPerAxis = m_Acceleration * LengthPerAxis;

    if(!IsReversed) {
        if (MoveDown != MoveUp) m_yvel += SpeedPerAxis * double(MoveDown ? 1 : -1);
        if (MoveRight != MoveLeft) m_xvel += SpeedPerAxis * double(MoveRight ? 1 : -1);
    }
    else{
        if (MoveDown != MoveUp) m_yvel += -(SpeedPerAxis * double(MoveDown ? 1 : -1));
        if (MoveRight != MoveLeft) m_xvel += -(SpeedPerAxis * double(MoveRight ? 1 : -1));
    }
    // RequestUpdate look direction
    int XMouse, YMouse;
    SDL_GetMouseState(&XMouse, &YMouse);

    m_xLook = m_World->CameraX() - m_x + XMouse - m_World->GameWindow()->Width() / 2.0;
    m_yLook = m_World->CameraY() - m_y + YMouse - m_World->GameWindow()->Height() / 2.0;
    double Distance = std::sqrt(std::pow(m_xLook, 2) + std::pow(m_yLook, 2));

    if (Distance != 0.0) {
        m_xLook /= Distance;
        m_yLook /= Distance;
    } else {
        m_xLook = 1.0;
        m_yLook = 0.0;
    }

    auto MouseState = SDL_GetMouseState(nullptr, nullptr);
    m_Using = MouseState & SDL_BUTTON(SDL_BUTTON_LEFT);  // If clicked, shoot = true
    m_Hooking = MouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
}

void Character::TickGameControllerControls() {
    // Check for current joystick values
    double AxisX, AxisY;
    m_GameController->GetJoystick1(AxisX, AxisY);

    // AxisX**2 + AxisY**2 <= 1 (keep direction length of 1)
    double Length = std::sqrt(std::pow(AxisX, 2) + std::pow(AxisY, 2));
    if (Length > 0.2) {  // Fix controller drifting
        if (Length > 0.0) {
            AxisX /= Length;
            AxisY /= Length;
        }
        // Checks if player is shifting (holding left stick)
        bool Shifting = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_LEFTSTICK);
        m_Acceleration = Shifting ? m_BaseAcceleration/3 : m_BaseAcceleration;

        // Accelerate in that direction
        if(!IsReversed) {
            m_xvel += m_Acceleration * AxisX;
            m_yvel += m_Acceleration * AxisY;
        }
        else {
            m_xvel += -(m_Acceleration * AxisX);
            m_yvel += -(m_Acceleration * AxisY);
        }
    }

    // RequestUpdate look direction
    double AxisX2, AxisY2;
    m_GameController->GetJoystick2(AxisX2, AxisY2);

    Length = std::sqrt(std::pow(AxisX2, 2) + std::pow(AxisY2, 2));
    if (Length > 0.6) {  // Fix controller drifting
        if (Length != 0.0) {
            m_xLook = AxisX2 / Length;
            m_yLook = AxisY2 / Length;
        } else {
            m_xLook = 1.0;
            m_yLook = 0.0;
        }
    }

    // Shooting
    m_Using = m_GameController->GetRightTrigger() > 0.7;
    m_Hooking = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    m_Reloading = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_X);

    // Switch weapons
    bool SwitchForward = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    bool SwitchBackward = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT) && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    bool SwitchHands = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_UP) && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_UP) ||
            m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) && !m_GameController->GetLastButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN);

    if (SwitchForward ^ SwitchBackward ^ SwitchHands) { // I hope this works as intended, only 1 at a time | ignore if multiple inputs at the same time
        if (SwitchHands) { m_SelectedWeaponIndex = -1; }
        else if (SwitchForward) {
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

// When in combat heal differently than out of combat
void Character::TickHealth() {
    auto CurrentTick = m_World->CurrentTick();
    bool ActiveRegeneration = CurrentTick - m_LastInCombat < m_RegenerationCooldown;
    m_Health += ActiveRegeneration ? m_PassiveRegeneration : m_ActiveRegeneration;

    if (m_Health > m_MaxHealth)
        m_Health = m_MaxHealth;
}

void Character::TickControls() {
    if (!m_Controllable)
        return;

    if (m_GameController) TickGameControllerControls();
    else TickKeyboardControls();
}

void Character::TickHook() {
    m_Hook.Tick(m_Hooking, m_LastHooking);
}

void Character::TickCollision() {
    auto Player = m_World->FirstPlayer();
    for (; Player; Player = (Character*)(Player->NextType())) {
        if (Player == this) continue;

        double XDistance = m_x - Player->GetX();
        double YDistance = m_y - Player->GetY();
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
        Accelerate(XPush, YPush);
        Player->Accelerate(-XPush, -YPush);
        if(Spiky){
            Player->Damage(1, true);
        }
    }
}

void Character::TickCurrentWeapon() {
    if (m_CurrentWeapon) {
        auto TempAmmo = m_CurrentWeapon->Ammo();
        if (m_Reloading && !m_LastReloading)
            m_CurrentWeapon->Reload();

        m_CurrentWeapon->Tick();
        auto CurrentAmmo = m_CurrentWeapon->Ammo();
        if (TempAmmo != CurrentAmmo) {
            m_AmmoCount->FlagForUpdate();
            if (!CurrentAmmo && TempAmmo) { m_AmmoCount->SetColor({ 255, 0, 0 }); }
            else { m_AmmoCount->SetColor( { 255, 255, 255 } ); }
        }
    } else {
        auto CurrentTick = m_World->CurrentTick();
        if (CurrentTick - m_LastFisted < 5)
            return;

        if (m_Using && !m_LastUsing) {
            m_LastFisted = CurrentTick;

            double Radians = std::atan2(m_yLook, m_xLook);

            double XHands, YHands;
            if (m_LastFistedR < m_LastFistedL) {
                m_LastFistedR = CurrentTick;
                XHands = m_x + std::cos(m_HandSpacing + Radians) * 25.0 + m_xLook * m_FistingRadius;
                YHands = m_y + std::sin(m_HandSpacing + Radians) * 25.0 + m_yLook * m_FistingRadius;
            } else {
                m_LastFistedL = CurrentTick;
                XHands = m_x + std::cos(-m_HandSpacing + Radians) * 25.0 + m_xLook * m_FistingRadius;
                YHands = m_y + std::sin(-m_HandSpacing + Radians) * 25.0 + m_yLook * m_FistingRadius;
            }

            auto Ent = m_World->FirstEntity();
            for (; Ent; Ent = Ent->Next()) {
                if (Ent == this)
                    continue;

                double XClosest = std::max(Ent->GetX() - Ent->GetW() / 2.0, std::min(Ent->GetX() + Ent->GetW() / 2.0, XHands));
                double YClosest = std::max(Ent->GetY() - Ent->GetH() / 2.0, std::min(Ent->GetY() + Ent->GetH() / 2.0, YHands));
                double Distance = std::sqrt(std::pow(XClosest - XHands, 2) + std::pow(YClosest - YHands, 2));
                if (Distance > m_FistingRadius)
                    continue;

                Ent->Accelerate(m_xLook * 5.0, m_yLook * 5.0);
                if (Ent->EntityType() != GameWorld::ENTTYPE_CHARACTER)
                    continue;

                ((Character*)Ent)->Damage(7, true);
            }
        }
    }
}

void Character::DrawCharacter() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_FRect DrawRect = {float(m_x) - float(m_w / 2.0),
                          float(m_y) - float(m_h / 2.0),
                          float(m_w),
                          float(m_h)};

   // if(m_HitTicks > 0) Render->SetColor(255, 0, 0, 255);
   // else { Render->SetColor(m_CharacterColor.r, m_CharacterColor.g, m_CharacterColor.b, 255); }
    // Render->FillRectFWorld(DrawRect);
    // Render->RenderTextureFWorld(ms_Texture->SDLTexture(), nullptr,DrawRect);
    ms_Texture->SetColorMod(m_CharacterColor.r, m_CharacterColor.g, m_CharacterColor.b);

    double Angle = std::atan2(m_yvel, m_xvel) / M_PI * 180.0;
    Render->RenderTextureExFWorld(ms_Texture->SDLTexture(), nullptr, DrawRect, Angle, nullptr, SDL_FLIP_NONE);
}

void Character::DrawHook() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    // Draw hook
    if (m_Hook.m_Deployed) {
        Render->SetColor(m_HookColor.r, m_HookColor.g, m_HookColor.b, 255);
        Render->LineWorld(m_x, m_y, m_Hook.m_x, m_Hook.m_y);
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
        SDL_Rect HealthplateRect = { int(m_x - healthplate_w / 2.0), int(m_y + m_h / 2.0), healthplate_w, healthplate_h };
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
        SDL_Rect HealthIntRect = { int(m_x - healthplate_w/2 / 2.0) ,int(m_y +m_h/2+healthplate_h/4), healthplate_w/2, healthplate_h/2 };

        Render->RenderTextureWorld(HealthPlate->SDLTexture(), nullptr, HealthplateRect);
        Render->RenderTextureWorld(HealthTexture->SDLTexture(), nullptr, HealthIntRect);
    }
}

void Character::DrawHands() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    double XLook = m_x + m_xLook * 50.0;
    double YLook = m_y + m_yLook * 50.0;
    Render->SetColor(m_HandColor.r, m_HandColor.g, m_HandColor.b, 255);
    Render->LineWorld(int(m_x), int(m_y), int(XLook), int(YLook));

    // TODO: Make different hand positions for different weapons (interesting problem)
    if (m_CurrentWeapon)
        return;

    auto CurrentTick = m_World->CurrentTick();
    double Radians = std::atan2(m_yLook, m_xLook);
    double Angle = Radians / M_PI * 180.0;


    double FistingKoefficientL = double(CurrentTick - m_LastFistedL) / double(m_FistingAnimationDuration);
    double FistingKoefficientR = double(CurrentTick - m_LastFistedR) / double(m_FistingAnimationDuration);
    if (FistingKoefficientL > 1.0) FistingKoefficientL = 1.0;
    if (FistingKoefficientR > 1.0) FistingKoefficientR = 1.0;

    FistingKoefficientL = (1.0 - FistingKoefficientL) * m_FistingRadius;
    FistingKoefficientR = (1.0 - FistingKoefficientR) * m_FistingRadius;

    double XOffLeft = std::cos(-m_HandSpacing + Radians) * 25.0 + m_xLook * FistingKoefficientL;
    double YOffLeft = std::sin(-m_HandSpacing + Radians) * 25.0 + m_yLook * FistingKoefficientL;
    double XOffRight = std::cos(m_HandSpacing + Radians) * 25.0 + m_xLook * FistingKoefficientR;
    double YOffRight = std::sin(m_HandSpacing + Radians) * 25.0 + m_yLook * FistingKoefficientR;

    SDL_FRect HandRectLeft = { float(m_x - 9 + XOffLeft),
                               float(m_y - 9 + YOffLeft),
                               18, 18 };
    SDL_FRect HandRectRight = { float(m_x - 9 + XOffRight),
                                float(m_y - 9 + YOffRight),
                                18, 18 };

    ms_FistTexture->SetColorMod(m_HookColor.r, m_HookColor.g, m_HookColor.b);
    Render->RenderTextureExFWorld(ms_FistTexture->SDLTexture(), nullptr, HandRectLeft, Angle, nullptr, SDL_FLIP_NONE);
    Render->RenderTextureExFWorld(ms_FistTexture->SDLTexture(), nullptr, HandRectRight, Angle, nullptr, SDL_FLIP_NONE);
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
    SDL_Rect NamePlateRect = { int(m_x - nameplate_w / 2.0), int(m_y - m_h / 2.0 - nameplate_h),
                      nameplate_w, nameplate_h };

    SDL_SetTextureAlphaMod(NamePlateTexture->SDLTexture(), Opacity);
    Render->RenderTextureWorld(NamePlateTexture->SDLTexture(), nullptr, NamePlateRect);


    char msg[64];
    std::snprintf(msg, sizeof(msg), "%ix, %iy", int(m_x), int(m_y));
    m_CoordinatePlate->SetText(msg);
    m_CoordinatePlate->SetColor(m_NameplateColor);
    Texture* CoordinateTexture = m_CoordinatePlate->RequestUpdate();

    int coordinate_w, coordinate_h;
    CoordinateTexture->Query(nullptr, nullptr, &coordinate_w, &coordinate_h);
    SDL_Rect CoordinateRect = { int(m_x - coordinate_w / 2.0), int(NamePlateRect.y - coordinate_h), coordinate_w, coordinate_h };
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
    SDL_Rect AmmoRect = { int(m_x - Ammo_w / 2.0) ,int(m_y +m_h/2+20), Ammo_w, Ammo_h };
    Render->RenderTextureWorld(AmmoTexture->SDLTexture(), nullptr, AmmoRect);
}

void Character::Event(const SDL_Event& currentEvent) {
    if (!m_Controllable || m_GameController)
        return;

    if (currentEvent.type == SDL_KEYDOWN && currentEvent.key.repeat == 0 ||
        currentEvent.type == SDL_KEYUP) {
        bool State = currentEvent.type == SDL_KEYDOWN;
        if (State) {
            int KeyCode = currentEvent.key.keysym.scancode;
            if (KeyCode == SDL_SCANCODE_GRAVE) { m_CurrentWeapon = nullptr; }
            else {
                if (KeyCode == SDL_SCANCODE_1) { SwitchWeapon(WEAPON_GLOCK); }
                else if (KeyCode == SDL_SCANCODE_2) { SwitchWeapon(WEAPON_SHOTGUN); }
                else if (KeyCode == SDL_SCANCODE_3) { SwitchWeapon(WEAPON_BURST); }
                else if (KeyCode == SDL_SCANCODE_4) { SwitchWeapon(WEAPON_MINIGUN); }
            }
        }

        // Reloads weapon on keyboard player with R button press
        if (currentEvent.key.keysym.scancode == SDL_SCANCODE_R)
            m_Reloading = State;

        for (int i = 0; i < NUM_CONTROLS; i++) {
            if (currentEvent.key.keysym.scancode == ms_DefaultControls[i])
                m_Movement[i] = State;
        }
    }
}

void Character::Tick() {
    // TODO: Make controls come from m_Player/m_AI rather than m_GameController
    TickHealth();
    TickControls();  // Do stuff depending on the current held buttons
    TickCurrentWeapon(); // Shoot accelerate reload etc.
    TickHook();  // Move hook and or player etc.
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
        ((int)(m_x) != (int)(m_xLast) ||
        (int)(m_y) != (int)(m_yLast)))
        m_CoordinatePlate->FlagForUpdate();

    m_LastHealth = m_Health;
    m_LastUsing = m_Using;
    m_LastHooking = m_Hooking;
    m_LastReloading = m_Reloading;
    m_xLast = m_x;
    m_yLast = m_y;

    m_HitTicks -= 1;
    if (m_HitTicks < 0)
        m_HitTicks = 0;

    if (m_Health <= 0.0) {
        m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(ms_DeathSound);
        delete this;
    }
}

void Character::Draw() {
    Clock* Timer = m_World->GameWindow()->Timer();
    double Light = 0.5 + (std::sin(Timer->GetTotalTimeElapsed() - m_ExistsSince) + 1.0) / 4;

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
    if(m_CurrentWeapon) DrawAmmo();
}
