//
// Created by 11dpjgailis on 16.03.2023.
//
#include "Character.h"
#include <cmath>
#include <iostream>
#include "Bullets.h"
#include <vector>

Sound* Character::ch_HitSound = nullptr;
Sound* Character::ch_DeathSound = nullptr;
static double sDiagonalLength = 1.0 / std::sqrt(2.0);
const int Character::sDefaultControls[NUM_CONTROLS] = {SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A };

Hook::Hook(Character* parent) {
    m_Parent = parent;
    m_x = 0.0;
    m_y = 0.0;
    m_xvel = 0.0;
    m_yvel = 0.0;
    m_MaxLength = 300.0;
    m_HookTravelSpeed = 35.0;
    m_WallDragForce = 1.5;

    m_HookerInfluenceRatio = 0.0;
    m_HookistInfluenceRatio = 0.0;
    SetInfluenceRatio(0.7);

    m_Deployed = false;
    m_Grabbed = GRABBED_NONE;
    m_GrabbedEntity = nullptr;
}

// The percentage of force that is applied to the hookist
// the remaining percentage will be applied to the hooker.
void Hook::SetInfluenceRatio(double ratio) {
    m_HookerInfluenceRatio = 1.0 - ratio;
    m_HookistInfluenceRatio = ratio;
}

// Set the hook as retracted
void Hook::Unhook() {
    m_Deployed = false;
    m_Grabbed = GRABBED_NONE;
    m_GrabbedEntity = nullptr;
}

// Set the hook as grabbed to the wall
void Hook::HookWall() {
    m_xvel = 0.0;
    m_yvel = 0.0;
    m_Grabbed = GRABBED_WALL;
}

void Hook::Tick(bool hooking, bool last_hooking) {
    GameWorld* World = m_Parent->World();

    if (!m_Deployed && hooking && !last_hooking) {
        m_Deployed = true;
        m_x = m_Parent->m_x;
        m_y = m_Parent->m_y;
        m_xvel = m_Parent->m_xLook * m_HookTravelSpeed;
        m_yvel = m_Parent->m_yLook * m_HookTravelSpeed;
    } else if (m_Deployed && !hooking && last_hooking) {  // Instant retraction for now
        Unhook();
    }

    if (!m_Deployed)
        return;

    if (m_Grabbed == GRABBED_NONE) {
        m_x += m_xvel;
        m_y += m_yvel;
    }

    double TravelX = m_x - m_Parent->m_x;
    double TravelY = m_y - m_Parent->m_y;
    double Length = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
    if (Length != 0.0) {
        TravelX /= Length;
        TravelY /= Length;
    }

    if (m_Grabbed == GRABBED_NONE) {
        // Make sure hook isn't longer than it is allowed to be
        if (Length > m_MaxLength) {
            m_x = m_Parent->m_x + TravelX * m_MaxLength;
            m_y = m_Parent->m_y + TravelY * m_MaxLength;
            m_xvel -= TravelX * 2.0;
            m_yvel -= TravelY * 2.0;
        }

        // Hook snaps to player - idk if its good or not cus i havent made it yet
        Character* Player = m_Parent->World()->FirstPlayer();
        for (; Player; Player = (Character*)(Player->NextType())) {
            if (Player == m_Parent)
                continue;

            if (Player->PointCollides(m_x, m_y)) {
                m_Grabbed = GRABBED_ENTITY;
                m_GrabbedEntity = Player;
                break;
            }
        }

        if (m_Grabbed == GRABBED_NONE) {
            // Hook snaps to wall - fix later cus ugly, prob fix when adding tiles and stuff cus doesnt rly matter tbh
            if (m_x < 0.0) {
                m_x = 0.0;
                HookWall();
            } else if (m_y < 0.0) {
                m_y = 0.0;
                HookWall();
            } else if (m_x > World->Width()) {
                m_x = World->Width();
                HookWall();
            } else if (m_y > World->Height()) {
                m_y = World->Height();
                HookWall();
            }
        }
    } else if (m_Grabbed == GRABBED_ENTITY) {
        m_x = m_GrabbedEntity->GetX();
        m_y = m_GrabbedEntity->GetY();
        if (m_GrabbedEntity->EntityType() == GameWorld::ENTTYPE_CHARACTER) {
            auto Player = (Character*)(m_GrabbedEntity);
            double Acceleration = Length / m_MaxLength * (1 - m_HookerInfluenceRatio);
            double Influence = Length / m_MaxLength * m_HookerInfluenceRatio;
            Player->Accelerate(-TravelX * Acceleration, -TravelY * Acceleration);
            m_Parent->Accelerate(TravelX * Influence, TravelY * Influence);
        }
    } else if (m_Grabbed == GRABBED_WALL) {
        m_Parent->m_xvel += TravelX * m_WallDragForce;
        m_Parent->m_yvel += TravelY * m_WallDragForce;
    }
}

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
    TextManager* TextHandler = world->GameWindow()->TextHandler();
    TTF_Font* Font = TextHandler->FirstFont();
    m_Nameplate = TextHandler->Render(Font, Name, { 255, 255, 255 }, true);

    m_HitTicks = false;
}

Character::~Character() {
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

void Character::TickKeyboardControls() {
    // Check if buttons are held
    bool MoveUp = m_Movement[CONTROL_UP];
    bool MoveRight = m_Movement[CONTROL_RIGHT];
    bool MoveDown = m_Movement[CONTROL_DOWN];
    bool MoveLeft = m_Movement[CONTROL_LEFT];

    bool Horizontally = MoveLeft != MoveRight;
    bool Vertically = MoveUp != MoveDown;

    // Accelerate when buttons are held
    double LengthPerAxis = (Horizontally && Vertically) ? sDiagonalLength : 1.0;
    double SpeedPerAxis = m_BaseAcceleration * LengthPerAxis;

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

        // Accelerate in that direction
        m_xvel += m_BaseAcceleration * AxisX;
        m_yvel += m_BaseAcceleration * AxisY;
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

void Character::Event(const SDL_Event& currentEvent) {
    if (!m_Controllable || m_GameController)
        return;

    if (currentEvent.type == SDL_KEYDOWN ||
        currentEvent.type == SDL_KEYUP) {
        bool State = currentEvent.type == SDL_KEYDOWN;
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

    if (m_Health <= 0.0) {
        m_World->GameWindow()->SoundHandler()->PlaySound(ch_DeathSound);
        delete this;
    }
}

void Character::Draw() {
    Clock* Timer = m_World->GameWindow()->Timer();
    Drawing* Render = m_World->GameWindow()->RenderClass();

    double Light = 0.5 + (std::sin(Timer->GetTotalTimeElapsed() - m_ExistsSince) + 1.0) / 4;
    SDL_Color Color = HSLtoRGB({ m_ColorHue, Light, 1.0 });

    if (m_Hook.m_Deployed) {
        Render->SetColor(Color.r, Color.g, Color.b, 255);
        Render->LineWorld(m_x, m_y, m_Hook.m_x, m_Hook.m_y);
    }

    SDL_FRect DrawRect = {float(m_x) - float(m_w / 2.0),
                          float(m_y) - float(m_h / 2.0),
                          float(m_w),
                          float(m_h)};
    Color = HSLtoRGB({ m_ColorHue, 1.0, Light });
    // SoundManager *SoundHandler = m_World->GameWindow()->SoundHandler();
    if(m_HitTicks > 0) {
        Render->SetColor(255, 0, 0, 255);
        // SoundHandler->PlaySound(ch_HitSound);
        // Need to decide if that's needed, since that's alot of sound, if it's going to be every bullet and every hit
        m_HitTicks -=1;
    }
    //Can later make it so the less hp the more red the character
    else { Render->SetColor(Color.r, Color.g, Color.b, 255); }
    Render->FillRectFWorld(DrawRect);

    // Render health bar
    if (m_Health != m_MaxHealth) {
        m_HealthBar.SetColor(Color.r, Color.g, Color.b, Color.a);
        Texture* HealthPlate = m_HealthBar.UpdateTexture();

        int healthplate_w, healthplate_h;
        HealthPlate->Query(nullptr, nullptr, &healthplate_w, &healthplate_h);
        SDL_Rect HealthplateRect = { int(m_x - healthplate_w / 2.0), int(m_y + m_h / 2.0), healthplate_w, healthplate_h };
        Render->RenderTextureWorld(HealthPlate->SDLTexture(), nullptr, HealthplateRect);
    }

    // Render the direction line
    double XLook = m_x + m_xLook * 50.0;
    double YLook = m_y + m_yLook * 50.0;
    Color = HSLtoRGB({ m_ColorHue, 1.0 - Light, 1.0 });
    Render->SetColor(Color.r, Color.g, Color.b, 255);
    Render->LineWorld(int(m_x), int(m_y), int(XLook), int(YLook));

    if (m_World->NamesShown() <= 0.05)  // Visibility under 5% - don't render the texts
        return;

    int Opacity = int(m_World->NamesShown() * 255.0);

    int nameplate_w, nameplate_h;
    m_Nameplate->Query(nullptr, nullptr, &nameplate_w, &nameplate_h);
    SDL_Rect NameplateRect = { int(m_x - nameplate_w / 2.0), int(m_y - m_h / 2.0 - nameplate_h), nameplate_w, nameplate_h };
    SDL_SetTextureAlphaMod(m_Nameplate->SDLTexture(), Opacity);
    Render->RenderTextureWorld(m_Nameplate->SDLTexture(), nullptr, NameplateRect);

    TextManager* TextHandler = m_World->GameWindow()->TextHandler();
    char msg[64];
    std::snprintf(msg, sizeof(msg), "%ix, %iy", int(m_x), int(m_y));
    auto CoordinateTexture = TextHandler->Render(TextHandler->FirstFont(), msg, { Color.r, Color.g, Color.b, 255 }, false);
    int coordinate_w, coordinate_h;
    CoordinateTexture->Query(nullptr, nullptr, &coordinate_w, &coordinate_h);
    SDL_Rect CoordinateRect = { int(m_x - coordinate_w / 2.0), int(NameplateRect.y - coordinate_h), coordinate_w, coordinate_h };
    SDL_SetTextureAlphaMod(CoordinateTexture->SDLTexture(), Opacity);
    Render->RenderTextureWorld(CoordinateTexture->SDLTexture(), nullptr, CoordinateRect);
    delete CoordinateTexture;
}
