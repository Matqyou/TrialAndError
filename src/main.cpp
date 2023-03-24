#define SDL_MAIN_HANDLED
#include "GameReference.h"
#include "GameWorld.h"
#include "technical stuff/GameControllers.h"
#include "entities/Character.h"
#include "entities/Bullets.h"
#include <vector>
#include <iostream>

SDL_DisplayMode dm;
GameReference* GameWindow;
GameWorld* World;
GameControllers* Controllers;
SDL_Texture* TextTexture;

bool Initialize() {
    srand(time(nullptr));
    GameWindow = new GameReference();
    if (!GameWindow->Initialize())
        return false;

    World = new GameWorld(GameWindow, 1000, 1000);
    World->SetPaused(true);

    TextManager* TextHandler = GameWindow->TextHandler();
    SDL_Renderer* Renderer = GameWindow->Renderer();

    TTF_Font* Font1 = TextHandler->LoadFont("GROBOLD.ttf", 16);
    TextTexture = TextHandler->Render(Font1, "get out or -.. .. .", { 255, 255, 255 });

    Controllers = new GameControllers();
    new Character(World, 500, -100);
    return true;
}


// Function to handle controller events
void handleControllerEvent(SDL_Event event)
{
    if (event.type == SDL_CONTROLLERBUTTONDOWN) {
        std::cout << "Button " << static_cast<int>(event.cbutton.button) << " on controller " << static_cast<int>(event.cbutton.which) << " was pressed" << std::endl;
    } else if (event.type == SDL_CONTROLLERBUTTONUP) {
        std::cout << "Button " << static_cast<int>(event.cbutton.button) << " on controller " << static_cast<int>(event.cbutton.which) << " was released" << std::endl;
    } else if (event.type == SDL_CONTROLLERAXISMOTION) {
        std::cout << "Axis " << static_cast<int>(event.caxis.axis) << " on controller " << static_cast<int>(event.caxis.which) << " moved to " << static_cast<int>(event.caxis.value) << std::endl;
    }
}


int main() {
    if (!Initialize()) {
        std::printf("Terminating..");
        exit(1);
    }

    SDL_Window* Window = GameWindow->Window();
    SDL_Renderer* Renderer = GameWindow->Renderer();
    Clock* Timer = GameWindow->Timer();

    // Load the PNG images
    SDL_Surface* connected = IMG_Load("chain.png");
    if (!connected)
    {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return 1;
    }
    SDL_Surface* disconnected = IMG_Load("dis_chain.png");
    if (!disconnected)
    {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return 1;
    }
    SDL_Surface* Icon = IMG_Load("PS4_Controller_Icon.png");
    if (!Icon)
    {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
        return 1;
    }

    SDL_Texture * texture_connected = SDL_CreateTextureFromSurface(Renderer, connected);
    SDL_Texture * texture_disconnected = SDL_CreateTextureFromSurface(Renderer, disconnected);
    SDL_Texture * texture_Icon = SDL_CreateTextureFromSurface(Renderer, Icon);

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
                        Running = false;
                    // else if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_F11)
                    //     SDL_SetWindowFullscreen(Window, !(SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN));
                } break;
                case SDL_CONTROLLERDEVICEADDED: {
                    int DeviceID = CurrentEvent.cdevice.which;
                    GameController* CurrentController = Controllers->OpenController(DeviceID);
                    auto* NewPlayer = new Character(World, 100, 100); // Add new player
                    NewPlayer->SetGameController(CurrentController);
                } break;
                case SDL_CONTROLLERDEVICEREMOVED: {
                    int InstanceID = CurrentEvent.cdevice.which;
                    GameController* DeletedController = Controllers->CloseController(InstanceID);
                    Character* CorrespondingPlayer = nullptr;

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
        SDL_SetRenderDrawColor(Renderer, 120, 200, 120, 255);
        SDL_RenderClear(Renderer);

        World->Draw();

        SDL_Rect DestinationRect;
        SDL_QueryTexture(TextTexture, nullptr, nullptr, &DestinationRect.w, &DestinationRect.h);
        DestinationRect.x = 0;
        DestinationRect.y = GameWindow->Height() - DestinationRect.h;
        SDL_RenderCopy(Renderer, TextTexture, nullptr, &DestinationRect);

        if (World->Paused()) {
            // start
            SDL_SetRenderDrawColor(Renderer, 90, 20, 20, 255);
            SDL_RenderFillRect(Renderer, &startButtonRect);
            // setting
            //SDL_SetRenderDrawColor(Renderer, 0, 80, 40, 255);
            //SDL_RenderFillRect(Renderer, &settingsButtonRect);

            SDL_Rect connected = { 120, 375, 80, 44 };
            SDL_Rect disconnected = { 200, 375, 80, 44 };
            SDL_Rect Icon = { 100, 400, 200, 109 };

            SDL_RenderCopy(Renderer, texture_connected, NULL, &connected);
            SDL_RenderCopy(Renderer, texture_disconnected, NULL, &disconnected);
            SDL_RenderCopy(Renderer, texture_Icon, NULL, &Icon);
        }

        SDL_RenderPresent(Renderer);
        Timer->Tick();
    }
    SDL_DestroyTexture(texture_connected);
    SDL_DestroyTexture(texture_disconnected);
    SDL_DestroyTexture(texture_Icon);
    SDL_FreeSurface(connected);
    SDL_FreeSurface(disconnected);
    SDL_FreeSurface(Icon);
    delete Controllers;
    delete World;
    delete GameWindow;
    return 0;
}
