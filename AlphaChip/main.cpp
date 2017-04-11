#include <iostream>
#include <SDL.h>
#include "chip8.h"

chip8 emulator;

void setupGraphics();
void setupInput();
void drawGraphics();

int main(int argc, char ** argv)
{
	bool quit = false;
	int scale = 10;

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = NULL;
	window = SDL_CreateWindow("AlphaChip", 100, 100, 64 * scale, 32 * scale, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

	if (window == NULL)
	{
		std::cout << "Window could not be created!" << std::endl;
		return 0;
	}

	SDL_Renderer* renderer = NULL;
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	// Set render color to black ( background )
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	// Clear window
	SDL_RenderClear(renderer);

	SDL_Event* mainEvent = new SDL_Event();

	setupGraphics();
	setupInput();

	emulator.initialize();
	emulator.loadGame("C:\\chip8-roms\\pong");

	
	while (!quit)
	{
		emulator.emulateCycle();
		//printf("Reading opcode: [0x%X]\n", emulator.opcode);
		//printf("The pc is at: [0x%X]\n", emulator.pc);

		if (emulator.drawFlag)
		{
			//emulator.debugDraw();
			for (int i = 0; i < 64 * 32; i++)
			{
				if(emulator.gfx[i] == 1)
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				else if (emulator.gfx[i] == 0)
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

				int column = i % 64;
				int row = (i - column) / 64;

				SDL_Rect r;
				r.x = column * scale;
				r.y = row * scale;
				r.w = 1 * scale;
				r.h = 1 * scale;

				SDL_RenderFillRect(renderer, &r);
			}

			SDL_PollEvent(mainEvent);
			SDL_RenderPresent(renderer);			
			SDL_RenderClear(renderer);
		}			

		switch (mainEvent->type)
		{
		case SDL_KEYDOWN:
			switch (mainEvent->key.keysym.sym)
			{
			case SDLK_1:
				emulator.key[0] = 1;
				break;
			case SDLK_2:
				emulator.key[1] = 1;
				break;
			case SDLK_3:
				emulator.key[2] = 1;
				break;
			case SDLK_4:
				emulator.key[0xC] = 1;
				break;
			case SDLK_q:
				emulator.key[4] = 1;
				break;
			case SDLK_w:
				emulator.key[5] = 1;
				break;
			case SDLK_e:
				emulator.key[6] = 1;
				break;
			case SDLK_r:
				emulator.key[0xD] = 1;
				break;
			case SDLK_a:
				emulator.key[8] = 1;
				break;
			case SDLK_s:
				emulator.key[9] = 1;
				break;
			case SDLK_d:
				emulator.key[10] = 1;
				break;
			case SDLK_f:
				emulator.key[0xE] = 1;
				break;
			case SDLK_z:
				emulator.key[12] = 1;
				break;
			case SDLK_x:
				emulator.key[13] = 1;
				break;
			case SDLK_c:
				emulator.key[14] = 1;
				break;
			case SDLK_v:
				emulator.key[0xF] = 1;
				break;
			case SDLK_ESCAPE:
				quit = true;
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (mainEvent->key.keysym.sym)
			{
			case SDLK_1:
				emulator.key[0] = 0;
				break;
			case SDLK_2:
				emulator.key[1] = 0;
				break;
			case SDLK_3:
				emulator.key[2] = 0;
				break;
			case SDLK_4:
				emulator.key[0xC] = 0;
				break;
			case SDLK_q:
				emulator.key[4] = 0;
				break;
			case SDLK_w:
				emulator.key[5] = 0;
				break;
			case SDLK_e:
				emulator.key[6] = 0;
				break;
			case SDLK_r:
				emulator.key[0xD] = 0;
				break;
			case SDLK_a:
				emulator.key[8] = 0;
				break;
			case SDLK_s:
				emulator.key[9] = 0;
				break;
			case SDLK_d:
				emulator.key[10] = 0;
				break;
			case SDLK_f:
				emulator.key[0xE] = 0;
				break;
			case SDLK_z:
				emulator.key[12] = 0;
				break;
			case SDLK_x:
				emulator.key[13] = 0;
				break;
			case SDLK_c:
				emulator.key[14] = 0;
				break;
			case SDLK_v:
				emulator.key[0xF] = 0;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	delete mainEvent;

	return 0;
}

void setupGraphics()
{	
}

void setupInput()
{
}

void drawGraphics()
{

}