#pragma once

// 🧹 System / Flow Control
#define CLEAR 0x00E0 // Clear the display (screen)
#define RET 0x00EE   // Return from subroutine
#define HLT 0x0004   // Halt the execution

// 🔀 Program Control
#define JMP 0x1000   // Jump to address NNN (0x1NNN)
#define CALL 0x2000  // Call subroutine at NNN (0x2NNN)
#define JPV0 0xB000  // Jump to address NNN + V0 (0xBNNN)
#define RNDVX 0xC000 // Vx = random byte AND NN (0xCXNN)

// 🎮 Input
#define SKPVX 0xE09E  // Skip next instruction if key in Vx is pressed (0xEX9E)
#define SKPNVX 0xE0A1 // Skip next instruction if key in Vx is NOT pressed (0xEXA1)
#define WAITKP 0xF00A // Wait for keypress, store it in Vx (0xFX0A)

// 🎯 Conditional Skips
#define SEB 0x3000  // Skip if Vx == NN (0x3XNN)
#define SNEB 0x4000 // Skip if Vx != NN (0x4XNN)
#define SER 0x5000  // Skip if Vx == Vy (0x5XY0)
#define SNER 0x9000 // Skip if Vx != Vy (0x9XY0)

// 🗃️ Registers / Constants
#define LDR 0x6000 // Load NN into Vx (0x6XNN)
#define ADD 0x7000 // Add NN to Vx (0x7XNN)
#define SET 0x8000 // 8XY_ family: Vx = Vy, logic, arithmetic (0x8XY_ ops)
#define LDI 0xA000 // Set I = NNN (0xANNN)

// 🧮 Timers
#define LDVXDT 0xF007 // Vx = delay_timer (0xFX07)
#define LDDTVX 0xF015 // delay_timer = Vx (0xFX15)
#define LDSTDT 0xF018 // sound_timer = Vx (0xFX18)

// 🧠 Index Register Ops
#define ADDIVX 0xF01E // I += Vx (0xFX1E)
#define LDFVX 0xF029  // I = location of sprite for digit in Vx (0xFX29)
#define STRBCD 0xF033 // Store BCD of Vx in memory at I, I+1, I+2 (0xFX33)

// 🧷 Memory Transfer
#define STOR 0xF055 // Store V0 to Vx in memory starting at I (0xFX55)
#define LOAD 0xF065 // Load memory into V0 to Vx starting at I (0xFX65)

// 🎨 Graphics
#define DRW 0xD000 // Draw sprite at (Vx, Vy) with height N (0xDXYN)
