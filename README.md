Chip8 Emulator Instructions
Setup
Download a Test ROM from the Internet
Download the SDL2 Library:

Make sure you have the SDL2 library installed. You will need the lib, include, and bin folders for the SDL2 library.
Place SDL2 Folders:

Ensure that the lib, include, and bin folders are in your working directory.
Compilation
To compile the Chip8 emulator, use the following command:

g++ -o chip8 main.cpp chip8.cpp platform.cpp -I./include -L./lib -lSDL2
-o chip8 specifies the output executable name.
main.cpp chip8.cpp platform.cpp are the source files to compile.
-I./include specifies the directory containing the SDL2 header files.
-L./lib specifies the directory containing the SDL2 library files.
-lSDL2 links against the SDL2 library.
Running the Emulator
To run the Chip8 emulator, use the following command:

chip8.exe 10 3 test_opcode.ch8
chip8.exe is the name of the compiled executable.
10 and 3 are example arguments (these might be your screen width and height or other parameters based on your implementation).
test_opcode.ch8 is the ROM file to load and run.
