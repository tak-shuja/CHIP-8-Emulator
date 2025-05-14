#include <stdio.h>
#include <SDL2/SDL.h>
#include "include/instructions.h"
#include "include/cpu.h"
#include "include/screen.h"
#include <stdlib.h>
#include <time.h>

void handleKeyInput(SDL_Event event, CPU *cpu)
{
    if (event.type == SDL_KEYDOWN) // Key Pressed
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_1:
            cpu->key[0x1] = true;
            break;
        case SDLK_2:
            cpu->key[0x2] = true;
            break;
        case SDLK_3:
            cpu->key[0x3] = true;
            break;
        case SDLK_4:
            cpu->key[0xC] = true;
            break;

        case SDLK_q:
            cpu->key[0x4] = true;
            break;
        case SDLK_w:
            cpu->key[0x5] = true;
            break;
        case SDLK_e:
            cpu->key[0x6] = true;
            break;
        case SDLK_r:
            cpu->key[0xD] = true;
            break;

        case SDLK_a:
            cpu->key[0x7] = true;
            break;
        case SDLK_s:
            cpu->key[0x8] = true;
            break;
        case SDLK_d:
            cpu->key[0x9] = true;
            break;
        case SDLK_f:
            cpu->key[0xE] = true;
            break;

        case SDLK_z:
            cpu->key[0xA] = true;
            break;
        case SDLK_x:
            cpu->key[0x0] = true;
            break;
        case SDLK_c:
            cpu->key[0xB] = true;
            break;
        case SDLK_v:
            cpu->key[0xF] = true;
            break;
        }
    }
    else if (event.type == SDL_KEYUP) // Key Released
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_1:
            cpu->key[0x1] = false;
            break;
        case SDLK_2:
            cpu->key[0x2] = false;
            break;
        case SDLK_3:
            cpu->key[0x3] = false;
            break;
        case SDLK_4:
            cpu->key[0xC] = false;
            break;

        case SDLK_q:
            cpu->key[0x4] = false;
            break;
        case SDLK_w:
            cpu->key[0x5] = false;
            break;
        case SDLK_e:
            cpu->key[0x6] = false;
            break;
        case SDLK_r:
            cpu->key[0xD] = false;
            break;

        case SDLK_a:
            cpu->key[0x7] = false;
            break;
        case SDLK_s:
            cpu->key[0x8] = false;
            break;
        case SDLK_d:
            cpu->key[0x9] = false;
            break;
        case SDLK_f:
            cpu->key[0xE] = false;
            break;

        case SDLK_z:
            cpu->key[0xA] = false;
            break;
        case SDLK_x:
            cpu->key[0x0] = false;
            break;
        case SDLK_c:
            cpu->key[0xB] = false;
            break;
        case SDLK_v:
            cpu->key[0xF] = false;
            break;
        }
    }
}

#define MAX_ROM_SIZE 4096 // Maximum size for the CHIP-8 memory

// Function to load a CHIP-8 ROM
int load_rom(const char *filename, uint8_t *memory)
{
    FILE *rom_file = fopen(filename, "rb");

    if (!rom_file)
    {
        perror("Failed to open ROM file");
        return -1;
    }

    // Get the total size of the rom
    fseek(rom_file, 0, SEEK_END);    // move the file pointer to EOF
    long rom_size = ftell(rom_file); // get the offset from start of file to file pointer
    fseek(rom_file, 0, SEEK_SET);    // move the file pointer to beginning of file again

    // check rom size and ensure it fits into chip-8 memory [4KB]
    if (rom_size > (MAX_ROM_SIZE - 0x200))
    {
        printf("Error: ROM is too large for CHIP-8 memory!\n");
        fclose(rom_file);
        return -1;
    }

    // Read the ROM data into memory starting from address 0x200
    // fread -> (buffer to be copied to, no of bytes read per count, total bytes, input stream)
    size_t bytes_read = fread(memory + 0x200, 1, rom_size, rom_file);
    if (bytes_read != rom_size)
    {
        printf("Error: Couldn't read the complete ROM file!\n");
        fclose(rom_file);
        return -1;
    }

    // close the rom file
    fclose(rom_file);
    printf("ROM loaded successfully!\n");

    return 0; // Success
}

int main(int argc, char **argv)
{

    // if argc < 2, then show help text

    if (argc < 2)
    {
        printf("Usage: <program name> <path to rom file.ch8>\n");
        return 1;
    }

    // Seed the random number generator
    srand(time(NULL));

    CPU cpu;
    reset(&cpu);

    const char *rom_filename = argv[1];

    if (load_rom(rom_filename, cpu.memory) == 0)
    {
        printf("ROM loaded to memory starting at address 0x200\n");
    }
    else
    {
        return 1;
    }

    SDL_Window *window = create_window("CHIP-8 Emulator");
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = 0;
            }
            handleKeyInput(event, &cpu);
        }

        if (cpu.waitingForKeypress)
        {
            handleKeyInput(event, &cpu);
            cpu.waitingForKeypress = false;
        }

        execute_cycle(&cpu); // Add a function to process one instruction cycle.

        // Update timers
        if (cpu.delay_timer > 0)
        {
            cpu.delay_timer--;
        }
        if (cpu.sound_timer > 0)
        {
            cpu.sound_timer--;
            if (cpu.sound_timer == 1)
            {
                // TODO: ADD BEEP SOUND HERE
            }
        }

        // clear the screen
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0x00, 0xDD, 0x30, 0xFF);
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                if (cpu.gfx[y][x])
                {
                    SDL_Rect pixel = {x * SCALE, y * SCALE, SCALE, SCALE};
                    SDL_RenderFillRect(renderer, &pixel);
                }
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }

    SDL_DestroyRenderer(renderer);

    SDL_DestroyWindow(window);
    SDL_Quit();

    // ? FOR DEBUGGING
    // print_registers(&cpu);
    // print_stats(&cpu);
    // print_timers(&cpu);
    return 0;
}