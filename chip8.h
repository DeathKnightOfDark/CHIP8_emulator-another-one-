#pragma once

#ifndef CHIP_8_H
#define CHIP_8_H
#include <stack>
#include <iostream>
/// <summary>
/// Указатель на функцию без параметров для взаимодействия с экраном
/// </summary>
typedef void (*screenVoidFuncWithoutParameters) (void);
typedef void (*screenVoidFuncWithPixelPosition) (uint8_t x, uint8_t y);
typedef bool (*screenBoolFuncWithPixelPosition) (uint8_t x, uint8_t y);
#define SCREEN_PIXEL_WIDTH 64
#define SCREEN_PIXEL_HEIGHT 32

const uint8_t font[] = {
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

class chip8_emulator
{
private:
	/// <summary>
	/// emulator data
	/// </summary>
	uint8_t* emulatorData;

	/// <summary>
	/// Programm counter
	/// </summary>
	int PC;

	/// <summary>
	/// Index register
	/// </summary>
	uint16_t I;

	/// <summary>
	/// address stack
	/// </summary>
	std::stack<uint16_t> addrStack;

	/// <summary>
	/// delay timer
	/// </summary>
	uint8_t delayTimer;

	/// <summary>
	/// sound timer
	/// </summary>
	uint8_t soundTimer;

	/// <summary>
	/// registers
	/// </summary>
	uint8_t* registers;
	/// <summary>
	/// Указатель на функцию для очистки экрана
	/// </summary>
	screenVoidFuncWithoutParameters clearScreen;
	//screenVoidFuncWithPixelPosition reversePixelOnScreen;
	screenVoidFuncWithPixelPosition reversePixelOnScreen;
	screenBoolFuncWithPixelPosition getPixelOnScreen;
	
public:
	chip8_emulator();
	~chip8_emulator();
	void executeInstruction(uint16_t opcode);
	void set_clearScreen(screenVoidFuncWithoutParameters func);
	void set_reversePixelOnScreen(screenVoidFuncWithPixelPosition func);
	void set_getPixelOnScreen(screenBoolFuncWithPixelPosition func);
	void loadProgrammToMemory(const uint16_t* source, size_t size);
	void runNextInstruction();
};
#endif // !CHIP_8_H


