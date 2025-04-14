
#include <SFML/Graphics.hpp>
#include <iostream>
#include "screen.h"
#include "chip8.h"
#include <functional>
#include "IBM_LOGO_TEST.h"
#include <vector>
#include <fstream>
#include <iterator>
#include <cstdio>

emulatorScreen screen{ sf::Vector2f{2, 2}, sf::Vector2f{10, 10} };
uint8_t currentKeyCode = 0xFF;
void clearScreen_wrapper()
{
	screen.fillPixels(false);
}
bool getPixel_wrapper(uint8_t x, uint8_t y)
{
	 return screen.getPixel(pixelPos{ x,y });
}

void redrawPixel_wrapper(uint8_t x, uint8_t y)
{
	screen.reversePixel(pixelPos{ x,y });
}
uint8_t generateRandomNumber()
{
	return (uint8_t)(rand() % (256));
}
bool checkIsKeyPressed_wrapper(uint8_t keyCode)
{
	return (keyCode == currentKeyCode);
}
uint8_t getCurrentPressedKey_wrapper()
{
	return currentKeyCode;
}

void loadFile(const char* filename, std::vector<char>& vectorToPutData)
{
	std::ifstream input;
	input.open(filename, std::ios::binary);
	if (input.is_open())
	{
		for (char c : std::vector<char>(std::istreambuf_iterator<char>(input), {}))
		{
			vectorToPutData.push_back(c);
		}
	}
}

void checkKeyBoard(uint8_t &keyCode)
{
	keyCode = 0xFF;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) keyCode = 0x1;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) keyCode = 0x2;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) keyCode = 0x3;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) keyCode = 0xC;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) keyCode = 0x4;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) keyCode = 0x5;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) keyCode = 0x6;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) keyCode = 0xD;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) keyCode = 0x7;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) keyCode = 0x8;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) keyCode = 0x9;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) keyCode = 0xE;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) keyCode = 0xA;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) keyCode = 0x0;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) keyCode = 0xB;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::V)) keyCode = 0xF;
}

int main()
{
	
	std::srand(std::time({}));
	sf::RenderWindow window(sf::VideoMode(10*EMULATOR_WIDTH_PIXELS+10, 10*EMULATOR_HEIGHT_PIXELS+10), "CHIP-8 emulator");
	sf::RectangleShape shape{sf::Vector2f{10, 10}};
	shape.setFillColor(sf::Color::White);
	shape.setPosition(sf::Vector2f{40, 40});
	int counter = 0;

	
	screen.setBorder(true);
	screen.fillPixels(false);
	chip8_emulator emul;
	emul.set_clearScreen((screenVoidFuncWithoutParameters)clearScreen_wrapper);
	emul.set_getPixelOnScreen((screenBoolFuncWithPixelPosition)getPixel_wrapper);
	emul.set_reversePixelOnScreen((screenVoidFuncWithPixelPosition)redrawPixel_wrapper);
	emul.set_generateRandomNumber((randomNumberGenerationFunction)generateRandomNumber);
	emul.set_checkIsKeyPressed((functionToCheckIsKeyPressed)(checkIsKeyPressed_wrapper));
	emul.set_getPressedKey((getCurrentPressedKey)(getCurrentPressedKey_wrapper));
	std::vector<char> programm;
	//loadFile("ROMS/1-ibm-logo.ch8", programm);
	//loadFile("ROMS/ibm.ch8", programm);
	loadFile("ROMS/Particle Demo [zeroZshadow, 2008].ch8", programm);
	//loadFile("ROMS/Pong (1 player).ch8", programm);
	if (programm.size() == 0) return -1;
	emul.loadProgrammToMemory((const uint8_t*)programm.data(), programm.size());
	//emul.loadProgrammToMemory(IBMlogo, sizeof(IBMlogo));

	//std::cout << sizeof(IBMlogo) << std::endl;
	int widthcounter = 0, heightcounter = 0;
	
	std::function<void(emulatorScreen*, bool)> f = &emulatorScreen::fillPixels;
	
	

	int64_t periodTime = 1000000 / 60;
	//screen.fillPixels(false);
	//f(&screen, true);
	sf::Clock cl;
	sf::Clock timerClock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}
		if (cl.getElapsedTime().asMicroseconds() > 1428)
		{
			
			
			emul.runNextInstruction();
			
			cl.restart();
		}
		if (timerClock.getElapsedTime().asMicroseconds() > periodTime)
		{
			checkKeyBoard(currentKeyCode);
			emul.decreaseTimers();
			
			window.clear();
			screen.render(&window);

			window.display();
			timerClock.restart();
		}
		//window.clear();

		

		
		
	}
	return 0;
}