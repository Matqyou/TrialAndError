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
// TODO: see if we can make a little system that tells us if the textures/sounds are unitialized

static double sDiagonalLength = 1.0 / std::sqrt(2.0);
const int Character::sDefaultControls[NUM_CONTROLS] = {SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A, SDL_SCANCODE_LSHIFT };

Character::Character(GameWorld* world, double start_x, double start_y, double start_xvel, double start_yvel)
 : Entity(world, GameWorld::ENTTYPE_CHARACTER, start_x, start_y, 50, 50, 0.93),
   m_Hook(this),
   m_BaseAcceleration(0.75),
   m_HealthBar(world->GameWindow(), &m_Health, &m_MaxHealth, 75, 15, 2, 2) {
    m_PlayerIndex = 0;
    m_ColorHue = double(rand()%360);
    m_Shooting = false;
    m_LastShooting = false;
    m_Reloading = false;
    m_LastReloading = false;

    m_CurrentWeapon = nullptr; // Start by holding nothing

    // Always set to null, cuz player should spawn without weapons
    memset(m_Weapons, 0, sizeof(m_Weapons));

    // But this is Latvia and we give the character free guns
    m_Weapons[WEAPON_GLOCK] = new WeaponGlock(this);
    m_Weapons[WEAPON_BURST] = new WeaponBurst(this);
    m_Weapons[WEAPON_SHOTGUN] = new WeaponShotgun(this);
    m_Weapons[WEAPON_MACHINEGUN] = new WeaponMinigun(this);

    m_MaxHealth = 100.0;
    m_Health = m_MaxHealth;

    m_GameController = nullptr;
    for (bool& State : m_Movement)
        State = false;

    m_World->GetNextPlayerIndex(this);
    if (m_PlayerIndex == 1) { memcpy(m_Controls, sDefaultControls, sizeof(m_Controls)); }  // Controls are copied
    else { memset(m_Controls, 0, sizeof(m_Controls)); }  // All controls are set to 0
    m_Controllable = true;

    m_xvel = start_xvel;
    m_yvel = start_yvel;
    m_xLook = 1.0;
    m_yLook = 0.0;

    m_Hooking = false;
    m_LastHooking = false;

    char Name[CHARACTER_MAX_NAME_LENGTH];
    std::snprintf(Name, CHARACTER_MAX_NAME_LENGTH, "Player%i", m_PlayerIndex);
    m_Name = Name;
    TextManager* TextHandler = world->GameWindow()->Assets()->TextHandler();
    TTF_Font* Font = TextHandler->FirstFont();
    m_Nameplate = new TextSurface(m_World->GameWindow()->Assets(),
                                  m_World->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                  Name, { 255, 255, 255, 255 });
    m_AmmoCount = new TextSurface(m_World->GameWindow()->Assets(),
                                  m_World->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                  "0", { 255, 255, 255, 255 });
    m_CoordinatePlate = new TextSurface(m_World->GameWindow()->Assets(),
                                        m_World->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                        "-x, -y", { 255, 255, 255, 255 });

    m_HealthInt = new TextSurface(m_World->GameWindow()->Assets(),
                                  m_World->GameWindow()->Assets()->TextHandler()->FirstFont(),
                                  "0", { 0, 0, 0, 255 });
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
    delete m_Nameplate;
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

// Add some velocity to this character
void Character::Accelerate(double accelerate_x, double accelerate_y) {
    m_xvel += accelerate_x;
    m_yvel += accelerate_y;
}

void Character::Damage(double damage, bool make_sound) {
    m_Health -= damage;
    m_HitTicks = 7;

    if (!make_sound)
        return;

    Sound* HurtSound = ms_HitSounds[rand()%3];
    m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(HurtSound);
}


bool Character::AmmoPickup(Ammo* ammo){
    if((ammo->Type() == GLOCK_AMMO) and (m_Weapons[WEAPON_GLOCK] != nullptr)) {
        AmmoAdded = m_Weapons[WEAPON_GLOCK]->SetAmmo(ammo->AmmoCount());
        ammo->SetAmmoCount(AmmoAdded);
        if (0 >= ammo->AmmoCount()) {
            return true;
        }
    }
    if ((ammo->Type() == BURST_AMMO) and (m_Weapons[WEAPON_BURST] != nullptr)) {
        AmmoAdded = m_Weapons[WEAPON_BURST]->SetAmmo(ammo->AmmoCount());
        ammo->SetAmmoCount(AmmoAdded);
        if (0 >= ammo->AmmoCount()) {
            return true;
        }
    }
    if ((ammo->Type() == SHOTGUN_AMMO) and (m_Weapons[WEAPON_SHOTGUN] != nullptr)) {
        AmmoAdded = m_Weapons[WEAPON_SHOTGUN]->SetAmmo(ammo->AmmoCount());
        ammo->SetAmmoCount(AmmoAdded);
        if (0 >= ammo->AmmoCount()) {
            return true;
        }
    }
    if ((ammo->Type() == MACHINEGUN_AMMO) and (m_Weapons[WEAPON_MACHINEGUN] != nullptr)) {
        AmmoAdded = m_Weapons[WEAPON_MACHINEGUN]->SetAmmo(ammo->AmmoCount());
        ammo->SetAmmoCount(AmmoAdded);
        if (0 >= ammo->AmmoCount()) {
            return true;
        }
    }
}


void Character::TickKeyboardControls() {
    // Check if buttons are held
    bool MoveUp = m_Movement[CONTROL_UP];
    bool MoveRight = m_Movement[CONTROL_RIGHT];
    bool MoveDown = m_Movement[CONTROL_DOWN];
    bool MoveLeft = m_Movement[CONTROL_LEFT];
    bool SlowMove = m_Movement[CONTROL_SHIFT];

    bool Horizontally = MoveLeft != MoveRight;
    bool Vertically = MoveUp != MoveDown;

    // Depending on if shift is held, change accelaration value
    if(SlowMove) m_Acceleration = m_BaseAcceleration/3;
    else m_Acceleration = m_BaseAcceleration;

    // Accelerate when buttons are held
    double LengthPerAxis = (Horizontally && Vertically) ? sDiagonalLength : 1.0;
    double SpeedPerAxis = m_Acceleration * LengthPerAxis;

    if (MoveDown != MoveUp) m_yvel += SpeedPerAxis * double(MoveDown ? 1 : -1);
    if (MoveRight != MoveLeft) m_xvel += SpeedPerAxis * double(MoveRight ? 1 : -1);
    // Update look direction
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
    m_Shooting = MouseState & SDL_BUTTON(SDL_BUTTON_LEFT);  // If clicked, shoot = true
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
        //Checks if player is shifting (holding left stick)
        if(m_GameController->GetButton(SDL_CONTROLLER_BUTTON_LEFTSTICK)){
            m_Acceleration = m_BaseAcceleration/3;
        }
        else m_Acceleration = m_BaseAcceleration;

        // Accelerate in that direction
        m_xvel += m_Acceleration * AxisX;
        m_yvel += m_Acceleration * AxisY;
    }

    // Update look direction
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
    m_Shooting = m_GameController->GetRightTrigger() > 0.7;
    m_Hooking = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    m_Reloading = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_X);

    // Switch weapons
    if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_UP) && m_Weapons[WEAPON_GLOCK])
        m_CurrentWeapon = m_Weapons[WEAPON_GLOCK];
    else if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && m_Weapons[WEAPON_SHOTGUN])
        m_CurrentWeapon = m_Weapons[WEAPON_SHOTGUN];
    else if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) && m_Weapons[WEAPON_BURST])
        m_CurrentWeapon = m_Weapons[WEAPON_BURST];
    else if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT) && m_Weapons[WEAPON_MACHINEGUN])
        m_CurrentWeapon = m_Weapons[WEAPON_MACHINEGUN];
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

void Character::TickWeapon() {
    if (!m_CurrentWeapon)
        return;

    if (m_Reloading && !m_LastReloading)
        m_CurrentWeapon->Reload();

    m_CurrentWeapon->Tick();
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
    // ms_FistTexture->SetColorMod(m_CharacterColor.r, m_CharacterColor.g, m_CharacterColor.b);

    double Angle = std::atan2(m_yvel, m_xvel) / M_PI * 180.0;
    Render->RenderTextureExFWorld(ms_FistTexture->SDLTexture(), nullptr, DrawRect, Angle, nullptr, SDL_FLIP_NONE);
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
        Texture* HealthPlate = m_HealthBar.UpdateTexture();

        int healthplate_w, healthplate_h;
        HealthPlate->Query(nullptr, nullptr, &healthplate_w, &healthplate_h);
        SDL_Rect HealthplateRect = { int(m_x - healthplate_w / 2.0), int(m_y + m_h / 2.0), healthplate_w, healthplate_h };
        char msg[64];

        std::snprintf(msg, sizeof(msg), "%i/%i", int(m_Health), int(m_MaxHealth));
        m_HealthInt->SetText(msg);

        if(m_Health < 50)m_HealthInt->SetColor(m_HealthRed);
        else m_HealthInt->SetColor(m_HealthBlack);
        Texture* HealthTexture = m_HealthInt->Update();
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

    double Radians = std::atan2(m_yLook, m_xLook);
    double Angle = Radians / M_PI * 180.0;

    double OffAngle = 40.0 / 180.0 * M_PI;
    double XOffLeft = std::cos(-OffAngle + Radians) * 25.0;
    double YOffLeft = std::sin(-OffAngle + Radians) * 25.0;
    double XOffRight = std::cos(OffAngle + Radians) * 25.0;
    double YOffRight = std::sin(OffAngle + Radians) * 25.0;

    SDL_FRect HandRectLeft = { float(m_x - 9 + XOffLeft),
                               float(m_y - 9 + YOffLeft),
                               18, 18 };
    SDL_FRect HandRectRight = { float(m_x - 9 + XOffRight),
                                float(m_y - 9 + YOffRight),
                                18, 18 };

    // ms_FistTexture->SetColorMod(m_HookColor.r, m_HookColor.g, m_HookColor.b);
    Render->RenderTextureExFWorld(ms_FistTexture->SDLTexture(), nullptr, HandRectLeft, Angle, nullptr, SDL_FLIP_NONE);
    Render->RenderTextureExFWorld(ms_FistTexture->SDLTexture(), nullptr, HandRectRight, Angle, nullptr, SDL_FLIP_NONE);
}

void Character::DrawNameplate() {
    if (m_World->NamesShown() <= 0.05)  // Visibility under 5% - don't render the texts
        return;

    Drawing* Render = m_World->GameWindow()->RenderClass();

    int Opacity = int(m_World->NamesShown() * 255.0);

    int nameplate_w, nameplate_h;
    Texture* NameplateTexture = m_Nameplate->Update();
    NameplateTexture->Query(nullptr, nullptr, &nameplate_w, &nameplate_h);
    SDL_Rect NameplateRect = { int(m_x - nameplate_w / 2.0), int(m_y - m_h / 2.0 - nameplate_h), nameplate_w, nameplate_h };

    SDL_SetTextureAlphaMod(NameplateTexture->SDLTexture(), Opacity);
    Render->RenderTextureWorld(NameplateTexture->SDLTexture(), nullptr, NameplateRect);

    char msg[64];
    std::snprintf(msg, sizeof(msg), "%ix, %iy", int(m_x), int(m_y));
    m_CoordinatePlate->SetText(msg);
    m_CoordinatePlate->SetColor(m_NameplateColor);
    Texture* CoordinateTexture = m_CoordinatePlate->Update();

    int coordinate_w, coordinate_h;
    CoordinateTexture->Query(nullptr, nullptr, &coordinate_w, &coordinate_h);
    SDL_Rect CoordinateRect = { int(m_x - coordinate_w / 2.0), int(NameplateRect.y - coordinate_h), coordinate_w, coordinate_h };
    SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), Opacity);
    Render->RenderTextureWorld(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect);
}



void Character::DrawAmmo(){
    Drawing* Render = m_World->GameWindow()->RenderClass();
    char msg[64];
    std::snprintf(msg, sizeof(msg), "%u/%u", m_CurrentWeapon->Ammo(), m_CurrentWeapon->TrueAmmo());
    m_AmmoCount->SetText(msg);
    Texture* AmmoTexture = m_AmmoCount->Update();

    int Ammo_w, Ammo_h;
    AmmoTexture->Query(nullptr, nullptr, &Ammo_w, &Ammo_h);
    SDL_Rect AmmoRect = { int(m_x - Ammo_w / 2.0) ,int(m_y +m_h/2+20), Ammo_w, Ammo_h };
    Render->RenderTextureWorld(AmmoTexture->SDLTexture(), nullptr, AmmoRect);
}
void Character::Event(const SDL_Event& currentEvent) {
    if (!m_Controllable || m_GameController)
        return;

    if (currentEvent.type == SDL_KEYDOWN ||
        currentEvent.type == SDL_KEYUP) {
        bool State = currentEvent.type == SDL_KEYDOWN;
        if (currentEvent.key.keysym.scancode == SDL_SCANCODE_GRAVE)
            m_CurrentWeapon = nullptr;
        if (currentEvent.key.keysym.scancode == SDL_SCANCODE_1 && m_Weapons[WEAPON_GLOCK])
            m_CurrentWeapon = m_Weapons[WEAPON_GLOCK];
        else if (currentEvent.key.keysym.scancode == SDL_SCANCODE_2 && m_Weapons[WEAPON_SHOTGUN])
            m_CurrentWeapon = m_Weapons[WEAPON_SHOTGUN];
        else if (currentEvent.key.keysym.scancode == SDL_SCANCODE_3 && m_Weapons[WEAPON_BURST])
            m_CurrentWeapon = m_Weapons[WEAPON_BURST];
        else if (currentEvent.key.keysym.scancode == SDL_SCANCODE_4 && m_Weapons[WEAPON_MACHINEGUN])
            m_CurrentWeapon = m_Weapons[WEAPON_MACHINEGUN];

        // Reloads weapon on keyboard player with R button press
        if (currentEvent.key.keysym.scancode == SDL_SCANCODE_R)
            m_Reloading = State;

        for (int i = 0; i < NUM_CONTROLS; i++) {
            if (currentEvent.key.keysym.scancode == m_Controls[i])
                m_Movement[i] = State;
        }
    }
}

void Character::Tick() {
    m_Health += 0.1;
    if (m_Health > m_MaxHealth)
        m_Health = m_MaxHealth;

    TickControls();  // Do stuff depending on the current held buttons
    TickWeapon(); // Shoot accelerate reload etc.
    TickHook();  // Move hook and or player etc.

    // Need every character to get here..
    // then we apply the accelerations of all
    // hooks and continue with the code below v v v
    // like collisions and velocity tick

    TickVelocity();  // Move the character entity
    TickWalls();  // Check if colliding with walls

    m_LastShooting = m_Shooting;
    m_LastHooking = m_Hooking;
    m_LastReloading = m_Reloading;

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
