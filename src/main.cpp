#define SDL_MAIN_HANDLED
#include "GameReference.h"
#include "GameWorld.h"
#include "technical stuff/GameControllers.h"
#include "entities/Character.h"
#include "entities/Bullets.h"
#include <vector>
#include <iostream>

GameReference* GameWindow;
GameWorld* World;
GameControllers* Controllers;
Texture* TextTexture;

bool Initialize() {
    srand(time(nullptr));
    GameWindow = new GameReference();
    if (!GameWindow->Initialize())
        return false;

    World = new GameWorld(GameWindow, 1000, 1000);
    World->SetCameraPos(World->Width() / 2.0, World->Height() / 2.0);
    GameWindow->RenderClass()->SetWorld(World);

    TextManager* TextHandler = GameWindow->TextHandler();
    TTF_Font* Font1 = TextHandler->LoadFont("GROBOLD.ttf", 16);
    TextTexture = TextHandler->Render(Font1, "get out or -.. .. .", { 255, 255, 255 });

    Controllers = new GameControllers();
    new Character(World, 30, 30, 10, 10);
    return true;
}


int main() {
    if (!Initialize()) {
        std::printf("Terminating..");
        exit(1);
    }

    Clock* Timer = GameWindow->Timer();
    Drawing* Draw = GameWindow->RenderClass();
    ImageManager* ImageHandler = GameWindow->ImageHandler();

    // Load the PNG images
    Texture* TextureConnected = ImageHandler->LoadTexture("assets/chain.png");
    Texture* TextureDisconnected = ImageHandler->LoadTexture("assets/dis_chain.png");
    Texture* TextureIcon = ImageHandler->LoadTexture("assets/PS4_Controller_Icon.png");
    Texture* Vignette = ImageHandler->LoadTexture("assets/vignette.png");
    SDL_SetTextureAlphaMod(Vignette->SDLTexture(), 200);

    SDL_Rect ConnectedRect = { 120, 375, 80, 44 };
    SDL_Rect DisconnectedRect = { 200, 375, 80, 44 };
    SDL_Rect IconRect = { 100, 400, 200, 109 };

    // Render the Start button
    SDL_Rect startButtonRect = { 350, 100, 250, 60 };
    // Render the Settings button
    //SDL_Rect settingsButtonRect = { 350, 200, 250, 60 };

    bool Running = true;
    while (Running) {
        // Input and events
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent)) {
            GameWindow->Event(CurrentEvent);
            World->Event(CurrentEvent);
            Controllers->Event(CurrentEvent);

            switch (CurrentEvent.type) {
                case SDL_QUIT: {
                    Running = false;
                } break;
                case SDL_KEYDOWN: {
                    if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                        World->SetPaused(!World->Paused());
                    // else if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_F11)
                    //     SDL_SetWindowFullscreen(Window, !(SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN));
                } break;
                case SDL_CONTROLLERDEVICEADDED: {
                    int DeviceID = CurrentEvent.cdevice.which;
                    GameController* CurrentController = Controllers->OpenController(DeviceID);
                    auto* NewPlayer = new Character(World, 30, 30, 10, 10); // Add new player
                    NewPlayer->SetGameController(CurrentController);
                } break;
                case SDL_CONTROLLERDEVICEREMOVED: {
                    int InstanceID = CurrentEvent.cdevice.which;
                    GameController* DeletedController = Controllers->CloseController(InstanceID);

                    World->DestroyPlayerByController(DeletedController);
                } break;
                case SDL_MOUSEBUTTONDOWN: {
                    if (CurrentEvent.button.button == SDL_BUTTON_LEFT)
                    {
                        int x = CurrentEvent.button.x;
                        int y = CurrentEvent.button.y;
                        if (x >= startButtonRect.x && x < startButtonRect.x + startButtonRect.w &&
                            y >= startButtonRect.y && y < startButtonRect.y + startButtonRect.h)
                        {
                            World->SetPaused(false);
                        }
                        //else if (x >= settingsButtonRect.x && x < settingsButtonRect.x + settingsButtonRect.w &&
                        //    y >= settingsButtonRect.y && y < settingsButtonRect.y + settingsButtonRect.h)
                        //{
                        //    std::cout << "Settings" << std::endl;
                        //}
                    }
                } break;
            }
        }

        // Ticking
        World->Tick();

        // Drawing
        Draw->SetColor(120, 0, 120, 255);
        Draw->Clear();

        World->Draw();

        Draw->RenderTextureFullscreen(Vignette->SDLTexture(), nullptr);

        SDL_Rect DestinationRect;
        TextTexture->Query(nullptr, nullptr, &DestinationRect.w, &DestinationRect.h);
        DestinationRect.x = 0;
        DestinationRect.y = GameWindow->Height() - DestinationRect.h;
        Draw->RenderTexture(TextTexture->SDLTexture(), nullptr, DestinationRect);

        if (World->Paused()) {
            Draw->SetBlendingMode(SDL_BLENDMODE_BLEND);
            Draw->SetColor(0, 0, 0, 200);
            Draw->BlendFullscreen();
            Draw->SetBlendingMode(SDL_BLENDMODE_NONE);

            // start
            Draw->SetColor(90, 20, 20, 255);
            Draw->FillRect(startButtonRect);
            // setting
            //SDL_SetRenderDrawColor(Renderer, 0, 80, 40, 255);
            //SDL_RenderFillRect(Renderer, &settingsButtonRect);

            Draw->RenderTexture(TextureConnected->SDLTexture(), nullptr, ConnectedRect);
            Draw->RenderTexture(TextureDisconnected->SDLTexture(), nullptr, DisconnectedRect);
            Draw->RenderTexture(TextureIcon->SDLTexture(), nullptr, IconRect);
        }

        Draw->UpdateWindow();
        Timer->Tick();
    }
    delete Controllers;
    delete World;
    delete GameWindow;
    return 0;
}
