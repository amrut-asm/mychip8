#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <time.h>

typedef void (*FPT)();

extern FPT dex[16];
extern FPT dex_0[15];
extern FPT dex_8[15];
extern FPT dex_e[15];
extern FPT dex_f[102];

uint8_t MEMORY[4096];	// 4 KB Memory

uint8_t V[16];		// 16 8-bit registers

uint16_t I;		// Index Register

uint16_t PC;		// Progam Counter

uint16_t STACK[16];	// Stack 
uint8_t SP;		// Stack Pointer

uint16_t OPCODE;

uint32_t VIDEO[2048]; // 64x32 Display

uint8_t KEYPAD[16];	// Input Device

uint8_t DT;

uint8_t FONTS[80] = {	0xF0, 0x90, 0x90, 0x90, 0xF0, 	// 0
			0x20, 0x60, 0x20, 0x20, 0x70, 	// 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, 	// 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, 	// 3
			0x90, 0x90, 0xF0, 0x10, 0x10, 	// 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, 	// 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, 	// 6
			0xF0, 0x10, 0x20, 0x40, 0x40, 	// 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, 	// 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, 	// 9
			0xF0, 0x90, 0xF0, 0x90, 0x90,	 // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, 	// B
			0xF0, 0x80, 0x80, 0x80, 0xF0, 	// C
			0xE0, 0x90, 0x90, 0x90, 0xE0, 	// D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, 	// E
			0xF0, 0x80, 0xF0, 0x80, 0x80 }; // F  
						
SDL_Window *Window;
SDL_Renderer *Renderer;
SDL_Texture *Texture;

bool DF;

uint32_t CC;

void reset();

void loadROM(char const* filename);

uint8_t prng();

void cycle();

void SYS_INVAL();

void SYS_0nnn();

void SYS_00EE();

void SYS_1nnn();

void SYS_2nnn();

void SYS_3xkk();

void SYS_4xkk();

void SYS_5xy0();

void SYS_6xkk();

void SYS_7xkk();

void SYS_8xy0();

void SYS_8xy1();

void SYS_8xy2();

void SYS_8xy3();

void SYS_8xy4();

void SYS_8xy5();

void SYS_8xy6();

void SYS_8xy7();

void SYS_8xyE();

void SYS_9xy0();

void SYS_Annn();

void SYS_Bnnn();

void SYS_Cxkk();

void SYS_Dxyn();

void SYS_Ex9E();

void SYS_ExA1();

void SYS_Fx07();

void SYS_Fx0A();

void SYS_Fx15();

void SYS_Fx18();

void SYS_Fx1E();

void SYS_Fx29();

void SYS_Fx33();

void SYS_Fx55();

void SYS_Fx65();
