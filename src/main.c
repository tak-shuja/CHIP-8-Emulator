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
        printf("Usage: ./out/main <path to rom file.ch8>\n");
        return 1;
    }

    const char *rom_filename = argv[1];

    // Seed the random number generator to get a true random number
    srand(time(NULL));

    CPU cpu;
    reset(&cpu);

    // Try loading the rom into the memory
    if (load_rom(rom_filename, cpu.memory) == 0)
    {
        printf("ROM loaded to memory starting at address 0x200\n");
    }
    else
    {
        return 1;
    }

    // create a pointer to window and renderer
    SDL_Window *window = create_window("CHIP-8 Emulator");
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int running = 1;
    const int cycles_per_frame = 10;
    SDL_Event event;

    while (running)
    {
        // time at start of the frame
        uint32_t frame_start = SDL_GetTicks();
        // SDL2 main event loop
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

        // to achieve 600 cycles per second, we must emulate
        // 10 cycles per frame (60fps)
        // so each frame will have 10 executions
        for (int i = 0; i < cycles_per_frame; i++)
        {
            execute_cycle(&cpu);
        }

        // Update timers once per frame
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

        // set active pixel color to terminal green
        SDL_SetRenderDrawColor(renderer, 0x00, 0xDD, 0x30, 0xFF);

        // draw the contents of memory buffer onto the screen
        draw_gfx_memory_buffer(&cpu, renderer);

        // Update the screen to reflect changes
        SDL_RenderPresent(renderer);

        // how long this frame took
        uint32_t frame_time = SDL_GetTicks() - frame_start;

        if (frame_time < FRAME_DELAY)
        {
            // Wait to match 60fps
            SDL_Delay(FRAME_DELAY - frame_time);
        }
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