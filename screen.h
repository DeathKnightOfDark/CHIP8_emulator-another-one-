#pragma once

#ifndef CHIP_8_SCREEN
#define CHIP_8_SCREEN
#define EMULATOR_WIDTH_PIXELS 64
#define EMULATOR_HEIGHT_PIXELS 32
#include <SFML/Graphics.hpp>
#include <map>
#include <tuple>

typedef std::tuple<int, int> pixelPos;
class emulatorScreen
{
public:
	emulatorScreen(sf::Vector2f pos, sf::Vector2f pixelSize);
	~emulatorScreen();
	void redrawShapes();
	void redrawPixel(pixelPos pixelPosition, bool newVal);
	void render(sf::RenderWindow* windowOnRenderTo);
	void fillPixels(bool value);
	void setBorder(bool borderCond);
	private:
		sf::Vector2f startingPose;
		sf::Vector2f sizeOfPixel;
		std::map<pixelPos, bool> pixels;
		std::map<pixelPos, sf::RectangleShape> pixelsShapes;
		bool drawPixelsWithSpaceBetween;

};
#endif // !CHIP_8_SCREEN

