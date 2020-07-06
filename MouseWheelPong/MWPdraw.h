#pragma once
#include "MWPheader.h"
#include "MWPgameInfo.h"
#include "MWPutil.h"

struct rgb
{
	unsigned char red;
	unsigned char green;
	unsigned char blue;

	rgb(unsigned char Red, unsigned char Green, unsigned char Blue)
	{
		red = Red;
		green = Green;
		blue = Blue;
	}
};
inline int32_t abs(int32_t abs);
inline void setPixel(uint32_t *pixel, const rgb & color);
void setPixelXY(Winfo* window, uint32_t x, uint32_t y, const rgb & color);
void background(Winfo* window, const rgb & color);
void randomBackground(Winfo* window);
void drawHorizontalLine(Winfo* window, uint32_t x0, uint32_t x1, uint32_t y, const rgb & color);
void drawVerticalLine(Winfo* window, uint32_t y0, uint32_t y1, uint32_t x, const  rgb & color);
void drawSpreadVerticalLines(Winfo* window, int numberOfLines, const rgb & color);
void drawLineB(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, const rgb & color);
void drawRectangle(Winfo* window, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const rgb & color);
void rasterizeRectangle(Winfo* window, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const rgb & color);
void drawTriangle(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb & color);
void rasterizeTriangle(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb & color);
void drawCircle(Winfo* window, uint32_t x, uint32_t y, uint32_t r, const rgb & color);
void rasterizeCircle(Winfo* window, uint32_t x, uint32_t y, uint32_t r, const rgb & color);
void drawPaddlesRegistrationScreen(Winfo* window, std::map<HANDLE, Player> & mouseMapping, const rgb & color);