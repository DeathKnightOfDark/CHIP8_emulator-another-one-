#include "screen.h"
emulatorScreen::emulatorScreen(sf::Vector2f pos, sf::Vector2f pixelSize)
{
	this->startingPose = pos;
	this->sizeOfPixel = pixelSize;
	this->drawPixelsWithSpaceBetween = false;
	for (int i = 0; i < EMULATOR_WIDTH_PIXELS; i++)
	{
		for (int j = 0; j < EMULATOR_HEIGHT_PIXELS; j++)
		{
			this->pixels[pixelPos{i,j}] = false;
			this->pixelsShapes[pixelPos{i,j}] = sf::RectangleShape{this->sizeOfPixel};
		}
	}
	this->redrawShapes();
}
emulatorScreen::~emulatorScreen()
{

}

void emulatorScreen::redrawShapes()
{
	for (int i = 0; i < EMULATOR_WIDTH_PIXELS; i++)
	{
		for (int j = 0; j < EMULATOR_HEIGHT_PIXELS; j++)
		{
			pixelPos key{ i,j };
			sf::Vector2f pixelPosition;
			pixelPosition.x = startingPose.x +3 + i * sizeOfPixel.x;
			pixelPosition.y = startingPose.y + 3+ j * sizeOfPixel.y;
			if (drawPixelsWithSpaceBetween)
			{
				pixelsShapes[key].setOutlineColor(sf::Color::Color(59,59,59,255));
				pixelsShapes[key].setOutlineThickness(1);
			}
			else
			{
				pixelsShapes[key].setOutlineThickness(0);
			}
			pixelsShapes[key].setPosition(pixelPosition);
			pixelsShapes[key].setFillColor(this->pixels[key] ? sf::Color::White : sf::Color::Black);
		}
	}
}

void emulatorScreen::redrawPixel(pixelPos pixelPosition, bool newVal)
{
	if ((std::get<0>(pixelPosition) < EMULATOR_WIDTH_PIXELS) && (std::get<1>(pixelPosition) < EMULATOR_HEIGHT_PIXELS))
	{
		this->pixels[pixelPosition] = newVal;
		this->pixelsShapes[pixelPosition].setFillColor(this->pixels[pixelPosition] ? sf::Color::White : sf::Color::Black);
	}
}

void emulatorScreen::render(sf::RenderWindow* windowOnRenderTo)
{
	for (int i = 0; i < EMULATOR_WIDTH_PIXELS; i++)
	{
		for (int j = 0; j < EMULATOR_HEIGHT_PIXELS; j++)
		{
			pixelPos key{ i,j };
			windowOnRenderTo->draw(this->pixelsShapes[key]);
		}
	}
}


void emulatorScreen::fillPixels(bool value)
{
	for (int i = 0; i < EMULATOR_WIDTH_PIXELS; i++)
	{
		for (int j = 0; j < EMULATOR_HEIGHT_PIXELS; j++)
		{
			pixelPos key{ i,j };
			redrawPixel(key, value);
		}
	}
}
void emulatorScreen::setBorder(bool borderCond)
{
	drawPixelsWithSpaceBetween = borderCond;
	redrawShapes();
}

bool emulatorScreen::getPixel(pixelPos pixelPosition)
{
	if ((std::get<0>(pixelPosition) < EMULATOR_WIDTH_PIXELS) && (std::get<1>(pixelPosition) < EMULATOR_HEIGHT_PIXELS))
	{
		return this->pixels[pixelPosition];
	}
	else return false;
}
void emulatorScreen::reversePixel(pixelPos pixelPosition)
{
	if ((std::get<0>(pixelPosition) < EMULATOR_WIDTH_PIXELS) && (std::get<1>(pixelPosition) < EMULATOR_HEIGHT_PIXELS))
	{
		this->redrawPixel(pixelPosition, !this->getPixel(pixelPosition));
	}
}