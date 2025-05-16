#include "include/cpu.h"
#include <stdio.h>
#include "include/instructions.h"
#include "include/constants.h"
#include "include/screen.h"
#include "include/io.h"
#include <stdlib.h>

uint8_t font_set[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void clear_memory(CPU *cpu)
{
    // clear the cpu memory
    for (int i = 0; i < 4096; i++)
    {
        cpu->memory[i] = 0;
    }
}

void load_fontset(CPU *cpu)
{
    // load fontset into the memory
    for (int i = 0; i < 80; i++)
    {
        cpu->memory[0x050 + i] = font_set[i];
    }
}

void set_counters(CPU *cpu)
{
    // 0x200: Program Start
    cpu->pc = 0x200;

    // set I and sp to 0.
    cpu->sp = 0;
    cpu->I = 0;
}

void clear_registers(CPU *cpu)
{
    // 0 all the registers
    for (int i = 0; i < 0x0F; i++)
    {
        cpu->V[i] = 0;
    }
}

void clear_gfx(CPU *cpu)
{
    // clear the gfx memory buffer
    for (int y = 0; y < HEIGHT; y++)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            cpu->gfx[y][x] = 0;
        }
    }
}

void clear_keys(CPU *cpu)
{
    // set all keys to false
    for (int i = 0; i < 0x10; i++)
    {
        cpu->key[i] = false;
    }
}

void reset_timers(CPU *cpu)
{
    cpu->delay_timer = 0;
    cpu->sound_timer = 0;
}

void reset(CPU *cpu)
{

    clear_memory(cpu);
    load_fontset(cpu);
    set_counters(cpu);
    clear_registers(cpu);
    clear_gfx(cpu);
    clear_keys(cpu);
    reset_timers(cpu);

    cpu->waitingForKeypress = false;
}

void push(CPU *cpu, uint16_t val)
{
    if (cpu->sp >= 16)
    {
        printf("Stack Overflow");
        return;
    }

    cpu->stack[cpu->sp] = val;
    cpu->sp++;
}

uint16_t pop(CPU *cpu)
{
    cpu->sp--;
    return cpu->stack[cpu->sp];
}

void print_stack(CPU *cpu)
{
    for (int i = 0; i < sizeof(cpu->stack) / sizeof(cpu->stack[0]); i++)
    {
        printf("0x%X\t", cpu->stack[i]);
    }
    printf("\n");
}

void draw_sprite(CPU *cpu, int reg_x, int reg_y, uint8_t height)
{

    cpu->V[0xF] = 0;
    for (int y = 0; y < height; y++)
    {
        // grab the sprite byte from the memory
        uint8_t sprite_byte = cpu->memory[cpu->I + y];
        for (int x = 0; x < 8; x++)
        {
            // grab each sprite pixel from the sprite byte
            // 1010 1010
            // to get the msb, we do 7-x. for lsb, only x is good enough.
            uint8_t sprite_pixel = (sprite_byte >> (7 - x)) & 1;

            // wrap around
            int px = (cpu->V[reg_x] + x) % 64;
            int py = (cpu->V[reg_y] + y) % 32;

            // if collision, set VF
            if (sprite_pixel && cpu->gfx[py][px])
            {
                cpu->V[0xF] = 1;
            }

            // xor to draw data
            cpu->gfx[py][px] ^= sprite_pixel;
        }
    }
}

void execute_8xy_opcode(CPU *cpu, int reg_x, int reg_y, int last_byte)
{
    switch (last_byte)
    {
    case 0:
        cpu->V[reg_x] = cpu->V[reg_y];
        break;
    case 1:
        cpu->V[reg_x] |= cpu->V[reg_y];
        cpu->V[0xF] = 0;
        break;
    case 2:
        cpu->V[reg_x] &= cpu->V[reg_y];
        cpu->V[0xF] = 0;
        break;
    case 3:
        cpu->V[reg_x] ^= cpu->V[reg_y];
        cpu->V[0xF] = 0;
        break;
    case 4:
    {
        uint8_t sum = cpu->V[reg_x] + cpu->V[reg_y];
        cpu->V[0xF] = (sum > 0xFF) ? 1 : 0;

        cpu->V[reg_x] += cpu->V[reg_y];
        break;
    }
    case 5:
        if (cpu->V[reg_x] >= cpu->V[reg_y])
        {
            cpu->V[0xF] = 1;
        }
        else
        {
            cpu->V[0xF] = 0;
        }
        cpu->V[reg_x] -= cpu->V[reg_y];
        break;
    case 6:
        cpu->V[0xF] = cpu->V[reg_x] & 0x1;
        cpu->V[reg_x] >>= 1;
        break;
    case 7:
        cpu->V[0xF] = (cpu->V[reg_y] >= cpu->V[reg_x]) ? 1 : 0;
        cpu->V[reg_x] = cpu->V[reg_y] - cpu->V[reg_x];
        break;
    case 0xE:
        cpu->V[0xF] = (cpu->V[reg_x] >> 7) & 0x1;
        cpu->V[reg_x] <<= 1;
        break;
    default:
        break;
    }
}

int fetch_instruction(CPU *cpu)
{
    return cpu->memory[cpu->pc] << 8 | cpu->memory[cpu->pc + 1];
}

void execute(CPU *cpu, int instruction, int *running)
{
    uint8_t reg_x, reg_y, reg;
    uint16_t address, value;
    uint8_t last_byte;
    bool increment_pc = true;

    reg = (instruction >> 8) & 0xF;
    reg_x = (instruction >> 8) & 0xF;
    reg_y = (instruction >> 4) & 0xF;

    address = instruction & 0x0FFF;

    switch (instruction)
    {
    case CLEAR:
        clear_screen(cpu);
        break;

    case RET:
        cpu->pc = pop(cpu);
        break;

    case HLT:
        *running = 0;
        break;

    default:
        switch (instruction & 0xF0FF)
        {
        case SKPVX:
            if (cpu->key[cpu->V[reg]])
                cpu->pc += 2;
            break;

        case SKPNVX:
            if (!cpu->key[cpu->V[reg]])
                cpu->pc += 2;
            break;

        case LDVXDT:
            cpu->V[reg] = cpu->delay_timer;
            break;

        case LDDTVX:
            cpu->delay_timer = cpu->V[reg];
            break;

        case LDSTDT:
            cpu->sound_timer = cpu->V[reg];
            break;

        case ADDIVX:

            cpu->I += cpu->V[reg];
            break;

        case LDFVX:

            value = cpu->V[reg];
            int index = value;
            index *= 5;             // each sprite is 5 bytes
            address = index + 0x50; // index + font base memory
            cpu->I = address;
            break;

        case STRBCD:

            value = cpu->V[reg];
            // save bcd into memory at index I, I+1 and I+2
            int ones, tens, hundreds;
            hundreds = value / 100;
            tens = (value / 10) % 10;
            ones = value % 10;
            cpu->memory[cpu->I] = hundreds;
            cpu->memory[cpu->I + 1] = tens;
            cpu->memory[cpu->I + 2] = ones;
            break;
        case STOR:

            for (uint8_t i = 0; i <= reg; i++)
            {
                cpu->memory[cpu->I + i] = cpu->V[i];
            }
            break;

        case LOAD:

            for (uint8_t i = 0; i <= reg; i++)
            {
                cpu->V[i] = cpu->memory[cpu->I + i];
            }
            break;

        case WAITKP:
            cpu->waitingForKeypress = true;

            for (int i = 0; i < 16; i++)
            {
                if (cpu->key[i])
                {
                    cpu->V[reg] = i;
                    cpu->waitingForKeypress = false;
                    break;
                }
            }
            break;

        default:
            switch (instruction & 0xF000)
            {
            case JMP:
                cpu->pc = instruction & 0x0FFF;
                increment_pc = false;
                break;

            case CALL:

                push(cpu, cpu->pc);
                cpu->pc = address;
                increment_pc = false;
                break;

            case SEB:

                value = instruction & 0xFF;
                if (cpu->V[reg] == value)
                    cpu->pc += 2;
                break;

            case SNEB:

                value = instruction & 0xFF;
                if (cpu->V[reg] != value)
                    cpu->pc += 2;
                break;

            case SER:
                if (cpu->V[reg_x] == cpu->V[reg_y])
                    cpu->pc += 2;
                break;

            case SNER:
                if (cpu->V[reg_x] != cpu->V[reg_y])
                    cpu->pc += 2;
                break;

            case LDR:

                value = instruction & 0xFF;
                cpu->V[reg] = value;
                break;

            case ADD:

                value = instruction & 0xFF;
                cpu->V[reg] += value;
                break;

            case SET:
                last_byte = instruction & 0xF;
                execute_8xy_opcode(cpu, reg_x, reg_y, last_byte);
                break;

            case LDI:
                cpu->I = instruction & 0x0FFF;
                break;

            case DRW:

                draw_sprite(cpu, reg_x, reg_y, instruction & 0xF);
                break;

            case JPV0:
                cpu->pc = address + cpu->V[0];
                increment_pc = false;
                break;

            case RNDVX:
                // reg = (instruction >> 8) & 0xF;
                cpu->V[reg] = (rand() % 256) & (instruction & 0xFF);
                break;

            default:
                printf("Unsupported instruction: 0x%.4X\n", instruction);
                *running = 0;
            }
        }
    }

    if (increment_pc)
        cpu->pc += 2;
}

void decode_and_execute(CPU *cpu, int *running)
{
    int instruction = cpu->memory[cpu->pc];
    instruction = fetch_instruction(cpu);
    execute(cpu, instruction, running);
}

void execute_cycle(CPU *cpu)
{
    int running = 1;
    if (running)
        decode_and_execute(cpu, &running);
}
