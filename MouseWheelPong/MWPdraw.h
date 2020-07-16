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

struct point
{
	uint32_t x;
	uint32_t y;

	point(uint32_t X, uint32_t Y)
	{
		x = X;
		y = Y;
	}

};

// stores info for drawing a line from the Low y point to the High y pointo
struct line
{
	// TODO: Change line struct to accept 2 const point structs instead of 4 uint32_ts?

	// start and endpoints of line
	uint32_t xHi;
	uint32_t yHi;
	uint32_t xLo;
	uint32_t yLo;

	// info needed to draw the line
	uint32_t dx;
	uint32_t dy;

	int32_t inc;
	int32_t decideVar;

	uint32_t curX;
	uint32_t curY;

	bool lowSlope;
	
	
	line(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
	{
		if (y1 < y2)
		{
			xHi = x2;
			yHi = y2;
			xLo = x1;
			yLo = y1;
		}
		else
		{
			xHi = x1;
			yHi = y1;
			xLo = x2;
			yLo = y2;
		}
		curX = xLo;
		curY = yLo;

		inc = (xHi > xLo) ? 1 : -1;

		dx = (inc == 1) ? xHi - xLo: xLo - xHi;
		dy = yHi - yLo;

		// Note: slope calculations based on (y, x)
		//       inverse of usual (x, y) to accomodate drawing from Low y to High y)
		lowSlope = dx < dy;
		
		decideVar = lowSlope ? (2 * dx - dy) : (2 * dy - dx);
	}

	line(const point & p1, const point & p2)
	{
		if (p1.y < p2.y)
		{
			xHi = p2.x;
			yHi = p2.y;
			xLo = p1.x;
			yLo = p1.x;
		}
		else
		{
			xHi = p1.x;
			yHi = p1.y;
			xLo = p2.x;
			yLo = p2.y;
		}
		curX = xLo;
		curY = yLo;

		inc = (xHi > xLo) ? 1 : -1;

		dx = (inc == 1) ? xHi - xLo : xLo - xHi;
		dy = yHi - yLo;

		// Note: slope calculations based on (y, x)
		//       inverse of usual (x, y) to accomodate drawing from Low y to High y)
		lowSlope = dx < dy;

		decideVar = lowSlope ? (2 * dx - dy) : (2 * dy - dx);
	}
};


inline void setPixel(uint32_t *pixel, const rgb & color);
void setPixelXY(Winfo* window, uint32_t x, uint32_t y, const rgb & color);
void background(Winfo* window, const rgb & color);
void randomBackground(Winfo* window);
void drawHorizontalLine(Winfo* window, uint32_t x0, uint32_t x1, uint32_t y, const rgb & color);
void drawVerticalLine(Winfo* window, uint32_t x, uint32_t y0, uint32_t y1, const  rgb & color);
void drawSpreadVerticalLines(Winfo* window, int numberOfLines, const rgb & color);
void drawRectangle(Winfo* window, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const rgb & color);
void rasterizeRectangle(Winfo* window, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const rgb & color);
void drawTriangle(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb & color);
void rasterizeTriangle(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb & color);
void drawCircle(Winfo* window, uint32_t x, uint32_t y, uint32_t r, const rgb & color);
void rasterizeCircle(Winfo* window, uint32_t x, uint32_t y, uint32_t r, const rgb & color);
void drawPaddlesRegistrationScreen(Winfo* window, Player * players, int numberOfPlayers, const rgb & color);
void bLineNext(line& ln);
void drawLine(Winfo* window, line & ln, const rgb & color);