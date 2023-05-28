//
// Created by Mim on 5/8/2023.
//
#include "Crates.h"
#include "character/Character.h"
#include <iostream>
#include <random>
Texture* Crates::ms_TextureBox = nullptr;
Texture* Crates::ms_TextureBreakingBox1 = nullptr;
Texture* Crates::ms_TextureBreakingBox2 = nullptr;
Sound* Crates::ms_HitSound = nullptr;
Sound* Crates::ms_BoxSound = nullptr;

Crates::Crates(GameWorld* world, double start_x, double start_y, double Health, int RandomDrop)
 : Entity(world, ENTITY_NORMAL, GameWorld::ENTTYPE_BOX, start_x, start_y, 50, 50, 0.0, 0.0, 0.95){
    m_World = world;
    m_Health = Health;
    m_Alive = true;
    m_Type = static_cast<DropType>(RandomDrop);
    m_Texture = &ms_TextureBox;
    // TODO make the randomiser in a seperate class
    std::random_device rd;
    std::mt19937 gen(rd());
    double interval[] = {0, 0, 1, 100};
    double weights[] = { .10, 0, .9};
    std::piecewise_constant_distribution<> dist(std::begin(interval),
                                                std::end(interval),
                                                weights);
    int RandomNumber = dist(gen);
    // Here it will set the an int value to typeID so it can later be sent into the errors so they spawn with a set drop
    if(RandomNumber < 20)  typeID = DISORIANTED;
    else if(RandomNumber < 40) { typeID = SPIKY; }
    else if(RandomNumber < 50) { typeID = CONFUSING_HP; }
    else if(RandomNumber < 60) { typeID = INVINCIBLE; }
    else if(RandomNumber < 70) { typeID = HEALERS_PARADISE; }
    else if(RandomNumber < 80) { typeID = RANGED; }
    else if(RandomNumber < 90) { typeID = SLOW_DOWN; }
    else if(RandomNumber < 100) { typeID = DANGEROUS_RECOIL; }
}

Crates::~Crates(){

}

void Crates::TickImpact(double x, double y) {
    auto Char = m_World->FirstPlayer();
    for (; Char; Char = (Character*)(Char->NextType())) {
        EntityCore* CharCore = Char->GetCore();
        bool Collides = (CharCore->m_x - 25 < x) &&
                        (CharCore->m_x + 25 > x) &&
                        (CharCore->m_y - 25 < y) &&
                        (CharCore->m_y + 25 > y);

        if (!Collides)
            continue;
    }
}

void Crates::DamageCrate(double Damage) {
    Sound *BoxHitSound = ms_BoxSound;
    m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(BoxHitSound);
    m_Health -= Damage;
    if(m_Health < 10)m_Texture = &ms_TextureBreakingBox2;
    else if(m_Health < 20)m_Texture = &ms_TextureBreakingBox1;
}

void Crates::Tick() {
    TickImpact(m_Core->m_x, m_Core->m_y);
    TickWalls();

    if (m_Health <= 0) {
        m_Alive = false;
        m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(ms_HitSound);
        if (m_Type != ERROR){
            srand (time(NULL));
            int Ammo_type = rand()%4;
            new Ammo(m_World, static_cast<AmmoType>(Ammo_type), m_Core->m_x, m_Core->m_y, 20);
        } else {
            new Error(m_World, m_Core->m_x, m_Core->m_y, typeID); // To change the drop just change typeID the enum value of whatever ERROR is needed
        }
    }
}

void Crates::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_FRect DrawRect = {float(m_Core->m_x) - float(m_Core->m_w / 2.0),
                          float(m_Core->m_y) - float(m_Core->m_h / 2.0),
                          float(m_Core->m_w),
                          float(m_Core->m_h)};

    Render->RenderTextureFWorld((*m_Texture)->SDLTexture(), nullptr, DrawRect);
}
