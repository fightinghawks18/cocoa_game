#include <iostream>

#include <SDL3/SDL.h>

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error("Failed to start SDL3");
    }

    bool gameRun = true;

    SDL_Window* window = SDL_CreateWindow("Cocoa", 800, 600, SDL_WINDOW_VULKAN);
    while (gameRun) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                gameRun = false;
                break;
            }
        }
    }

    std::cout << "Hello World!" << std::endl;

    SDL_Quit();
    return 0;
}
