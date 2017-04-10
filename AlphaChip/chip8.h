#pragma once
class chip8
{
public:
	chip8();
	~chip8();
	void initialize();
	void emulateCycle();
	void loadGame(std::string filename);
	void setKeys();

	bool drawFlag;

	unsigned short opcode;
	unsigned char memory[4096];
	unsigned char V[16];	// General purpose registers. V[0xF] is a carry flag.
	unsigned short I;		// Index register, to store addresses
	unsigned short pc;		// Program counter

	unsigned char gfx[64 * 32];

	unsigned char delay_timer;
	unsigned char sound_timer;

	unsigned short stack[16];
	unsigned short sp;
	unsigned char key[16];

	unsigned char chip8_fontset[80];

	/* 0x000-0x1FF - Chip 8 interpreter (contains font set in emu) [0 - 511]
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F) [80 - 160]
	0x200-0xFFF - Program ROM and work RAM  [512 - 7777]*/
};

