#include "graphics.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <stdexcept>

Graphics::Graphics(const std::string& title, int window_width,
                   int window_height)
    : width{window_width}, height{window_height} {
    // initialize SDL
    int result = SDL_Init(SDL_INIT_VIDEO);
    if (result < 0) {
        std::cout << SDL_GetError() << "\n";
    }
    // Create window
    window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, window_width,
                              window_height, 0);
    // Errors
    if (!window) {
        std::cout << SDL_GetError() << "\n";
    }
    // Create renderer
    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

Graphics::~Graphics() {
    // clean up: release SDL resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Graphics::clear() {
    // clear the screen by painting it black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void Graphics::draw(const SDL_Rect& rect, const Color& color) {
    // draw a filled rectangle
    SDL_SetRenderDrawColor(renderer, color.red, color.green, color.blue,
                           color.alpha);
    SDL_RenderFillRect(renderer, &rect);
}

void Graphics::update() {
    // show the current canvas on the screen
    SDL_RenderPresent(renderer);
}
