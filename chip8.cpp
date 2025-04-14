#include "chip8.h"

chip8_emulator::chip8_emulator()
{
	emulatorData = new uint8_t[4096];
	registers = new uint8_t[16];
	I = 0;
	PC = 0x200;
	delayTimer = 0;
	soundTimer = 0;
	clearScreen = NULL;
	reversePixelOnScreen = NULL;
	getPixelOnScreen = NULL;
	generateRandomNumber = NULL;
	checkIsKeyPressed = NULL;
	getPressedKey = NULL;
	memcpy((void *)&this->emulatorData[0x050], (const void *)font, sizeof(font));
	shiftBehaviour = true;
	jumpWithOffsetConfigurable = true;
}

chip8_emulator::~chip8_emulator()
{
	delete[] emulatorData;
	delete[] registers;
}

void chip8_emulator::set_clearScreen(screenVoidFuncWithoutParameters func)
{
	this->clearScreen = func;
}

void chip8_emulator::set_reversePixelOnScreen(screenVoidFuncWithPixelPosition func)
{
	this->reversePixelOnScreen = func;
}
void chip8_emulator::set_getPixelOnScreen(screenBoolFuncWithPixelPosition func)
{
	this->getPixelOnScreen = func;
}

void chip8_emulator::set_checkIsKeyPressed(functionToCheckIsKeyPressed func)
{
	this->checkIsKeyPressed = func;
}

void chip8_emulator::set_getPressedKey(getCurrentPressedKey func)
{
	this->getPressedKey = func;
}
void chip8_emulator::skipInstruction()
{
	this->PC += 2;
}
void chip8_emulator::executeInstruction(uint16_t opcode)
{
	uint8_t command = (uint8_t)((opcode & 0xF000) >> 12);
	
	uint16_t secondNibble = (opcode & 0x0F00);
	uint8_t secondNibble_shifted = (uint8_t)(secondNibble >> 8);
	uint16_t thirdNibble = (opcode & 0x00F0);
	uint8_t thirdNibble_shifted = (uint8_t)(thirdNibble >> 4);
	uint16_t fourthNibble = (opcode & 0x000F);
	uint8_t secondByte = (uint8_t)(opcode & 0xFF);
	switch (command)
	{

	case 0x0:
	{
		/// 0x00E0 - clear screen command
		if (secondByte == 0x0E)
		{
			if (this->clearScreen != NULL)
				this->clearScreen();
		}
		if (secondByte == 0xEE)
		{
			this->PC = (uint16_t)this->addrStack.pop();
		}

		break;
	}
		/// 0x1NNN - jump command
	case 0x1:
	{
		uint16_t val = secondNibble | thirdNibble | fourthNibble;
		this->PC = val;
		break;
	}
	/// 0x2NNN - subroutine
	case 0x2:
	{
		this->addrStack.push(this->PC);
		uint16_t val = secondNibble | thirdNibble | fourthNibble;
		this->PC = val;
		break;
	}
	/// 0x3XNN - skip instruction, if value in registers[X] == NN
	case 0x3:
	{
		
		if (this->registers[secondNibble_shifted] == secondByte)
			this->skipInstruction();
		break;
	}

	/// 0x4XNN - skip instruction, if value in registers[X] != NN
	case 0x4:
	{
		
		if (this->registers[secondNibble_shifted] != secondByte)
			this->skipInstruction();
		break;
	}
	/// 0x5XY0 - skip instruction, if the value in registers[X] == value in registers[Y]
	case 0x5:
	{
		if (fourthNibble == 0x0)
		{
			if (this->registers[secondNibble_shifted] == this->registers[thirdNibble_shifted])
				this->skipInstruction();
		}
		break;
	}
		/// 0x6XNN - command to set NN-value to registers[X]
	case 0x6:
	{
		
		this->registers[secondNibble_shifted] = secondByte;

		break;
	}
	/// 0x7XNN - add the value NN to registers[X]
	case 0x7:
	{
		
		this->registers[secondNibble_shifted] += secondByte;

		break;
	}
	/// 0x8... - do set of logic and arithmetic commands
	case 0x8:
	{

		this->performALUoperation(secondNibble_shifted, thirdNibble_shifted, (uint8_t)(fourthNibble & 0xFF));
		break;
	}
	/// 0x9XY0 - skip instruction, if the value in registers[X] != value in registers[Y]
	case 0x9:
	{
		if (fourthNibble == 0x0)
		{
			if (this->registers[secondNibble_shifted] != this->registers[thirdNibble_shifted])
				this->skipInstruction();
		}
		break;
	}
	/// 0xANNN - Set index - set register I to the value NNN
	case 0xA:
	{
		this->I = secondNibble | thirdNibble | fourthNibble;

		break;
	}
	/// 0xBNNN - Jump with offset 
	case 0xB:
	{
		int jumpAdress = (int)(secondNibble | thirdNibble | fourthNibble);
		if (this->jumpWithOffsetConfigurable)
		{
			jumpAdress += (int)(this->registers[secondNibble >> 8]);
		}
		this->PC = jumpAdress;
		break;
	}
	/// 0xCXNN - Random - generates random number and binary ANDs in with the value NN, and puts result in registers[X]
	case 0xC:
	{
		if (this->generateRandomNumber != NULL)
		{
			uint8_t num = this->generateRandomNumber();
			num = num & secondByte;
			this->registers[secondNibble_shifted] = num;
		}
		break;
	}
	/// 0xDXYN - display pixels on screen
	case 0xD:
	{
		uint8_t vx_value = this->registers[secondNibble_shifted] & 63;
		uint8_t vy_value = this->registers[thirdNibble_shifted] & 31;
		uint8_t n_value = (uint8_t)fourthNibble;

		this->registers[0xF] = 0;
		for (int i = 0; i < n_value; i++)
		{
			if (vy_value < SCREEN_PIXEL_HEIGHT)
			{
				uint8_t x = vx_value;
				uint8_t spriteData = this->emulatorData[this->I + i];
				for (uint8_t j = 128; j > 0; j >>= 1)
				{
					if (x < SCREEN_PIXEL_WIDTH)
					{
						bool pixelVal = this->getPixelOnScreen(x, vy_value);
						if (spriteData & j)
						{
							if (pixelVal)
								this->registers[0xF] = 1;
							this->reversePixelOnScreen(x, vy_value);
						}
						x++;
					}
				}
				vy_value++;
			}
		}

		break;
	}
	/// 0xEX9E / 0xEXA1 - skip if key
	case 0xE:
	{
		if (this->checkIsKeyPressed != NULL)
		{
			/// skip, if key with the code from registers[X] IS  pressed
			if (secondByte == 0x9E)
			{
				if (this->checkIsKeyPressed(this->registers[secondNibble_shifted]))
					this->PC += 2;
			}
			/// skip, if key with the code from registers[X] IS NOT pressed
			if (secondByte == 0xA1)
			{
				std::cout << (int)this->registers[secondNibble_shifted] << std::endl;
				if (!this->checkIsKeyPressed(this->registers[secondNibble_shifted]))
					this->PC += 2;
			}
		}
		break;
	}
	/// Perform various operations
	case 0xF:
	{
		switch (secondByte)
		{
		/// 0xFX07 - sets registers[X] to the current val of delay timer
		case 0x07:
		{
			this->registers[secondNibble_shifted] = this->delayTimer;
			break;
		}
		/// 0xFX15 - sets delay timer to the value from registers[X]
		case 0x15:
		{
			this->delayTimer = this->registers[secondNibble_shifted];
			break;
		}
		/// 0xFX18 - set the sound timer to the value in registers[X]
		case 0x18:
		{
			this->soundTimer = this->registers[secondNibble_shifted];
			break;
		}
		/// 0xFX1E - Add to index - the index register I will get the value in registers[X] added to it  (with overflow to registers[0xF]
		case 0x1E:
		{
			this->I = this->I + this->registers[secondNibble_shifted];
			if (this->I > 0x1000)
			{
				this->registers[0xF] = 1;
				this->I = this->I & 0x0FFF;
			}
			break;
		}

		/// 0xFX0A - Get Key - stop executing, unless key is pressed. After the key is pressed - put its code to registers[X]
		case 0x0A:
		{
			if (this->getPressedKey != NULL)
			{
				uint8_t pressedKey = this->getPressedKey();
				if (pressedKey <= 0xF)
				{
					this->registers[secondNibble_shifted] = pressedKey;
				}
				else
				{
					this->PC -= 2;
				}
			}

			break;
		}
		/// 0xFX29 - Font Character - the index register I is set to the adress of hexadecimal character in registers[X].
		case 0x29:
		{
			this->I = 0x0050 + (uint16_t)(this->registers[secondNibble_shifted]) * 5;
			break;
		}
		/// 0xFX33 - binary code decimal conversion. registers[X] divided into three decimal digits, and stores them in memory at the adress in the index register I
		case 0x33:
		{
			uint8_t num = this->registers[secondNibble_shifted];
			this->emulatorData[this->I] = num / 100;
			this->emulatorData[this->I + 1] = (num % 100) / 10;
			this->emulatorData[this->I + 2] = (num % 100) % 10;
			break;
		}
		/// 0xFX55 - store memory - storing in memory values from registers[0] to registers[X]
		case 0x55:
		{
			
			for (int i = 0; i < secondNibble_shifted; i++)
			{
				this->emulatorData[this->I + i] = this->registers[i];
			}
			break;
		}
		/// 0xFX65 - load memory - loading from memory to registers[0] to registers[X]
		case 0x65:
		{
			
			for (int i = 0; i < secondNibble_shifted; i++)
			{
				this->registers[i] = this->emulatorData[this->I + i];
			}
			break;
		}
		default: 
		{
			std::cout << "Unknown instruction error, opcode: " << opcode << std::endl;
		}
		}
	}
	}
}

void chip8_emulator::loadProgrammToMemory(const uint8_t *source, size_t size)
{
	memcpy((void *)&this->emulatorData[0x200], (const void *)source, (size_t)size);
	this->PC = 0x200;
}

void chip8_emulator::runNextInstruction()
{

	// uint16_t instruction =  this->emulatorData[this->PC+1]<<8 | (uint16_t)(this->emulatorData[this->PC]) ;
	uint16_t byte0 = (uint16_t)this->emulatorData[this->PC];
	uint16_t byte1 = (uint16_t)this->emulatorData[this->PC + 1];
	uint16_t instruction = byte1 | (byte0 << 8);
	std::cout << "Executing comand: " << std::hex<< instruction << std::endl;

	this->PC += 2;

	executeInstruction(instruction);
}

void chip8_emulator::performALUoperation(uint8_t secondNibble, uint8_t thirdNibble, uint8_t fourthNibble)
{
	switch (fourthNibble)
	{
		/// 0x8XY0 - Set - registers[X] = registers[Y]
	case 0:
	{
		this->registers[secondNibble] = this->registers[thirdNibble];
		break;
	}

	/// 0x8XY1 - LOGICAL OR - registers[X] = (registers[X] | registers[Y])
	case 1:
	{
		this->registers[secondNibble] = this->registers[secondNibble] | this->registers[thirdNibble];
		break;
	}
	/// 0x8XY2 - LOGICAL AND - registers[X] = (registers[X] & registers[Y])
	case 2:
	{
		this->registers[secondNibble] = this->registers[secondNibble] & this->registers[thirdNibble];
		break;
	}
	/// 0x8XY3 - LOGICAL XOR - registers[X] = (registers[X] ^(XOR) registers[Y])
	case 3:
	{
		this->registers[secondNibble] = this->registers[secondNibble] ^ this->registers[thirdNibble];
		break;
	}
	/// 0x8XY4 - ADD - registers[X] set to the value of (registers[X] + registers[Y]) with overflowing to registers[0xF]
	case 4:
	{

		if ((uint16_t)this->registers[secondNibble] + (uint16_t)this->registers[thirdNibble] > 0xFF)
		{
			this->registers[0xF] = 1;
		}
		this->registers[secondNibble] = this->registers[secondNibble] + this->registers[thirdNibble];
		break;
	}
	/// 0x8XY5 substract (registers[X] - registers[Y]) with overflowing to registers[0xF]
	case 5:
	{
		if (this->registers[secondNibble] > this->registers[thirdNibble])
			this->registers[0xF] = 1;
		else
		{
			if (this->registers[thirdNibble] > this->registers[secondNibble])
				this->registers[0xF] = 0;
		}
		this->registers[secondNibble] = this->registers[secondNibble] - this->registers[thirdNibble];
		break;
	}
	/// 0x8XY6 set registers[X] to the val of registers[Y] and shift registers[X] to the right on 1 bit (with overflowing to registers[0xF])
	case 6:
	{
		if (this->shiftBehaviour)
		{
			this->registers[secondNibble] = this->registers[thirdNibble];
		}

		this->registers[0xF] = this->registers[secondNibble] & 0x1;
		this->registers[secondNibble] = this->registers[secondNibble] >> 1;

		break;
	}
	/// 0x8XY7 substract (registers[Y] - registers[X]) with overflowing to registers[0xF]
	case 7:
	{
		if (this->registers[thirdNibble >> 4] > this->registers[secondNibble])
			this->registers[0xF] = 1;
		else
		{
			if (this->registers[secondNibble] > this->registers[thirdNibble >> 4])
				this->registers[0xF] = 0;
		}
		this->registers[thirdNibble >> 4] = this->registers[thirdNibble >> 4] - this->registers[secondNibble];
		break;
	}
	/// 0x8XY6 set registers[X] to the val of registers[Y] and shift registers[X] to the left on 1 bit (with overflowing to registers[0xF])
	case 0xE:
	{
		if (this->shiftBehaviour)
		{
			this->registers[secondNibble] = this->registers[thirdNibble >> 4];
		}

		this->registers[0xF] = (this->registers[secondNibble] & 0x80) >> 7;
		this->registers[secondNibble] = this->registers[secondNibble] << 1;
	}
	}
}
void chip8_emulator::set_generateRandomNumber(randomNumberGenerationFunction func)
{
	this->generateRandomNumber = func;
}
void chip8_emulator::decreaseTimers()
{
	if (this->delayTimer > 0) this->delayTimer--;
	if (this->soundTimer > 0) this->soundTimer--;
}
#ifdef DEBUG_MODE
uint8_t chip8_emulator::getRegisterVal(uint16_t registerNumber)
{
	if (registerNumber <= 0xF)
		return this->registers[registerNumber];
}

void chip8_emulator::setRegisterVal(uint16_t registerNumber, uint8_t valToSet)
{
	if (registerNumber <= 0xF)
	{
		this->registers[registerNumber] = valToSet;
	}
}


#endif