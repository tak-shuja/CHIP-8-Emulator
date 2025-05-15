#pragma once

#include <inttypes.h>
#include "./constants.h"

typedef struct
{
    uint16_t pc;
    uint16_t I;
    uint16_t sp;
    uint8_t V[16];
    uint8_t memory[MAX_ROM_SIZE];
    uint16_t stack[16];
    bool key[16];
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t gfx[HEIGHT][WIDTH];
    bool waitingForKeypress;

} CPU;

void reset(CPU *);
void print_stats(CPU *);
void push(CPU *, uint16_t);
uint16_t pop(CPU *);
void print_stack(CPU *);
void execute_cycle(CPU *);
void print_registers(CPU *);
void load_program(CPU *, uint8_t *, int);
void print_program(CPU *, int);
void print_keys(CPU *);
void print_timers(CPU *);