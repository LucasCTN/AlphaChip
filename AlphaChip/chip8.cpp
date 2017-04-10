#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "chip8.h"


chip8::chip8()
{
	unsigned char chip8_fontset[80] =
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
}


chip8::~chip8()
{
}

void chip8::initialize()
{
	this->pc		= 0x200;
	this->opcode	= 0;
	this->I			= 0;
	this->sp		= 0;

	// Clear display
	for (int i = 0; i < 64 * 32; i++)
	{
		gfx[i] = 0;
	}

	// Clear stack
	for (int i = 0; i < 16; i++)
	{
		stack[i] = 0;
	}

	// Clear registers V0-VF
	for (int i = 0; i < 16; i++)
	{
		key[i] = V[i] = 0;
	}

	// Clear memory
	for (int i = 0; i < 4096; i++)
	{
		memory[i] = 0;
	}

	// Load fontset
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	// Reset timers
	delay_timer = 0;
	sound_timer = 0;

	drawFlag = true;
	srand(time(NULL));
}

void chip8::emulateCycle()
{
	// Fetch opcode
	this->opcode = this->memory[this->pc] << 8 | this->memory[this->pc + 1];

	// Decode opcode
	switch (opcode & 0xF000)
	{
		case 0x0000: // (Multiple opcodes)
		{
			switch (opcode & 0x000F)
			{
				case 0x0000: // 0x00E0: Clears the screen        
							 // Execute opcode
				{
					for (int i = 0; i < 64 * 34; i++)
					{
						gfx[i] = 0x0;
					}

					pc += 2;
				}
				break;

				case 0x000E: // 0x00EE: Returns from subroutine          
							 // Execute opcode
				{
					--sp;
					pc = stack[sp];
					pc += 2;
				}
				break;

				default:
					printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			}
			break;
		}
		case 0x1000: // 1NNN: Jumps to address NNN.
					 // Execute opcode
		{
			pc = opcode & 0x0FFF;
			break;
		}
		case 0x2000: // 2NNN: Calls subroutine at NNN.
					 // Execute opcode
		{
			stack[sp] = pc;
			sp += 1;
			pc = opcode & 0x0FFF;
			break;
		}
		case 0x3000:	// 3XNN: Skips the next instruction if VX equals NN. 
						// (Usually the next instruction is a jump to skip a code block).
						// Execute opcode
		{
			if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		}
		case 0x4000:	// 4XNN: Skips the next instruction if VX doesn't equal NN.
						// (Usually the next instruction is a jump to skip a code block)
						// Execute opcode
		{
			if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
				pc += 4;
			else
				pc += 2;
			break;
		}
		case 0x5000:	// 5XY0: Skips the next instruction if VX equals VY.
						// (Usually the next instruction is a jump to skip a code block)
						// Execute opcode
		{
			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
				pc += 4;
			else
				pc += 2;
			break;
		}
		case 0x6000: // 6XNN: Sets VX to NN.
					 // Execute opcode
		{
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
			break;
		}
		case 0x7000: // 7XNN: Adds NN to VX.
					 // Execute opcode
		{
			V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
			pc += 2;
			break;
		}
		case 0x8000: // (Multiple opcodes)
		{
			switch (opcode & 0x000F)
			{
				case 0x0000: // 0x8XY0: Sets VX to the value of VY.        
							 // Execute opcode
				{
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				}
				case 0x0001: // 0x8XY1: Sets VX to VX or VY. (Bitwise OR operation) VF is reset to 0.        
							 // Execute opcode
				{
					V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x0F00) >> 8] || V[(opcode & 0x00F0) >> 4]);
					V[0xF] = 0;
					pc += 2;
					break;
				}
				case 0x0002: // 0x8XY2: Sets VX to VX and VY. (Bitwise AND operation) VF is reset to 0.        
							 // Execute opcode
				{
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
					V[0xF] = 0;

					pc += 2;
					break;
				}
				case 0x0003: // 0x8XY3: Sets VX to VX xor VY. VF is reset to 0.        
							 // Execute opcode
				{
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
					V[0xF] = 0;

					pc += 2;
					break;
				}
				case 0x0004: // 0x8XY4: Sets VX to the value of VY.        
							 // Execute opcode
				{
					if (V[(opcode & 0x00F0) >> 4] > 0xFF - V[(opcode & 0x0F00) >> 8])
						V[15] = 1; //carry
					else
						V[15] = 0;

					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					pc += 2;
					break;
				}
				case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.      
							 // Execute opcode
				{
					if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
						V[0xF] = 0;
					else
						V[0xF] = 1;

					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];

					pc += 2;
					break;
				}
				default:
					printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			}
			break;
		}
		case 0xC000: // CXNN: Sets VX to the result of a bitwise and operation on a random number 
					 // (Typically: 0 to 255) and NN.
					 // Execute opcode
		{
			V[(opcode & 0x0F00) >> 8] += ((rand() % 0xFF) & (opcode & 0x00FF));
			pc += 2;
			break;
		}
		case 0xA000: // ANNN: Sets I to the address NNN
					 // Execute opcode
		{
			I = opcode & 0x0FFF;
			pc += 2;
			break;
		}
		case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
					 // Execute opcode
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short height = (opcode & 0x000F);
			unsigned short pixel;

			V[0xF] = 0; // Cleaning the flag register
			for (int yLine = 0; yLine < height; yLine++)
			{
				pixel = memory[I + yLine];
				for (int xLine = 0; xLine < 8; xLine++)
				{
					if ((pixel & (0x80 >> xLine)) != 0)
					{
						if (gfx[(x + xLine + ((y + yLine) * 64))] == 1)
							V[0xF] = 1;
						gfx[x + xLine + ((y + yLine) * 64)] ^= 1;
					}
				}
			}

			drawFlag = true;
			pc += 2;
			break;
		}
		case 0xE000: // (Multiple opcodes)
		{
			switch (opcode & 0x000F)
			{
				case 0x000E: // 0xEX9E: Skips the next instruction if the key stored in VX is pressed.
							 // (Usually the next instruction is a jump to skip a code block)
							 // Execute opcode
				{
					if (key[V[(opcode & 0x0F00) >> 8]] != 0)
						pc += 4;
					else
						pc += 2;

					break;
				}

				case 0x0001: // 0xEXA1: Skips the next instruction if the key stored in VX isn't pressed. 
							 // (Usually the next instruction is a jump to skip a code block)
							 // Execute opcode
				{
					if (key[V[(opcode & 0x0F00) >> 8]] == 0)
						pc += 4;
					else
						pc += 2;
				}
				break;

				default:
					printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
					break;
			}
			break;
		}
		case 0xF000: // (Multiple opcodes)
		{
			switch (opcode & 0x000F)
			{
				case 0x0003: // 0xFX33: Stores the Binary-coded decimal representation of VX at the addresses I,
							 //											I plus 1, and I plus 2.
							 // Execute opcode
				{
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
					pc += 2;
					break;
				}
				case 0x0007: // 0xFX07: Sets VX to the value of the delay timer.
							 // Execute opcode
				{
					V[(opcode & 0x0F00) >> 8] = delay_timer;
					pc += 2;
					break;
				}
				case 0x0008: // 0xFX18: Sets the sound timer to VX.
							 // Execute opcode
				{
					sound_timer = (opcode & 0x0F00) >> 8;
					pc += 2;
					break;
				}
				case 0x0009: // 0xFX29: Sets I to the location of the sprite for the character in VX. 
							 //			Characters 0-F (in hexadecimal) are represented by a 4x5 font.
							 // Execute opcode
				{
					I = V[(opcode & 0x0F00) >> 8] * 0x5;
					pc += 2;
					break;
				}
				case 0x0005: //Multiple opcodes
				{
					switch (opcode & 0x00F0)
					{
						case 0x0010: // FX15: Sets the delay timer to VX.
									 // Execute opcode
						{
							delay_timer = V[(opcode && 0x0F00) >> 8];
							pc += 2;
							break;
						}
						case 0x0050: // FX55: Stores V0 to VX (including VX) in memory starting at address I.
									 // Execute opcode
						{
							for (int j = 0; j < (opcode & 0x0F00) >> 8; j++)
							{
								memory[I + j] = V[j];
							}
							pc += 2;
							printf("Faltou  I=I+X+1");
							break;
						}
						case 0x0060: // FX65: Fills V0 to VX (including VX) with values from memory starting at address I.
									 // Execute opcode
						{
							for (int j = 0; j < (opcode & 0x0F00) >> 8; j++)
							{
								V[j] = memory[I + j];
							}
							pc += 2;
							printf("Faltou  I=I+X+1");
							break;
						}
						default:
							printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
					}
				}
				case 0x000E: // 0xFX1E: Adds VX to I
							 //	VF is set to 1 when there is a range overflow (I+VX>0xFFF), and to 0 when there isn't.
							 // Execute opcode
				{

					if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
						V[0xF] = 1;
					else
						V[0xF] = 0;

					I += V[(opcode & 0x0F00) >> 8];
					pc += 2;
					break;
				}
				default:
					printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			}
			break;
		}
		default:
			printf("Unknown opcode: 0x%X\n", opcode);
			break;
	}

	// Update timers
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			printf("BEEP!\n");
			printf("\7");
		--sound_timer;
	}
}

void chip8::loadGame(std::string new_filename)
{
	FILE *fileptr;
	char *buffer;
	long filelen;

	fileptr = fopen(new_filename.c_str(), "rb");	// Open the file in binary mode
	fseek(fileptr, 0, SEEK_END);					// Jump to the end of the file
	filelen = ftell(fileptr);						// Get the current byte offset in the file
	rewind(fileptr);								// Jump back to the beginning of the file

	buffer = (char *)malloc((filelen + 1) * sizeof(char));	// Enough memory for file + \0
	fread(buffer, filelen, 1, fileptr);						// Read in the entire file
	fclose(fileptr);										// Close the file

	std::memcpy(memory + 0x200, buffer, 4096 - 0x200); // Rom data starts at 0x200
}

void chip8::setKeys()
{
	//throw std::exception("not implemented");
}