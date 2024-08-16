# Chip8 Emulator Instructions

Created a functional chip8 emulator capable of running classic games like Pong. Implemented core components such as opcode handling, memory management, and graphical rendering.

## Required Files

Make sure you have the SDL2 library installed. You will need the lib, include, and bin folders for the SDL2 library. Place SDL2 Folders in your present working directory

Also Download a test ROM to run

## Commands

```
g++ -o chip8 main.cpp chip8.cpp platform.cpp -I./include -L./lib -lSDL2 -o

chip8.exe 10 3 test_opcode.ch8 chip8.exe
```

## Acknowledgements

Resources used:

**_[CodeSlinger](http://www.codeslinger.co.uk/pages/projects/chip8/examples.html)_**\
**_[Austin Morlan](https://austinmorlan.com/posts/chip8_emulator/#16-input-keys)_**\
**_[Wiki](https://en.wikipedia.org/wiki/CHIP-8)_**\
**_[Laurence Muller](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)_**
