// FONTS START ADDRESS AMBIGUOUS
#include <stdio.h>
#include "chip8.h"

void reset()
{
	for(uint16_t i=0 ; i<4096 ; i++)
		MEMORY[i] = 0;
	for(uint8_t r=0 ; r<16 ; r++)
		V[r] = 0;
	I = 0;
	PC = 0;
	for(uint8_t i=0 ; i<16 ; i++)
		STACK[i] = 0;
	SP = 0;
	OPCODE = 0;
	for(uint16_t i=0 ; i<2048 ; i++)
		VIDEO[i] = 0;
	for(uint8_t i=0 ; i<16 ; i++)
		KEYPAD[i] = 0;
	DT = 0;
	DF = false;
	CC = 0;
	srand(time(NULL));
	for(uint16_t i=0 ; i<=80 ; i++)
		MEMORY[i] = FONTS[i];
	PC = 0x200;
}

void loadROM(char const *ROM)
{
	FILE *ROMptr;
	ROMptr = fopen(ROM, "rb");
	if(ROMptr == NULL)
	{
		fprintf(stderr, "Unable to open file for reading\n");
		exit(EXIT_FAILURE);
	}
	fseek(ROMptr, 0, SEEK_END);
	uint16_t ROMptr_length = ftell(ROMptr);
	fseek(ROMptr, 0, SEEK_SET);
	char *buffer = malloc(ROMptr_length);
	fread(buffer, 1, ROMptr_length, ROMptr);
	fclose(ROMptr);
	for(uint16_t i=0 ; i<ROMptr_length ; i++)
	{
		MEMORY[0x200+i] = buffer[i];
	}
	free(buffer);
}

void cycle()
{
	OPCODE = (MEMORY[PC] << 8u) | MEMORY[PC+1];
	PC = PC + 2;
	dex[(OPCODE & 0xF000u) >> 12u]();
	CC++;
	// HANDLE DELAY TIMER
	if(DT > 0)
	{
		if(CC >= 16)
		{
			--DT;
			CC = 0;
		}
	}
}

void SYS_INVAL()
{
	printf("Invalid OPCODE %x\n", OPCODE);
	exit(EXIT_FAILURE);
}

void SYS_00E0()
{
	memset(VIDEO, 0, sizeof(VIDEO));
	DF = true;
}

void SYS_00EE()
{
	PC = STACK[SP];
	SP--;
}

void SYS_1nnn()
{
	uint16_t nnn = (OPCODE & 0x0FFFu);
	PC = nnn;
}

void SYS_2nnn()
{
	SP++;
	STACK[SP] = PC;
	uint16_t nnn = (OPCODE & 0x0FFFu);
	PC = nnn;
}

// ENSURE SIZED COMPARISON
void SYS_3xkk()
{
	uint8_t x  = (OPCODE & 0x0F00u) >> 8u;
	uint8_t kk = (OPCODE & 0x00FFu);
	if(V[x] == kk)
		PC = PC + 2;
}

// ENSURE SIZED COMPARISION
void SYS_4xkk()
{
	uint8_t x  = (OPCODE & 0x0F00u) >> 8u;
	uint8_t kk = (OPCODE & 0x00FFu);
	if(V[x] != kk)
		PC = PC + 2;
}

void SYS_5xy0()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	if(V[x] == V[y])
		PC = PC + 2;
}

void SYS_6xkk()
{
	uint8_t x  = (OPCODE & 0x0F00u) >> 8u;
	uint8_t kk = (OPCODE & 0x00FFu);
	V[x] = kk;
}

void SYS_7xkk()
{
	uint8_t x  = (OPCODE & 0x0F00u) >> 8u;
	uint8_t kk = (OPCODE & 0x00FFu);
	V[x] = V[x] + kk;
}

void SYS_8xy0()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
       	V[x] = V[y];
}

void SYS_8xy1()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	V[x] = (V[x] | V[y]);
}

void SYS_8xy2()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	V[x] = (V[x] & V[y]);
}

void SYS_8xy3()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	V[x] = (V[x] ^ V[y]);
}

// REBUG
void SYS_8xy4()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	if((x+y) > 255u)
		V[0xF] = 1;
	else
		V[0xF] = 0;
	V[x] = (V[x] + V[y]);
}

void SYS_8xy5()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	if(V[x] > V[y])
		V[0xF] = 1;
	else
		V[0xF] = 0;
	V[x] = (V[x] - V[y]);
}

// REBUG
void SYS_8xy6()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	V[0xF] = (V[x] & 0x10u);
	V[x] = (V[x] >> 1);
}

void SYS_8xy7()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	if(V[y] > V[x])
		V[0xF] = 1;
	else
		V[0xF] = 0;
	V[x] = (V[y] - V[x]);
}

// REBUG
void SYS_8xyE()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	V[0xF] = (V[x] & 0x80u) >> 7u;
	V[x] = (V[x] << 1);
}

void SYS_9xy0()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	if(V[x] != V[y])
		PC = PC+2;
}

void SYS_Annn()
{
	uint16_t nnn = (OPCODE & 0x0FFFu);
	I = nnn;
}

void SYS_Bnnn()
{
	uint16_t nnn = (OPCODE & 0x0FFFu);
	PC = (V[0] + nnn);
}

void SYS_Cxkk()
{
	uint8_t x  = (OPCODE & 0x0F00u) >> 8u;
	uint8_t kk = (OPCODE & 0x00FFu);
	uint8_t prng_b = rand() & kk;
	if(prng_b == 32)
		prng_b = prng_b / 2;;
	V[x] = prng_b; 
}

void SYS_Dxyn()
{
	
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t y = (OPCODE & 0x00F0u) >> 4u;
	uint8_t n = (OPCODE & 0x000Fu);
	uint8_t xc = V[x];
	uint8_t yc = V[y];
	V[0xF] = 0;
	for(uint16_t i=0 ; i<n ; i++)
	{
		uint8_t sb = MEMORY[I + i];
		for(uint16_t col=0 ; col<8 ; col++)
		{
			uint8_t sp = sb & (0x80u >> col);
			
			uint8_t yyPos = yc + i;
			uint8_t xxPos = xc + col;
			xxPos = xxPos%64;
			yyPos = yyPos%32;

			uint32_t *spp = &VIDEO[(yyPos) * 64 + (xxPos)];
			if(sp)
			{
				if(*spp == 0xFFFFFFFF)
					V[0xF] = 1;

				*spp ^= 0xFFFFFFFF;
			}
		}
	}
	DF = true;
}

void SYS_Ex9E()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t k = V[x];
	if(KEYPAD[k])
		PC = PC + 2;
}

void SYS_ExA1()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t k = V[x];
	if(!KEYPAD[k])	
		PC = PC + 2;
}

void SYS_Fx07()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	V[x] = DT;
}

void SYS_Fx0A()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	bool flag = false;
	for(int k=0 ; k<16; k++)
		if(KEYPAD[k])
		{
			V[x] = k;
			flag = true;
			break;
		}
	if(flag == false)
		PC = PC - 2;
}

void SYS_Fx15()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	DT = V[x];
}

void SYS_Fx18()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	// soundTIMER ignored here
}

void SYS_Fx1E()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	I = I + V[x];
}

// REBUG
void SYS_Fx29()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t digit = V[x];
      	I = (5 * digit);
}

void SYS_Fx33()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	uint8_t number = V[x];
	MEMORY[I + 2] = number % 10;
	number = number / 10;
	MEMORY[I + 1] = number % 10;
	number = number / 10;
	MEMORY[I + 0] = number % 10;
}

void SYS_Fx55()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	for(uint8_t r=0 ; r<=x ; r++)
		MEMORY[I + r] = V[r];
}

void SYS_Fx65()
{
	uint8_t x = (OPCODE & 0x0F00u) >> 8u;
	for(uint8_t r=0 ; r<=x ; r++)
		V[r] = MEMORY[I + r];
}

void DEX_0()
{
	dex_0[OPCODE & 0x000Fu]();
}

void DEX_8()
{
	dex_8[OPCODE & 0x000Fu]();
}

void DEX_E()
{
	dex_e[OPCODE & 0x000Fu]();
}

void DEX_F()
{
	dex_f[OPCODE & 0x00FFu]();
}

FPT dex[16] = { DEX_0, 	  SYS_1nnn, SYS_2nnn, SYS_3xkk,
	       	SYS_4xkk, SYS_5xy0, SYS_6xkk, SYS_7xkk,
		DEX_8,    SYS_9xy0, SYS_Annn, SYS_Bnnn,
		SYS_Cxkk, SYS_Dxyn, DEX_E,    DEX_F };

FPT dex_0[15] = { SYS_00E0,  SYS_INVAL, SYS_INVAL, SYS_INVAL,
		  SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		  SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		  SYS_INVAL, SYS_INVAL, SYS_00EE };

FPT dex_8[15] = { SYS_8xy0,  SYS_8xy1,  SYS_8xy2,  SYS_8xy3,
		  SYS_8xy4,  SYS_8xy5,  SYS_8xy6,  SYS_8xy7,
		  SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		  SYS_INVAL, SYS_INVAL, SYS_8xyE };

FPT dex_e[15] = { SYS_INVAL,  SYS_ExA1,  SYS_INVAL, SYS_INVAL,
		  SYS_INVAL,  SYS_INVAL, SYS_INVAL, SYS_INVAL,
		  SYS_INVAL,  SYS_INVAL, SYS_INVAL, SYS_INVAL,
		  SYS_INVAL,  SYS_INVAL, SYS_Ex9E };

FPT dex_f[102] = { SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_Fx07,
		   SYS_INVAL, SYS_INVAL, SYS_Fx0A,  SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_Fx15,  SYS_INVAL, SYS_INVAL,
		   SYS_Fx18,  SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_Fx1E,  SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_Fx29,  SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_Fx33,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_Fx55,  SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_INVAL, SYS_INVAL, SYS_INVAL,
		   SYS_INVAL, SYS_Fx65 };

void refresh_screen(const void *pixels, int pitch)
{
	SDL_UpdateTexture(Texture, NULL, pixels, pitch);
	SDL_RenderClear(Renderer);
	SDL_RenderCopy(Renderer, Texture, NULL, NULL);
	SDL_RenderPresent(Renderer);
}

bool input(uint8_t *KEYPAD)
{
	bool quit = false;
	SDL_Event event;
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						quit = true;
						break;
					
					case SDLK_x:
						KEYPAD[0] = 1;
						break;

					case SDLK_1:
						KEYPAD[1] = 1;
						break;

					case SDLK_2:
						KEYPAD[2] = 1;
						break;
					
					case SDLK_3:
						KEYPAD[3] = 1;
						break;

					case SDLK_q:
						KEYPAD[4] = 1;
						break;

					case SDLK_w:
						KEYPAD[5] = 1;
						break;

					case SDLK_e:
						KEYPAD[6] = 1;
						break;

					case SDLK_a:
						KEYPAD[7] = 1;
						break;

					case SDLK_s:
						KEYPAD[8] = 1;
						break;

					case SDLK_d:
						KEYPAD[9] = 1;
						break;

					case SDLK_z:
						KEYPAD[0xA] = 1;
						break;

					case SDLK_c:
						KEYPAD[0xB] = 1;
						break;

					case SDLK_4:
						KEYPAD[0xC] = 1;
						break;

					case SDLK_r:
						KEYPAD[0xD] = 1;
						break;

					case SDLK_f:
						KEYPAD[0xE] = 1;
						break;

					case SDLK_v:
						KEYPAD[0xF] = 1;
						break;
				}
			} break;

			case SDL_KEYUP:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_x:
						KEYPAD[0] = 0;
						break;

					case SDLK_1:
						KEYPAD[1] = 0;
						break;

					case SDLK_2:
						KEYPAD[2] = 0;
						break;

					case SDLK_3:
						KEYPAD[3] = 0;
						break;

					case SDLK_q:
						KEYPAD[4] = 0;
						break;

					case SDLK_w:
						KEYPAD[5] = 0;
						break;

					case SDLK_e:
						KEYPAD[6] = 0;
						break;

					case SDLK_a:
						KEYPAD[7] = 0;
						break;

					case SDLK_s:
						KEYPAD[8] = 0;
						break;

					case SDLK_d:
						KEYPAD[9] = 0;
						break;

					case SDLK_z:
						KEYPAD[0xA] = 0;
						break;

					case SDLK_c:
						KEYPAD[0xB] = 0;
						break;

					case SDLK_4:
						KEYPAD[0xC] = 0;
						break;

					case SDLK_r:
						KEYPAD[0xD] = 0;
						break;

					case SDLK_f:
						KEYPAD[0xE] = 0;
						break;

					case SDLK_v:
						KEYPAD[0xF] = 0;
						break;
				}
			} break;
		}
	}
	return quit;
}

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("Usage: %s ROM\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	reset();
	char const *ROM = argv[1];
	loadROM(ROM);
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL Initialization failed!\n");
		exit(EXIT_FAILURE);
	}
	Window = SDL_CreateWindow("myCHIP-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, 0);
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
	Texture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	int pitch = sizeof(VIDEO[0]) * 64;
	bool quit = false;
	while(!quit)
	{
		quit = input(KEYPAD);
		cycle();
		SDL_Delay(1);
		if(DF == true)
		{
			refresh_screen(VIDEO, pitch);
			DF = false;
		}
	}
	return 0;
}
