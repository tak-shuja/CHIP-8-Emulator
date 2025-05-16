#pragma once

#include <SDL2/SDL.h>

SDL_Window *create_window(const char *);
void clear_screen(CPU *cpu);
void draw_gfx_memory_buffer(CPU *, SDL_Renderer *);