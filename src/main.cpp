#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include "stdint.h"
#include "SDL2/SDL.h"
#include "chip8.h"

using namespace std;

int main(int argc, char **argv)
{
    int      w     = 512;
    int      h     = 256;
    Chip8    chip8 = Chip8();
    uint32_t pixels[32 * 64];

    uint8_t keymap[16] = {
        SDLK_x, SDLK_1, SDLK_2, SDLK_3, SDLK_q, SDLK_w, SDLK_e, SDLK_a,
        SDLK_s, SDLK_d, SDLK_z, SDLK_c, SDLK_4, SDLK_r, SDLK_f, SDLK_v,
    };

    SDL_Window *window = NULL;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);
    SDL_Texture *sdlTexture =
        SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

load:
    std::string fname = "roms/BRIX";
    if (argc == 2)
        fname = argv[1];

    if (!chip8.load_rom(fname))
        exit(1);

    while (true) {
        chip8.run(false, false);

        if (chip8.get_draw_flag()) {
            Uint64 start = SDL_GetPerformanceCounter();
            chip8.set_draw_flag(false);
            for (int i = 0; i < 32 * 64; i++) {
                if (chip8.get_display_value(i) == 0) {
                    pixels[i] = 0xFF000000;
                } else {
                    pixels[i] = 0xFFFFFFFF;
                }
            }
            SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
            SDL_RenderPresent(renderer);

            Uint64 end       = SDL_GetPerformanceCounter();
            float  elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
            if (16.666f > elapsedMS)
                SDL_Delay(floor(16.666f - elapsedMS));
        }
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                exit(0);
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    exit(0);
                if (e.key.keysym.sym == SDLK_F1)
                    goto load;
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.set_keypad_value(i, 1);
                    }
                }
            }
            if (e.type == SDL_KEYUP) {
                for (int i = 0; i < 16; ++i) {
                    if (e.key.keysym.sym == keymap[i]) {
                        chip8.set_keypad_value(i, 0);
                    }
                }
            }
        }
    }
}