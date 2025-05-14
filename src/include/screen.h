#pragma once

#include <SDL2/SDL.h>



void init_video();
SDL_Window *create_window(const char*);
void clear_screen(CPU *cpu);