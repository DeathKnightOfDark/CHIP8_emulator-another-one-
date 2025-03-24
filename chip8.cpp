#include "chip8.h"

chip8_emulator::chip8_emulator()
{
	emulatorData = new uint8_t[4096];
	registers = new uint8_t[16];
	I = 0;
	PC = 0;
	delayTimer = 0;
	soundTimer = 0;

}

chip8_emulator::~chip8_emulator()
{

}