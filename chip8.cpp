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
	memcpy((void*)&this->emulatorData[0x050], (const void*)font, sizeof(font));

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

void chip8_emulator::executeInstruction(uint16_t opcode)
{
	uint8_t command = (uint8_t)((opcode & 0xF000)>>12);
	uint16_t secondNibble = (opcode & 0x0F00);
	uint16_t thirdNibble = (opcode & 0x00F0);
	uint16_t fourthNibble = (opcode & 0x000F);
	
	switch (command)
	{
		///0x00E0 - clear screen command
	case 0x0: 
	{
		if (clearScreen!=NULL) this->clearScreen();
		
		break;
	}
		///0x1NNN - jump command
	case 0x1: 
	{
		uint16_t val = secondNibble | thirdNibble | fourthNibble;
		this->PC = val;
		break;
	}
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
		if (this->registers[secondNibble] == val) this->PC += 2;
		break;
	}
	case 0x4:
	{
		uint16_t val = thirdNibble | fourthNibble;
		if (this->registers[secondNibble] != val) this->PC += 2;
		break;
	}
		///0x6XNN - set val to register command
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
		if (fourthNibble==0) this->registers[secondNibble] = this->registers[thirdNibble >> 4];
		break;
	}
	case 0xA: 
	{
		this->I = secondNibble | thirdNibble | fourthNibble;
		
		break;
	}
	case 0xB: 
	{
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
				for (uint8_t j = 128; j > 0; j>>=1)
				{
					if (x < SCREEN_PIXEL_WIDTH)
					{
						bool pixelVal = this->getPixelOnScreen(x, vy_value);
						if (spriteData & j)
						{
							if (pixelVal) this->registers[0xF] = 1;
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
	}
}

void chip8_emulator::loadProgrammToMemory(const uint8_t* source, size_t size)
{
	memcpy((void*)&this->emulatorData[0x200], (const void*)source, (size_t)size);
	this->PC = 0x200;
}

void chip8_emulator::runNextInstruction()
{

	//uint16_t instruction =  this->emulatorData[this->PC+1]<<8 | (uint16_t)(this->emulatorData[this->PC]) ;
	uint16_t byte0 = (uint16_t)this->emulatorData[this->PC];
	uint16_t byte1 = (uint16_t)this->emulatorData[this->PC + 1];
	uint16_t instruction = byte1 | (byte0 << 8);
	std::cout << "Executing comand: " << instruction << std::endl;
	
	this->PC += 2;
	
	executeInstruction(instruction);
}