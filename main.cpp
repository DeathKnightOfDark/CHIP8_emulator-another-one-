#include <SFML/Graphics.hpp>
#include <iostream>
#include "screen.h"
int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 800), "procedural animation");
	sf::RectangleShape shape{sf::Vector2f{10, 10}};
	shape.setFillColor(sf::Color::White);
	shape.setPosition(sf::Vector2f{40, 40});
	int counter = 0;
	
	emulatorScreen screen{ sf::Vector2f{100,100},sf::Vector2f{10,10} };
	sf::Clock t;
	int widthcounter = 0, heightcounter = 0;
	screen.fillPixels(true);
	screen.redrawPixel(pixelPos{ 10,10 }, false);
	screen.setBorder(true);
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		}

		if (t.getElapsedTime().asMilliseconds() > 200)
		{
			if (heightcounter < EMULATOR_HEIGHT_PIXELS)
			{
				screen.redrawPixel(pixelPos{ widthcounter, heightcounter }, false);
				widthcounter++;
				heightcounter++;
			}
			else
			{
				widthcounter = 0;
				heightcounter = 0;
			}
			t.restart();
		}

		window.clear();
		
		screen.render(&window);
		
		window.display();
		/*if (counter < 1)
		{
			window.clear();
			window.draw(shape);
			window.display();
			counter++;
		}*/
	}
	return 0;
}