//
// Created by edijs on 16.04.2023.
//

#include "menu.h"
#include "SDL.h"
#include "../..//technical stuff/TextManager.h"
#include <iostream>

// Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const auto choice = 0;
// Function prototypes
void drawMainMenu(SDL_Renderer* renderer);
void handleInput(SDL_Event& e, int& choice);

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create window and renderer
    SDL_Window* window = SDL_CreateWindow("Main Menu", SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                          SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Set background color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Event loop flag
    bool quit = false;

    // Current choice in the menu
    int choice = 0;

    // Event loop
    while (!quit) {
        // Handle events on queue
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            // Handle menu input
            handleInput(e, choice);
        }

        // Clear screen
        SDL_RenderClear(renderer);

        // Draw main menu
        drawMainMenu(renderer);

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

void drawMainMenu(SDL_Renderer* renderer) {
    // Create font and surface
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Main Menu", {255, 255, 255});

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // Set texture coordinates
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - surface->w / 2, 50, surface->w, surface->h};

    // Render texture
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    // Destroy font, surface, and texture
    TTF_CloseFont(font);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Create character skins menu item
    surface = TTF_RenderText_Solid(font, "Character Skins", {255, 255, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    textRect = {SCREEN_WIDTH / 2 - surface->w / 2, 150, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Create guns menu item
    surface = TTF_RenderText_Solid(font, "Guns", {255, 255, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    textRect = {SCREEN_WIDTH / 2 - surface->w / 2, 200, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Create input menu item
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    textRect = {SCREEN_WIDTH / 2 - surface->w / 2, 250, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Create controller option
    surface = TTF_RenderText_Solid(font, "Controller", {255, 255, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    textRect = {SCREEN_WIDTH / 2 - surface->w / 2, 300, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Create keyboard option
    surface = TTF_RenderText_Solid(font, "Keyboard", {255, 255, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    textRect = {SCREEN_WIDTH / 2 - surface->w / 2, 350, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    // Create selector arrow
    surface = TTF_RenderText_Solid(font, ">", {255, 255, 255});
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    textRect = {SCREEN_WIDTH / 2 - 100, 300 + choice * 50, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void handleInput(SDL_Event& e, int& choice) {
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_UP:
                choice = (choice - 1 + 3) % 3; // Wrap around to last item
                break;

            case SDLK_DOWN:
                choice = (choice + 1) % 3; // Wrap around to first item
                break;

            case SDLK_RETURN:
                switch (choice) {
                    case 0:
                        std::cout << "Selected character skins.\n";
                        break;

                    case 1:
                        std::cout << "Selected guns.\n";
                        break;

                    case 2:
                        std::cout << "Selected input.\n";
                        break;
                }
                break;
        }
    }
}
