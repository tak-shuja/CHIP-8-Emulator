#pragma once

#include "cpu.h"
#include <SDL2/SDL.h>

void handleKeyInput(SDL_Event, CPU *);
int load_rom(const char *, uint8_t *);

void print_keys(CPU *);

void print_timers(CPU *);

void print_stats(CPU *cpu);
void print_registers(CPU *cpu);
