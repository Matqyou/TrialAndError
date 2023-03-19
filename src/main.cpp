#define SDL_MAIN_HANDLED
#include <SDL.h>
#include "technical stuff/Clock.h"
#include "entities/Character.h"
#include <vector>
#include <iostream>

SDL_Window* Window;
SDL_Renderer* Renderer;
Clock* Timer;

std::vector<Character*> aPlayers;

bool Initialize() {
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if (Result) {
        std::printf("Error while initializing %s", SDL_GetError());
        return false;
    }

    Window = SDL_CreateWindow("TrialAndError", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              900, 700, 0);
    if (!Window) {
        std::printf("Error while creating the window %s", SDL_GetError());
        return false;
    }

    Renderer = SDL_CreateRenderer(Window, -1, 0);
    if (!Renderer) {
        std::printf("Error while creating the renderer %s", SDL_GetError());
        return false;
    }

    Timer = new Clock(60);
    aPlayers.push_back(new Character(Renderer, 100, 100));
    aPlayers.push_back(new Character(Renderer, 200, 100));
    aPlayers.push_back(new Character(Renderer, 300, 100));

    return true;
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
            for (Character* Player : aPlayers)
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
            }
        }

        for (Character* Player : aPlayers)
            Player->Tick();

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);

        for (Character* Player : aPlayers)
            Player->Draw();

        SDL_RenderPresent(Renderer);
        Timer->Tick();
    }

    for (Character* Player : aPlayers)
        delete Player;

    delete Timer;
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    return 0;
}
