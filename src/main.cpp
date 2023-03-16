#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* Window = SDL_CreateWindow("TrialAndError", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 600, 0);
    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, 0);

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

    return 0;
}
