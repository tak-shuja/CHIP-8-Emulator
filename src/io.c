#include "include/io.h"
#include <stdio.h>

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


void print_keys(CPU *cpu)
{
  printf("-------- CPU KEYS ---------\n");
  for (uint8_t i = 0; i <= 0xF; i++)
  {
    printf("K[%d]: 0x%X\n", i, cpu->key[i]);
  }
  printf("---------------------------\n");
}


void print_timers(CPU *cpu)
{
  printf("-------- CPU TIMERS ---------\n");
  printf("Delay Timer: %d\nSound Timer: %d\n", cpu->delay_timer, cpu->sound_timer);
  printf("---------------------------\n");
}

void print_stats(CPU *cpu)
{
  printf("-------- CPU STATS --------\n");
  printf("SP: 0x%X\nPC: 0x%X\nI: 0x%X\n", cpu->sp, cpu->pc, cpu->I);
  printf("---------------------------\n");
}

void print_registers(CPU *cpu)
{
  printf("-------- CPU REGISTERS --------\n");
  for (uint8_t i = 0; i <= 0xF; i++)
  {
    printf("V[%X]:\t0x%X\n", i, cpu->V[i]);
  }
  printf("---------------------------\n");
}
