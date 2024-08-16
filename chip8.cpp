#include "chip8.hpp"
#include <cstring>  // For memset
#include <chrono>
#include <random>
#include <fstream>
#include <iostream> // For system clock

const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x050;
const unsigned int START_ADDRESS = 0x200;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;
uint8_t fontset[FONTSET_SIZE] =
    {
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

// Constructor definition
Chip8::Chip8()
    : randGen(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count())), 
      randByte(0, 255)
{
    // Initialize PC to the start address for ROM
    pc = START_ADDRESS;

    // Load the fontset into memory
    LoadFontset();
    
}

// Load ROM implementation
void Chip8::LoadROM(const char* filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open())
    {
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; i++)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
}

// Load fontset implementation
void Chip8::LoadFontset()
{
    for (unsigned int i = 0; i < FONTSET_SIZE; ++i)
    {
        memory[FONTSET_START_ADDRESS + i] =fontset[i];
    }
}

// Randomize register implementation
void Chip8::RandomizeRegister(uint8_t reg)
{
    if (reg < 16) // Ensure the register index is valid
    {
        registers[reg] = randByte(randGen);
    }
}

// clear screen
void Chip8::OP_00E0()
{
	memset(video, 0, sizeof(video));
}
//return from a subroutine
void Chip8::OP_00EE(){
    --sp;
    pc=stack[sp];
    
}
void Chip8::OP_NULL()
	{}
// jump to location nnn
void Chip8::OP_1nnn(){
    uint16_t jump=opcode & 0x0FFFu;
    pc=jump;

}

//2nnn: call subroutine at nnn
void Chip8::OP_2nnn(){
    uint16_t address=opcode & 0x0FFFu;
    stack[sp]=pc;
    ++sp;
    pc=address;
    
}
// skip next instructions if Vx=3kk
void Chip8::OP_3xkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	if (registers[Vx] == byte)
	{
		pc += 2;
	}
}

// 4xkk - SNE Vx, byte

// Skip next instruction if Vx != kk.
void Chip8::OP_4xkk(){
    uint8_t Vx= (opcode & 0x0F00u)>>8u;
    uint8_t bytes=opcode & 0x00FFu;
    if (registers[Vx]!=bytes){
        pc+=2;
    }
}
// 5xy0 - SE Vx, Vy
// Skip next instruction if Vx = Vy.
void Chip8::OP_5xy0(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t Vy=(opcode & 0x00F0u)>>4u;
    if (registers[Vx]==registers[Vy]){
        pc+=2;
    }
    
}
// 6xkk - LD Vx, byte
// Set Vx = kk.

void Chip8::OP_6xkk(){
    uint8_t Vx= (opcode & 0x0F00u) >> 8u;
    uint8_t byte=(opcode & 0x00FFu);
    registers[Vx]=byte;

}
// 7xkk - ADD Vx, byte
// Set Vx = Vx + kk.
void Chip8::OP_7xkk(){
    uint8_t Vx=(opcode & 0x0F00u)>> 8u;
    uint8_t byte=(opcode & 0x00FFu);

    registers[Vx]+=byte;
}

// 8xy0 - LD Vx, Vy
// Set Vx = Vy.
void Chip8::OP_8xy0(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t Vy=(opcode & 0x00F0u)>>4u;

    registers[Vx]=registers[Vy];
}
// 8xy1 - OR Vx, Vy
// Set Vx = Vx OR Vy.
void Chip8::OP_8xy1(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t Vy=(opcode & 0x00F0u)>> 4u;

    registers[Vx]=registers[Vx] | registers[Vy];    
}
// 8xy2 - AND Vx, Vy
// Set Vx = Vx AND Vy.
void Chip8::OP_8xy2(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t Vy=(opcode & 0x00F0u)>> 4u;

    registers[Vx]=registers[Vx] & registers[Vy];    
}
// 8xy3 - XOR Vx, Vy
// Set Vx = Vx XOR Vy.
void Chip8::OP_8xy3(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t Vy=(opcode & 0x00F0u)>> 4u;

    registers[Vx]=registers[Vx] ^ registers[Vy];  

    
}
// 8xy4 - ADD Vx, Vy
// Set Vx = Vx + Vy, set VF = carry.
void Chip8::OP_8xy4(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t Vy=(opcode & 0x00F0u)>> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if (sum > 255U) {
        registers[0xF]=1;
        // The value is greater than 255
    } else {
        registers[0xF]=0;
        // The value is 255 or less       
    }
    registers[Vx] = sum & 0xFFu;

}
void Chip8::OP_8xy5(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    // Subtract Vy from Vx
    uint16_t subtract = registers[Vx] - registers[Vy];

    // Set VF to 1 if there's no borrow (Vx > Vy), else 0
    if (registers[Vx] > registers[Vy]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    // Store the result in Vx, keep only the lowest 8 bits
    registers[Vx] = subtract & 0xFFu;
}


// 8xy6 - SHR Vx
// Set Vx = Vx SHR 1.
// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
// A right shift is performed (division by 2), and the least significant bit is saved in Register VF.
void Chip8::OP_8xy6(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
    // saving lsb 
    registers[0xF]=registers[Vx] & 0x01u;

    registers[Vx] >>= 1;
    
}
// 8xy7 - SUBN Vx, Vy
// Set Vx = Vy - Vx, set VF = NOT borrow.
// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
void Chip8::OP_8xy7(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx]=registers[Vy]-registers[Vx];
    if (registers[Vy]>registers[Vx]){
        registers[0xF]=1;
    }else{
        registers[0xF]=0;
    }
    
}
// 8xyE - SHL Vx {, Vy}
// Set Vx = Vx SHL 1.
// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
// A left shift is performed (multiplication by 2), and the most significant bit is saved in Register VF.

void Chip8::OP_8xyE(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    // Save the most-significant bit (MSB) of Vx before shifting
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u; // MSB is in the 8th bit (0x80u)
    // Perform the left shift operation (multiply by 2)
    registers[Vx] <<= 1;   
}

// 9xy0 - SNE Vx, Vy
// Skip next instruction if Vx != Vy.
// Since our PC has already been incremented by 2 in Cycle(), we can just increment by 2 again to skip the next instruction
void Chip8::OP_9xy0(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    
    if (registers[Vx]!=registers[Vy]){
        pc+=2;
    }
}
// Annn - LD I, addr
// Set I = nnn.
void Chip8::OP_Annn()
{
	uint16_t address = opcode & 0x0FFFu;

	index = address;
}
// Bnnn - JP V0, addr
// Jump to location nnn + V0.
void Chip8::OP_Bnnn()
{
	uint16_t address = opcode & 0x0FFFu;

	pc =registers[0]+ address;
}
// Cxkk - RND Vx, byte
// Set Vx = random byte AND kk.
void Chip8::OP_Cxkk()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = randByte(randGen) & byte;
}
// Dxyn - DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

void Chip8::OP_Dxyn()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint8_t height = opcode & 0x000Fu;

	// Wrap if going beyond screen boundaries
	uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

	registers[0xF] = 0;
    // go through each grid point and get the screen pixel to draw
    // check if the sprite pixel is already on
    // if yes set collision flag 
    // xor the screen pixel and spirte pixel

	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}
// Ex9E - SKP Vx
// Skip next instruction if key with the value of Vx is pressed.
void Chip8::OP_Ex9E(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];
    if (keypad[key]){
        pc+=2;
    }
}
// ExA1 - SKNP Vx
// Skip next instruction if key with the value of Vx is not pressed.
void Chip8::OP_ExA1(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];
    if (!keypad[key]){
        pc+=2;
    }
}
// Fx07 - LD Vx, DT
// Set Vx = delay timer value.
void Chip8::OP_Fx07(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx]=delayTimer;
	
}

// Fx0A - LD Vx, K
// Wait for a key press, store the value of the key in Vx.
// The easiest way to “wait” is to decrement the PC by 2 whenever a keypad value is not detected. This has the effect of running the same instruction repeatedly.
void Chip8::OP_Fx0A(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    
	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12])
	{
		registers[Vx] = 12;
	}
	else if (keypad[13])
	{
		registers[Vx] = 13;
	}
	else if (keypad[14])
	{
		registers[Vx] = 14;
	}
	else if (keypad[15])
	{
		registers[Vx] = 15;
	}
	else
	{
		pc -= 2;
	}
}
// Fx15 - LD DT, Vx
// Set delay timer = Vx.
void Chip8::OP_Fx15(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    delayTimer=registers[Vx];
}

// Fx18 - LD ST, Vx
// Set sound timer = Vx.
void Chip8::OP_Fx18(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    soundTimer=registers[Vx];
}
// Fx1E - ADD I, Vx
// Set I = I + Vx.
void Chip8::OP_Fx1E(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    index+=registers[Vx];
}
// Fx29 - LD F, Vx
// Set I = location of sprite for digit Vx.
void Chip8::OP_Fx29(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t digit= registers[Vx];
    index=FONTSET_START_ADDRESS+(5*digit);
}
// Fx33 - LD B, Vx
// Store BCD representation of Vx in memory locations I, I+1, and I+2.
// The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
void Chip8::OP_Fx33(){
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t value=registers[Vx];
    memory[index+2]=value%10;
    value/=10;
    memory[index+1]=value%10;
    value/=10;
    memory[index]=value%10;
}
//Store registers V0 through Vx in memory starting at location I.
void Chip8::OP_Fx55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		memory[index + i] = registers[i];
	}
}
// Fx65 - LD Vx, [I]
// Read registers V0 through Vx from memory starting at location I.
void Chip8::OP_Fx65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= Vx; ++i)
	{
		registers[i]=memory[index+i];
	}
}

void Chip8::Cycle() {
    // Fetch
    opcode = (memory[pc] << 8u) | memory[pc + 1];

    // Increment the PC before we execute anything
    pc += 2;

    // Decode and Execute using switch-case
    switch (opcode & 0xF000u) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: OP_00E0(); break; // Clear the display
                case 0x00EE: OP_00EE(); break; // Return from subroutine
                default: OP_NULL(); break; // Unknown opcode
            }
            break;

            case 0x1000: OP_1nnn(); break; // Jump to address nnn
            case 0x2000: OP_2nnn(); break; // Call subroutine at nnn
            case 0x3000: OP_3xkk(); break; // Skip next instruction if Vx == kk
            case 0x4000: OP_4xkk(); break; // Skip next instruction if Vx != kk
            case 0x5000: OP_5xy0(); break; // Skip next instruction if Vx == Vy
            case 0x6000: OP_6xkk(); break; // Set Vx = kk
            case 0x7000: OP_7xkk(); break; // Set Vx += kk

        case 0x8000:
            switch (opcode & 0x000Fu) {
                case 0x0000: OP_8xy0(); break; // Set Vx = Vy
                case 0x0001: OP_8xy1(); break; // Set Vx = Vx OR Vy
                case 0x0002: OP_8xy2(); break; // Set Vx = Vx AND Vy
                case 0x0003: OP_8xy3(); break; // Set Vx = Vx XOR Vy
                case 0x0004: OP_8xy4(); break; // Set Vx += Vy, set VF = carry
                case 0x0005: OP_8xy5(); break; // Set Vx -= Vy, set VF = NOT borrow
                case 0x0006: OP_8xy6(); break; // Set Vx = Vx SHR 1
                case 0x0007: OP_8xy7(); break; // Set Vx = Vy - Vx, set VF = NOT borrow
                case 0x000E: OP_8xyE(); break; // Set Vx = Vx SHL 1
                default: OP_NULL(); break; // Unknown opcode
            }
            break;

        case 0x9000: OP_9xy0(); break; // Skip next instruction if Vx != Vy
        case 0xA000: OP_Annn(); break; // Set I = nnn
        case 0xB000: OP_Bnnn(); break; // Jump to address nnn + V0
        case 0xC000: OP_Cxkk(); break; // Set Vx = random byte AND kk
        case 0xD000: OP_Dxyn(); break; // Draw sprite at (Vx, Vy)

        case 0xE000:
            switch (opcode & 0x00FFu) {
                case 0x009E: OP_Ex9E(); break; // Skip next instruction if key with value of Vx is pressed
                case 0x00A1: OP_ExA1(); break; // Skip next instruction if key with value of Vx is not pressed
                default: OP_NULL(); break; // Unknown opcode
            }
            break;

        case 0xF000:
            switch (opcode & 0x00FFu) {
                case 0x0007: OP_Fx07(); break; // Set Vx = delay timer value
                case 0x000A: OP_Fx0A(); break; // Wait for a key press, store the value of the key in Vx
                case 0x0015: OP_Fx15(); break; // Set delay timer = Vx
                case 0x0018: OP_Fx18(); break; // Set sound timer = Vx
                case 0x001E: OP_Fx1E(); break; // Set I += Vx
                case 0x0029: OP_Fx29(); break; // Set I = location of sprite for digit Vx
                case 0x0033: OP_Fx33(); break; // Store BCD representation of Vx in memory
                case 0x0055: OP_Fx55(); break; // Store registers V0 to Vx in memory
                case 0x0065: OP_Fx65(); break; // Read registers V0 to Vx from memory
                default: OP_NULL(); break; // Unknown opcode
            }
            break;

        default: OP_NULL(); break; // Unknown opcode
    }

    // Decrement the delay timer if it's been set
    if (delayTimer > 0) {
        --delayTimer;
    }

    // Decrement the sound timer if it's been set
    if (soundTimer > 0) {
        --soundTimer;
    }
}