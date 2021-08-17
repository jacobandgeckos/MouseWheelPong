#pragma once
#include "MWPheader.h"
#include "MWPutil.h"
#include "MWPgameInfo.h"

struct point
{
	int32_t x;
	int32_t y;

	point()
	{

	}

	point(int32_t X, int32_t Y)
	{
		x = X;
		y = Y;
	}
};

// stores info for drawing a line from the Low y point to the High y point
class line
{
public:

	// start and endpoints of line
	int32_t xHi;
	int32_t yHi;
	int32_t xLo;
	int32_t yLo;

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

		dx = (inc == 1) ? xHi - xLo : xLo - xHi;
		dy = yHi - yLo;

		// Note: slope calculations based on (y, x)
		//       inverse of usual (x, y) to accomodate drawing from Low y to High y)
		lowSlope = dx < dy;

		startLine();
	}

	line(const point& p1, const point& p2)
	{
		if (p1.y < p2.y)
		{
			xHi = p2.x;
			yHi = p2.y;
			xLo = p1.x;
			yLo = p1.y;
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

		startLine();
	}

	void startLine()
	{
		decideVar = lowSlope ? (2 * dx - dy) : (2 * dy - dx);
	}
};


inline void setPixel(uint32_t* pixel, const rgb& color);
void setPixelXY(Winfo* window, uint32_t x, uint32_t y, const rgb& color);
void background(Winfo* window, const rgb& color);
void randomBackground(Winfo* window);
void drawHorizontalLine(Winfo* window, uint32_t x0, uint32_t x1, uint32_t y, const rgb& color);
void drawVerticalLine(Winfo* window, uint32_t x, uint32_t y0, uint32_t y1, const  rgb& color);
void drawSpreadVerticalLines(Winfo* window, int numberOfLines, const rgb& color);
void drawRectangle(Winfo* window, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const rgb& color);
void rasterizeRectangle(Winfo* window, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const rgb& color);
void drawTriangle(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb& color);
void rasterizeTriangle(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb& color);
void drawCircle(Winfo* window, uint32_t x, uint32_t y, uint32_t r, const rgb& color);
void rasterizeCircle(Winfo* window, uint32_t x, uint32_t y, uint32_t r, const rgb& color);
void drawPaddlesRegistrationScreen(Winfo* window, Player* players, int numberOfPlayers, const rgb& color);
void bLineNext(line& ln);
void drawLine(Winfo* window, line& ln, const rgb& color);
void fillTriangle(Winfo* window, const point& p1, const point& p2, const point& p3, const rgb& color);
void drawNGon(Winfo* window, const int N, const int radius, double angleOffset, const point& center, const rgb& color);
void drawQuadraticBCurveNaive(Winfo* window, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t cx1, uint32_t cy1, float step, const rgb& color);
bool xIntersection(int32_t xInt, line ln, point& intPoint);
bool yIntersection(int32_t yInt, line ln, point& intPoint);
LinkedList shClipToScreen(Winfo* window, LinkedList in_poly);
void drawPoly(Winfo* window, LinkedList poly, const rgb& color);
void fillPoly(Winfo* window, LinkedList poly, const rgb& color);

void drawPNG(Winfo* window, const point & topLeft, const PNG & p);