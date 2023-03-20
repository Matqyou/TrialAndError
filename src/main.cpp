#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "technical stuff/Clock.h"
#include "technical stuff/TextManager.h"
#include "technical stuff/GameControllers.h"
#include "entities/Character.h"
#include <vector>
#include <iostream>
SDL_DisplayMode dm;
SDL_Window* Window;
SDL_Renderer* Renderer;
Clock* Timer;
TextManager* TextHandler;
GameControllers* Controllers;
int width = 900;
int height = 700;

std::vector<Character*> Players;
SDL_Texture* TextTexture;

bool Initialize() {
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if (Result) {
        std::printf("Error while initializing main %s\n", SDL_GetError());
        return false;
    }
    int ImageFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    int ResultImage = IMG_Init(ImageFlags);
    if (ResultImage != ImageFlags) {
        std::printf("Error while initializing Image %s\n", SDL_GetError());
        return false;
    }
    int ResultTTF = TTF_Init();
    if (ResultTTF) {
        std::printf("Error while initializing TTF %s\n", SDL_GetError());
        return false;
    }

    Window = SDL_CreateWindow("TrialAndError", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, 0);
    if (!Window) {
        std::printf("Error while creating the window %s\n", SDL_GetError());
        return false;
    }

    Renderer = SDL_CreateRenderer(Window, -1, 0);
    if (!Renderer) {
        std::printf("Error while creating the renderer %s\n", SDL_GetError());
        return false;
    }
    // Initialize up to 3 controllers
    for (int i = 0; i < 3; i++) {
        SDL_GameControllerOpen(i);
    }
    Timer = new Clock(60);
    TextHandler = new TextManager();

    TTF_Font* Font1 = TextHandler->LoadFont("GROBOLD.ttf", 24);
    SDL_Surface* TempSurface = TTF_RenderText_Solid(Font1, "Text", SDL_Color{ 255, 255, 255, 255 });
    TextTexture = SDL_CreateTextureFromSurface(Renderer, TempSurface);
    SDL_FreeSurface(TempSurface);

    Controllers = new GameControllers();

    Players.push_back(new Character(Renderer, 100, 100));
    Players.push_back(new Character(Renderer, 200, 100));
    Players.push_back(new Character(Renderer, 300, 100));

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

    bool Running = true;
    while (Running) {
        SDL_Event CurrentEvent;
        while (SDL_PollEvent(&CurrentEvent)) {
            Controllers->Event(CurrentEvent);
            for (Character* Player : Players)
                Player->Event(CurrentEvent);

            switch (CurrentEvent.type) {
                case SDL_QUIT: {
                    Running = false;
                } break;
                case SDL_KEYDOWN: {
                    if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                        Running = false;
                    else if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_F11)
                        SDL_SetWindowFullscreen(Window, !(SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN));
                } break;
                case SDL_CONTROLLERDEVICEADDED: {
                    int DeviceID = CurrentEvent.cdevice.which;
                    GameController* CurrentController = Controllers->OpenController(DeviceID);
                    Players[0]->SetGameController(CurrentController);
                } break;
                case SDL_CONTROLLERDEVICEREMOVED: {
                    int InstanceID = CurrentEvent.cdevice.which;
                    Controllers->CloseController(InstanceID);
                } break;
            }
        }

        for (Character* Player : Players)
            Player->Tick();


        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);

        for (Character* Player : Players)
            Player->Draw();

        SDL_Rect DestinationRect;
        DestinationRect.x = 400;
        DestinationRect.y = 400;
        SDL_QueryTexture(TextTexture, nullptr, nullptr, &DestinationRect.w, &DestinationRect.h);
        SDL_RenderCopy(Renderer, TextTexture, nullptr, &DestinationRect);

        SDL_RenderPresent(Renderer);
        Timer->Tick();
    }
    for (Character* Player : Players)
        delete Player;
    delete Controllers;
    SDL_DestroyTexture(TextTexture);
    delete TextHandler;
    delete Timer;
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    for (int i = 0; i < 3; i++) {
        SDL_GameControllerClose(SDL_GameControllerFromInstanceID(i));
    }
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
