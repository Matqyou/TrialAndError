//
// Created by 11dpjgailis on 16.03.2023.
//
#include "Character.h"
#include <cmath>
#include <iostream>
#include "Bullets.h"
#include <vector>

static double sDiagonalLength = 1.0 / std::sqrt(2.0);
const int Character::sDefaultControls[NUM_CONTROLS] = {SDL_SCANCODE_W, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_A };

Character::Character(GameWorld* world, double start_x, double start_y, double start_xvel, double start_yvel)
 : Entity(world, GameWorld::ENTTYPE_CHARACTER, start_x, start_y, 50, 50, 0.93) {
    m_PlayerIndex = 0;
    m_ColorHue = double(rand()%360);
    m_Shoot = false;
    m_LastShoot = false;

    m_CurrentWeapon = nullptr; // Start by holding nothing

    // Always set to null, cuz player should spawn without weapons
    memset(m_Weapons, 0, sizeof(m_Weapons));

    // But this is Latvia and we give the character free guns
    m_Weapons[WEAPON_GLOCK] = new WeaponGlock(this);
    m_Weapons[WEAPON_BURST] = new WeaponBurst(this);
    m_Weapons[WEAPON_SHOTGUN] = new WeaponShotgun(this);
    m_Weapons[WEAPON_MACHINEGUN] = new WeaponMinigun(this);

    m_Health = 100.0;
    //m_MachinegunTick = 0.0;
    //m_BurstTick = 0;
    //m_BurstShots = 0;
    //m_StartBurstShots = 3;
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

    m_xHook = 0.0;
    m_yHook = 0.0;
    m_xvelHook = 0.0;
    m_yvelHook = 0.0;
    m_HookDeployed = false;
    m_Hook = false;
    m_LastHook = false;
    m_HookGrabbedWall = false;

    char Name[CHARACTER_MAX_NAME_LENGTH];
    std::snprintf(Name, CHARACTER_MAX_NAME_LENGTH, "Player%i", m_PlayerIndex);
    m_Name = Name;
    TextManager* TextHandler = world->GameWindow()->TextHandler();
    TTF_Font* Font = TextHandler->FirstFont();
    m_Nameplate = TextHandler->Render(Font, Name, { 255, 255, 255 }, true);
}

Character::~Character() {

}

void Character::SetGameController(GameController* gameController) {
    m_GameController = gameController;
}

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
    m_Shoot = MouseState & SDL_BUTTON(SDL_BUTTON_LEFT);  // If clicked, shoot = true
    m_Hook = MouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
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
    m_Shoot = m_GameController->GetRightTrigger() > 0.7;
    m_Hook = m_GameController->GetButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

    // Switch weapons
    if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_UP) && m_Weapons[WEAPON_GLOCK])
        m_CurrentWeapon = m_Weapons[WEAPON_GLOCK];
    else if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) && m_Weapons[WEAPON_SHOTGUN])
        m_CurrentWeapon = m_Weapons[WEAPON_SHOTGUN];
    else if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_DOWN) && m_Weapons[WEAPON_BURST])
        m_CurrentWeapon = m_Weapons[WEAPON_BURST];
    else if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_DPAD_LEFT) && m_Weapons[WEAPON_MACHINEGUN])
        m_CurrentWeapon = m_Weapons[WEAPON_MACHINEGUN];

    //Reloads weapon on square button press on controller
    if (m_GameController->GetButton(SDL_CONTROLLER_BUTTON_X) && m_CurrentWeapon){
        m_CurrentWeapon->Reload();

        //Change later to single pointer to weapon
        // if(m_Weapon == WEAPON_GLOCK){
        //     m_Glock.Reload();
        // }
        // else if(m_Weapon == WEAPON_BURST){
        //     m_Burst.Reload();
        // }
        // else if(m_Weapon == WEAPON_MACHINEGUN){
        //     m_Minigun.Reload();
        // }
        // else if(m_Weapon == WEAPON_SHOTGUN){
        //     m_Shotgun.Reload();
        // }
    }
}

void Character::TickControls() {
    if (!m_Controllable)
        return;

    if (m_GameController) TickGameControllerControls();
    else TickKeyboardControls();
}

void Character::TickHook() {
    if (!m_HookDeployed && m_Hook && !m_LastHook) {
        m_HookDeployed = true;
        m_xHook = m_x;
        m_yHook = m_y;
        m_xvelHook = m_xLook * 35;
        m_yvelHook = m_yLook * 35;
    } else if (m_HookDeployed && !m_Hook && m_LastHook) {  // Instant retraction for now
        m_HookDeployed = false;
        m_HookGrabbedWall = false;
    }

    if (!m_HookDeployed)
        return;

    m_xHook += m_xvelHook;
    m_yHook += m_yvelHook;

    double TravelX = m_xHook - m_x;
    double TravelY = m_yHook - m_y;
    double Length = std::sqrt(std::pow(TravelX, 2) + std::pow(TravelY, 2));
    if (Length != 0.0) {
        TravelX /= Length;
        TravelY /= Length;
    }

    if (!m_HookGrabbedWall) {
        const double MaxLength = 300.0;
        if (Length > MaxLength) {
            m_xHook = m_x + TravelX * MaxLength;
            m_yHook = m_y + TravelY * MaxLength;
            m_xvelHook -= TravelX * 2.0;
            m_yvelHook -= TravelY * 2.0;
        }

        // Hook snaps to wall - fix later cus ugly
        if (m_xHook < 0.0) {
            m_xHook = 0.0;
            m_xvelHook = 0.0;
            m_yvelHook = 0.0;
            m_HookGrabbedWall = true;
        } else if (m_yHook < 0.0) {
            m_yHook = 0.0;
            m_xvelHook = 0.0;
            m_yvelHook = 0.0;
            m_HookGrabbedWall = true;
        } else if (m_xHook > m_World->Width()) {
            m_xHook = m_World->Width();
            m_xvelHook = 0.0;
            m_yvelHook = 0.0;
            m_HookGrabbedWall = true;
        } else if (m_yHook > m_World->Height()) {
            m_yHook = m_World->Height();
            m_xvelHook = 0.0;
            m_yvelHook = 0.0;
            m_HookGrabbedWall = true;
        }
    } else {
        m_xvel += TravelX * 1.5;
        m_yvel += TravelY * 1.5;
    }
}

void Character::TickWeapon() {
    if (!m_CurrentWeapon)
        return;

    m_CurrentWeapon->Tick();

    // if (m_Weapon == WEAPON_GLOCK) m_Glock.Tick();
    // else if (m_Weapon == WEAPON_SHOTGUN) m_Shotgun.Tick();
    // else if (m_Weapon == WEAPON_BURST) m_Burst.Tick();
    // else if (m_Weapon == WEAPON_MACHINEGUN) m_Minigun.Tick();
    //auto CurrentTick = m_World->CurrentTick();
    //if(m_BurstShots && CurrentTick - m_BurstTick > 5) {
    //    m_BurstTick = CurrentTick;
    //    m_BurstShots--;
    //    new Bullets(m_World, m_x, m_y, m_xLook * 10, m_yLook * 10);
    //    m_xvel += -m_xLook * 2;
    //    m_yvel += -m_yLook * 2;
    //}
//
    //if (!m_Shoot) {
    //    m_MachinegunTick -= 0.5;
    //    if (m_MachinegunTick < 0.0)
    //        m_MachinegunTick = 0.0;
    //    return;
    //}
//
    //if (m_Weapon == WEAPON_NONE)
    //    return;
//
//
    //if (m_Weapon == WEAPON_GLOCK) {
    //    if (CurrentTick - m_LastShot < 24)
    //        return;
    //    m_LastShot = CurrentTick;
    //    new Bullets(m_World, m_x, m_y, m_xLook * 10, m_yLook * 10);
    //    m_xvel += -m_xLook * 10;
    //    m_yvel += -m_yLook * 10;
//
    //} else if (m_Weapon == WEAPON_BURST) {
    //    if (CurrentTick - m_LastShot < 48)
    //        return;
//
    //    m_LastShot = CurrentTick;
    //    m_BurstShots = m_StartBurstShots - 1;
    //    m_BurstTick = CurrentTick;
//
    //    new Bullets(m_World, m_x, m_y, m_xLook * 10, m_yLook * 10);
    //    m_xvel += -m_xLook*2;
    //    m_yvel += -m_yLook*2;
    //}
    //else if (m_Weapon == WEAPON_SHOTGUN) {
    //    if (CurrentTick - m_LastShot < 72)
    //        return;
//
    //    m_LastShot = CurrentTick;
    //    const int bullet_count = 8;
    //    const double spread_angle = 75.0/180.0*M_PI;
    //    const double spacing = spread_angle / bullet_count;
    //    double radians =  atan2(m_yLook, m_xLook);
    //    double start_angle = radians - spread_angle/2;
    //    double end_angle = radians + spread_angle/2;
//
    //    for(double current_angle=start_angle; current_angle<end_angle; current_angle += spacing){
    //        double direction_x = cos(current_angle);
    //        double direction_y = sin(current_angle);
    //        double speed_multi = (double((rand()%20)-10))/10 + 10;
    //        new Bullets(m_World, m_x, m_y,  direction_x * speed_multi, direction_y * speed_multi);
    //    }
    //    m_xvel += -m_xLook * 30;
    //    m_yvel += -m_yLook * 30;
//
    //}
    //else if (m_Weapon == WEAPON_MACHINEGUN) {
    //    if (m_MachinegunTick > 15) m_MachinegunTick = 15;
    //    if (CurrentTick - m_LastShot < 10+15 - int(m_MachinegunTick))
    //        return;
    //    m_LastShot = CurrentTick;
    //    m_MachinegunTick += 1.4;
    //    new Bullets(m_World, m_x, m_y, m_xLook * 30, m_yLook * 30);
    //    m_xvel += -m_xLook * 4;
    //    m_yvel += -m_yLook * 4;
    //}
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

        //Reloads weapon on keyboard player with R button press
        if (currentEvent.key.keysym.scancode == SDL_SCANCODE_R && m_CurrentWeapon){
            m_CurrentWeapon->Reload();

            //Change later to single pointer to weapon
            // if(m_Weapon == WEAPON_GLOCK){
            //     m_Glock.Reload();
            // }
            // else if(m_Weapon == WEAPON_BURST){
            //     m_Burst.Reload();
            // }
            // else if(m_Weapon == WEAPON_MACHINEGUN){
            //     m_Minigun.Reload();
            // }
            // else if(m_Weapon == WEAPON_SHOTGUN){
            //     m_Shotgun.Reload();
            // }
        }

        for (int i = 0; i < NUM_CONTROLS; i++) {
            if (currentEvent.key.keysym.scancode == m_Controls[i])
                m_Movement[i] = State;
        }
    }
}

void Character::Tick() {
    TickControls();  // Do stuff depending on the current held buttons
    TickVelocity();  // Move the chracter entity
    TickWalls();
    TickHook();
    TickWeapon();

    m_LastShoot = m_Shoot;
    m_Shoot = false;  // Reset shooting at end of each tick
    m_LastHook = m_Hook;
    if(m_Health <= 0.0){delete this;}
}

void Character::Draw() {
    Clock* Timer = m_World->GameWindow()->Timer();
    Drawing* Render = m_World->GameWindow()->RenderClass();

    double Light = 0.5 + (std::sin(Timer->GetTotalTimeElapsed() - m_ExistsSince) + 1.0) / 4;
    SDL_Color Color = HSLtoRGB({ m_ColorHue, Light, 1.0 });

    if (m_HookDeployed) {
        Render->SetColor(Color.r, Color.g, Color.b, 255);
        Render->LineWorld(m_x, m_y, m_xHook, m_yHook);
    }

    SDL_FRect DrawRect = {float(m_x) - float(m_w/2),
                          float(m_y) - float(m_h/2),
                          float(m_w),
                          float(m_h)};
    Color = HSLtoRGB({ m_ColorHue, 1.0, Light });
    Render->SetColor(Color.r, Color.g, Color.b, 255);
    Render->FillRectFWorld(DrawRect);

    double XLook = m_x + m_xLook * 50.0;
    double YLook = m_y + m_yLook * 50.0;
    Color = HSLtoRGB({ m_ColorHue, 1.0 - Light, 1.0 });
    Render->SetColor(Color.r, Color.g, Color.b, 255);
    Render->LineWorld(int(m_x), int(m_y), int(XLook), int(YLook));

    if (m_World->NamesShown() <= 0.05)  // Visibility under 5% - don't render
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
