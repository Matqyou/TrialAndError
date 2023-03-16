#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>

SDL_Window* Window;
SDL_Renderer* Renderer;

bool Initialize() {
    int Result = SDL_Init(SDL_INIT_EVERYTHING);
    if (Result) {
        std::printf("Error while initializing %s", SDL_GetError());
        return false;
    }

    Window = SDL_CreateWindow("TrialAndError", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, 0);
    if (!Window) {
        std::printf("Error while creating the window %s", SDL_GetError());
        return false;
    }

    Renderer = SDL_CreateRenderer(Window, -1, 0);
    if (!Renderer) {
        std::printf("Error while creating the renderer %s", SDL_GetError());
        return false;
    }

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
            switch (CurrentEvent.type) {
                case SDL_QUIT: {
                    Running = false;
                } break;
                case SDL_KEYDOWN: {
                    if (CurrentEvent.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
                        Running = false;
                } break;
            }
        }

        SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
        SDL_RenderClear(Renderer);
        SDL_RenderPresent(Renderer);
    }

    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    return 0;
}
