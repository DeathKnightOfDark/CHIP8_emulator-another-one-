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
	uint16_t thirdNibble = (opcode & 0x00F0);
	uint16_t fourthNibble = (opcode & 0x000F);
	uint16_t secondByte = (opcode & 0xFF);
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
	case 0x3:
	{
		uint16_t val = thirdNibble | fourthNibble;
		if (this->registers[secondNibble] == val)
			this->skipInstruction();
		break;
	}
	case 0x4:
	{
		uint16_t val = thirdNibble | fourthNibble;
		if (this->registers[secondNibble] != val)
			this->skipInstruction();
		break;
	}
	case 0x5:
	{
		if (fourthNibble == 0x0)
		{
			if (this->registers[secondNibble] == this->registers[thirdNibble >> 4])
				this->skipInstruction();
		}
		break;
	}
		/// 0x6XNN - set val to register command
	case 0x6:
	{
		uint16_t val = thirdNibble | fourthNibble;
		this->registers[(uint8_t)(secondNibble >> 8)] = val;

		break;
	}
	case 0x7:
	{
		uint16_t val = thirdNibble | fourthNibble;
		this->registers[(uint8_t)(secondNibble >> 8)] += val;

		break;
	}
	case 0x8:
	{

		this->performALUoperation(secondNibble >> 8, thirdNibble, fourthNibble);
		break;
	}
	case 0x9:
	{
		if (fourthNibble == 0x0)
		{
			if (this->registers[secondNibble] != this->registers[thirdNibble >> 4])
				this->skipInstruction();
		}
		break;
	}
	case 0xA:
	{
		this->I = secondNibble | thirdNibble | fourthNibble;

		break;
	}
	case 0xB:
	{
		int jumpAdress = ((int)(secondNibble)) << 8 | (int)(secondByte);
		if (this->jumpWithOffsetConfigurable)
		{
			jumpAdress += (int)(this->registers[secondNibble >> 8]);
		}
		this->PC = jumpAdress;
		break;
	}
	case 0xC:
	{
		if (this->generateRandomNumber != NULL)
		{
			uint8_t num = this->generateRandomNumber();
			num = num & (uint8_t)secondByte;
			this->registers[secondNibble >> 8] = num;
		}
		break;
	}
	case 0xD:
	{
		uint8_t vx_value = this->registers[secondNibble >> 8] & 63;
		uint8_t vy_value = this->registers[thirdNibble >> 4] & 31;
		uint8_t n_value = fourthNibble;

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
	case 0xE:
	{
		if (this->checkIsKeyPressed != NULL)
		{
			if (secondByte == 0x9E)
			{
				if (this->checkIsKeyPressed(this->registers[secondNibble >> 8]))
					this->PC += 2;
			}
			if (secondByte == 0xA1)
			{
				if (!this->checkIsKeyPressed(this->registers[secondNibble >> 8]))
					this->PC += 2;
			}
		}
		break;
	}
	case 0xF:
	{
		switch (secondByte)
		{
		case 0x07:
		{
			this->registers[secondNibble >> 8] = this->delayTimer;
			break;
		}
		case 0x15:
		{
			this->delayTimer = this->registers[secondNibble >> 8];
			break;
		}
		case 0x18:
		{
			this->soundTimer = this->registers[secondNibble >> 8];
			break;
		}
		case 0x1E:
		{
			this->I = this->I + this->registers[secondNibble >> 8];
			if (this->I > 0x1000)
			{
				this->registers[0xF] = 1;
				this->I = this->I & 0x0FFF;
			}
			break;
		}
		case 0x0A:
		{
			if (this->getPressedKey != NULL)
			{
				uint8_t pressedKey = this->getPressedKey();
				if (pressedKey <= 0xF)
				{
					this->registers[secondNibble >> 8] = pressedKey;
				}
				else
				{
					this->PC -= 2;
				}
			}

			break;
		}
		case 0x29:
		{
			this->I = 0x0050 + (uint16_t)(this->registers[secondNibble >> 8]) * 5;
			break;
		}
		case 0x33:
		{
			uint8_t num = this->registers[secondNibble >> 8];
			this->emulatorData[this->I] = num / 100;
			this->emulatorData[this->I + 1] = (num % 100) / 10;
			this->emulatorData[this->I + 2] = (num % 100) % 10;
			break;
		}
		case 0x55:
		{
			uint8_t num = secondNibble >> 8;
			for (int i = 0; i < num; i++)
			{
				this->emulatorData[this->I + i] = this->registers[i];
			}
			break;
		}
		case 0x65:
		{
			uint8_t num = secondNibble >> 8;
			for (int i = 0; i < num; i++)
			{
				this->registers[i] = this->emulatorData[this->I + i];
			}
			break;
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
	std::cout << "Executing comand: " << instruction << std::endl;

	this->PC += 2;

	executeInstruction(instruction);
}

void chip8_emulator::performALUoperation(uint16_t secondNibble, uint16_t thirdNibble, uint16_t fourthNibble)
{
	switch (fourthNibble)
	{
		/// set instruction
	case 0:
	{
		this->registers[secondNibble] = this->registers[thirdNibble >> 4];
		break;
	}

	/// OR instruction ( vx OR vy)
	case 1:
	{
		this->registers[secondNibble] = this->registers[secondNibble] | this->registers[thirdNibble >> 4];
		break;
	}
	/// AND instruction (vx AND vy)
	case 2:
	{
		this->registers[secondNibble] = this->registers[secondNibble] & this->registers[thirdNibble >> 4];
		break;
	}
	/// XOR instruction (vx XOR vy)
	case 3:
	{
		this->registers[secondNibble] = this->registers[secondNibble] ^ this->registers[thirdNibble >> 4];
		break;
	}
	/// vx + vy instruction (with overflowing to vf)
	case 4:
	{

		if ((uint16_t)this->registers[secondNibble] + (uint16_t)this->registers[thirdNibble >> 4] > 0xFF)
		{
			this->registers[0xF] = 1;
		}
		this->registers[secondNibble] = this->registers[secondNibble] + this->registers[thirdNibble >> 4];
		break;
	}
	/// substract (vx - vy)
	case 5:
	{
		if (this->registers[secondNibble] > this->registers[thirdNibble >> 4])
			this->registers[0xF] = 1;
		else
		{
			if (this->registers[thirdNibble >> 4] > this->registers[secondNibble])
				this->registers[0xF] = 0;
		}
		this->registers[secondNibble] = this->registers[secondNibble] - this->registers[thirdNibble >> 4];
		break;
	}
	case 6:
	{
		if (this->shiftBehaviour)
		{
			this->registers[secondNibble] = this->registers[thirdNibble >> 4];
		}

		this->registers[0xF] = this->registers[secondNibble] & 0x1;
		this->registers[secondNibble] = this->registers[secondNibble] >> 1;

		break;
	}
	/// substract (vy - vx)
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

void chip8_emulator::set_generateRandomNumber(randomNumberGenerationFunction func)
{
	this->generateRandomNumber = func;
}
#endif