#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "chip8.hpp"     // Include your Chip8 header
#include "platform.hpp"  // Include your Platform header
#include <string>
#include <windows.h>


int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <scale> <delay> <ROM>" << std::endl;
        return 1;
    }

    // Parse command-line arguments, take scale and delay
    int scale = std::stoi(argv[1]);
    int delay = std::stoi(argv[2]);
    // read the romfile from argument provided
    const char* romFile = argv[3];

    // Initialize Chip8 emulator
    Chip8 chip8;
    chip8.LoadROM(romFile);

    // Initialize SDL and create a window
    Platform platform("CHIP-8 Emulator", 64 * scale, 32 * scale, 64, 32);

    // Main loop
    bool quit = false;

    while (!quit) {
        // Process input
        quit = platform.ProcessInput(chip8.keypad);

        // Cycle the Chip8 emulator
        chip8.Cycle();

        // Update the display
        platform.Update(chip8.video, 64 * sizeof(uint32_t)); // Assuming video is a uint32_t array

        // Control the speed of the emulator
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    return 0;
}