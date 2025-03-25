#include <SFML/Graphics.hpp>
#include <iostream>
#include "screen.h"
#include "chip8.h"
#include <functional>
#include "IBM_LOGO_TEST.h"

emulatorScreen screen{ sf::Vector2f{2, 2}, sf::Vector2f{10, 10} };
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
int main()
{
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
	
	emul.loadProgrammToMemory(IBMlogo, sizeof(IBMlogo));

	//std::cout << sizeof(IBMlogo) << std::endl;
	int widthcounter = 0, heightcounter = 0;
	
	std::function<void(emulatorScreen*, bool)> f = &emulatorScreen::fillPixels;
	
	//screen.fillPixels(false);
	//f(&screen, true);
	sf::Clock cl;
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}
		if (cl.getElapsedTime().asMilliseconds() > 5)
		{
			emul.runNextInstruction();
			cl.restart();
		}
		

		window.clear();
		
		screen.render(&window);
		
		window.display();
		
	}
	return 0;
}