#include "MWPdraw.h"
#include "MWPutil.h"

inline void setPixel(uint32_t *pixel, uint32_t red, uint32_t green, uint32_t blue)
{
	// draws to an individual pixel
	// by looping through an amount of memory equal to the number of rows * columns

	/*              0  1  2  3
			 Pixel in mem: BB GG RR xx

			 loaded into reg
			 0x xxRRGGBB
	*/
	*pixel = ((red << 16) | (green << 8) | blue);
}

void background(Winfo* window, char red, char green, char blue)
{
	//Draws to every pixel in memory, filling in the whole memory
	uint32_t* Pixel = (uint32_t*)window->winPixMemory;
	int pixels = window->winHeight * window->winWidth;

	for(int count = 0; count < pixels; ++count)
	{
		setPixel(Pixel, red, green, blue);
		++Pixel;
	}
}

void randomBackground(Winfo* window)
{
	//Draws to every pixel in memory, filling in the whole memory
	uint32_t* Pixel = (uint32_t*)window->winPixMemory;
	int pixels = window->winHeight * window->winWidth;

	for(int count = 0; count < pixels; ++count)
	{
		setPixel(Pixel, MWPrand() * 255, MWPrand() * 255, MWPrand() * 255);
		++Pixel;
	}
}

void drawHorizontalLine(Winfo* window, uint32_t x0, uint32_t x1, uint32_t y, char red, char green, char blue)
{
	int index;
	int endX;
	uint32_t* Pixel = ((uint32_t*)window->winPixMemory) + window->winWidth * y;

	if(x0 < x1)
	{
		Pixel += x0;
		index = x0;
		endX = x1;
	}
	else
	{
		Pixel += x1;
		index = x1;
		endX = x0;
	}
	for(; index <= endX; ++index)
	{
		setPixel(Pixel, red, green, blue);
		++Pixel;
	}
}

void drawVerticalLine(Winfo* window, uint32_t y0, uint32_t y1, uint32_t x, char red, char green, char blue)
{
	int index;
	int endY;
	uint32_t* Pixel = ((uint32_t*)window->winPixMemory) + x;

	if(y0 < y1)
	{
		Pixel += y0 * window->winWidth;
		index = y0;
		endY = y1;
	}
	else
	{
		Pixel += y1 * window->winWidth;
		index = y1;
		endY = y0;
	}
	for(; index <= endY; ++index)
	{
		setPixel(Pixel, red, green, blue);
		Pixel += window->winWidth;
	}
}

void
drawRectangle(Winfo* window, int x, int y, int width, int height, uint32_t red, uint32_t green, uint32_t blue)
{
	uint32_t* Pixel = ((uint32_t*)window->winPixMemory) + y * window->winWidth + x;

	for(int yindex = 0; yindex < height; ++yindex)
	{
		for(int xindex = 0; xindex < width; ++xindex)
		{
			setPixel(Pixel, red, green, blue);
			++Pixel;
		}
		Pixel = ((uint32_t*)window->winPixMemory) + (y + yindex) * window->winWidth + x;
	}
}