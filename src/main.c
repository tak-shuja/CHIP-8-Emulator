#include <stdio.h>
#include <SDL2/SDL.h>
#include "include/instructions.h"
#include "include/cpu.h"
#include "include/screen.h"
#include "include/io.h"
#include <stdlib.h>
#include <time.h>


int main(int argc, char **argv)
{

    // if argc < 2, then show help text

    if (argc < 2)
    {
        printf("Usage: <program name> <path to rom file.ch8>\n");
        return 1;
    }

    const char *rom_filename = argv[1];

    // Seed the random number generator to get a true random number
    srand(time(NULL));

    CPU cpu;
    reset(&cpu);


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