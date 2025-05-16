#include "include/constants.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include "include/cpu.h"

void init_sdl_video()
{
    if (SDL_Init(SDL_INIT_VIDEO) > 0)
    {
        printf("Failed to init video! Error: %s\n", SDL_GetError());
        return;
    }
}

SDL_Window *create_window(const char *title)
{
    return SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WIDTH * SCALE,
        HEIGHT * SCALE,
        SDL_WINDOW_SHOWN);
}

void clear_screen(CPU *cpu)
{
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            cpu->gfx[y][x] = 0;
        }
    }
}

void draw_gfx_memory_buffer(CPU *cpu, SDL_Renderer *renderer)
{
    // X and Y cooridnates
    uint8_t x, y;
    for (y = 0; y < HEIGHT; y++)
    {
        for (x = 0; x < WIDTH; x++)
        {

            if (cpu->gfx[y][x])
            {
                // pixel -> X, Y, Width, Height
                SDL_Rect pixel = {x * SCALE, y * SCALE, SCALE, SCALE};
                // create a rectangle
                SDL_RenderFillRect(renderer, &pixel);
            }
        }
    }
};