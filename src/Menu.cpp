//
// Created by Mim on 6/5/2023.
//

#include "Menu.h"
GameReference* Window;
AssetsManager* AssetsHandler;
SoundManager* SoundHandler;
Menu::Menu(GameReference* GameWindow,Drawing* Render, Texture* TextureStart, int width, int height){
    TextureButtonStart = TextureStart;
    Renderer = Render;
    Window = GameWindow;
    m_Height = height;
    m_Width = width;
    AssetsHandler = Window->Assets();
    SoundHandler = AssetsHandler->SoundHandler();
    GameWindow->Render()->SetWorld(reinterpret_cast<GameWorld *>(this));
    StartButtonRect = { int(GameWindow->GetWidth2()) - 150,
                                 int(GameWindow->GetHeight2()) - 200,
                                 300, 100 };

}
void Menu::Draw() {

}