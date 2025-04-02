#pragma once
#define DEBUG_MODE
#ifndef CHIP_8_H
#define CHIP_8_H

#include <iostream>
#include "stackRealisation.h"

/// <summary>
/// Указатель на функцию без параметров для взаимодействия с экраном
/// </summary>
typedef void (*screenVoidFuncWithoutParameters) (void);

/// <summary>
/// void Function type with parameters x and y. Use for screen interactions (set pixel
/// </summary>
typedef void (*screenVoidFuncWithPixelPosition) (uint8_t x, uint8_t y);

/// <summary>
/// bool function type with parameters x and y. Use for screen interactions(get pixel color, etc)
/// </summary>
typedef bool (*screenBoolFuncWithPixelPosition) (uint8_t x, uint8_t y);
/// <summary>
/// uint8_t function type to generate random byte number
/// </summary>
typedef uint8_t(*randomNumberGenerationFunction)(void);


/// <summary>
/// function type to get key, which is currently pressed
/// </summary>
typedef uint8_t(*getCurrentPressedKey)(void);

/// <summary>
/// bool function to check, is key pressed. (keycode - key code of button(0x0...0xF)
/// </summary>
typedef bool (*functionToCheckIsKeyPressed)(uint8_t keyCode);
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
	custom_stack addrStack;

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
	/// flag represents Shift instructions behaviour. true - puts vy into vx and shifts vx in 1 bit; false - shifts vx and ignores vy
	/// </summary>
	bool shiftBehaviour;

	/// <summary>
	/// Flag represents behaviour of "Jump with offset"(BNNN) instruction. true - command works as BXNN(jumps to adress XNN+vX register value). False - jumps to NNN register without offset
	/// </summary>
	bool jumpWithOffsetConfigurable;
	/// <summary>
	/// Указатель на функцию для очистки экрана
	/// </summary>
	screenVoidFuncWithoutParameters clearScreen;
	//screenVoidFuncWithPixelPosition reversePixelOnScreen;
	screenVoidFuncWithPixelPosition reversePixelOnScreen;
	screenBoolFuncWithPixelPosition getPixelOnScreen;
	randomNumberGenerationFunction generateRandomNumber;
	functionToCheckIsKeyPressed checkIsKeyPressed;
	getCurrentPressedKey getPressedKey;
	void skipInstruction();
	
public:
	chip8_emulator();
	~chip8_emulator();
	void executeInstruction(uint16_t opcode);
	void set_clearScreen(screenVoidFuncWithoutParameters func);
	void set_reversePixelOnScreen(screenVoidFuncWithPixelPosition func);
	void set_getPixelOnScreen(screenBoolFuncWithPixelPosition func);
	void set_generateRandomNumber(randomNumberGenerationFunction func);
	void set_checkIsKeyPressed(functionToCheckIsKeyPressed func);
	void set_getPressedKey(getCurrentPressedKey func);
	void loadProgrammToMemory(const uint8_t* source, size_t size);
	void performALUoperation(uint16_t secondNibble, uint16_t thirdNibble, uint16_t fourthNibble);
	void runNextInstruction();
#ifdef DEBUG_MODE 
	uint8_t getRegisterVal(uint16_t registerNumber);
	void setRegisterVal(uint16_t registerNumber, uint8_t valToSet);
#endif
};
#endif // !CHIP_8_H


