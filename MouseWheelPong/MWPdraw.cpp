#include "MWPdraw.h"
#include "MWPutil.h"


inline void setPixel(uint32_t *pixel, const rgb & color)
{
	// draws to an individual pixel
	// by looping through an amount of memory equal to the number of rows * columns

	/*              0  1  2  3
			 Pixel in mem: BB GG RR xx

			 loaded into reg
			 0x xxRRGGBB
	*/
	*pixel = ((color.red << 16) | (color.green << 8) | color.blue);
}

void setPixelXY(Winfo* window, uint32_t x, uint32_t y, const rgb & color)
{
	uint32_t* pixel = (uint32_t*)window->winPixMemory + window->winWidth*y + x;
	*pixel = ((color.red << 16) | (color.green << 8) | color.blue);
}

void background(Winfo* window, const rgb & color)
{
	//Draws to every pixel in memory, filling in the whole memory
	uint32_t* Pixel = (uint32_t*)window->winPixMemory;
	int pixels = window->winHeight * window->winWidth;

	for(int count = 0; count < pixels; ++count)
	{
		setPixel(Pixel, color);
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
		setPixel(Pixel, rgb((unsigned char)MWPrand() * 255, (unsigned char)MWPrand() * 255, (unsigned char)MWPrand() * 255));
		++Pixel;
	}
}

void drawHorizontalLine(Winfo* window, uint32_t x0, uint32_t x1, uint32_t y, const rgb & color)
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
		setPixel(Pixel, color);
		++Pixel;
	}
}

void drawVerticalLine(Winfo* window, uint32_t x, uint32_t y0, uint32_t y1, const rgb &color)
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
		setPixel(Pixel, color);
		Pixel += window->winWidth;
	}
}

// Takes in line 
void bLineNext(line & ln)
{
	// Reverse (y, x) Low Slope Line
	if(ln.lowSlope)
	{
		if (ln.decideVar > 0)
		{
			ln.curX += ln.inc;
			ln.decideVar -= 2 * ln.dy;
		}
		ln.decideVar += 2 * ln.dx;
		++ln.curY;
	}

	// Reverse (y, x)High Slope Line
	else
	{
		if (ln.decideVar > 0)
		{
			++ln.curY;
			ln.decideVar -= 2 * ln.dx;
		}
		ln.decideVar += 2 * ln.dy;
		ln.curX += ln.inc;
	}
}

void fillTriangle(Winfo* window, const point & p1, const point & p2, const point & p3, const rgb & color) {
	//First, set 3 points: high, mid and low.

	const point* vHi = &p1;
	const point* vMid = &p2;
	const point* vLo = &p3;

	if (vLo->y > vMid->y) 
	{
		std::swap(vLo, vMid);
	}
	if (vMid->y > vHi->y)
	{
		std::swap(vMid, vHi);
	}
	if (vLo->y > vMid->y)
	{
		std::swap(vLo, vMid);
	}

	// TODO: Add exceptions for when one or all sides of triangle are straight lines?

	line lHi(*vHi, *vMid);   
	line lLo(*vMid, *vLo);
	line lFull(*vHi, *vLo);

	// Currently resolved by drawHorizontalLine(), is that efficient?
	// Use this to tell if the middle height vertex is at the left or right of the triangle.
	// bool midpointLeft = ( ((vMid->x)*2) < ((vHi->x) + (vLo->x)) );

	uint32_t xLast1 = vHi->x;
	uint32_t xLast2= vHi->x;
	uint32_t yLast = vHi->y;

	while ( (lHi.curY != lHi.yLo) || (lHi.curX != lHi.xLo) )
	{
		// after y of each side advances by 1, draw the horizontal line between the points
		// (when 1 y advances first, wait for the other)
		// use line.lowSlope to see which line to advance first, and which line will need more iterations to advance to the next y
		

		// This won't quite work, what if both lines have a low y/x slope in the same direction?
		// TODO: account for cases where lines both have the same low slope 
		while (lHi.curY == yLast)
		{
			xLast1 = lHi.curX;
			bLineNext(lHi);
		}
		while (lFull.curY == yLast)
		{
			xLast2 = lFull.curX;
			bLineNext(lHi);
		}
		drawHorizontalLine(window, xLast1, xLast2, yLast, color);
		yLast = lFull.curY;
	}

	//posibly one horizontal Line in between, add a drawHorizontalLine here if it is missing

	while ((lLo.curY != lLo.yLo) || (lLo.curX != lLo.xLo))
	{

	}
}

void drawLine(Winfo* window, line & ln, const rgb & color)
{
	while ((ln.curY != ln.yHi) || (ln.curX != ln.xHi))
	{
		setPixelXY(window, ln.curX, ln.curY, color);
		bLineNext(ln);
	}
	setPixelXY(window, ln.curX, ln.curY, color);
}

void drawRectangle(Winfo* window, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const rgb & color)
{
	drawHorizontalLine(window, x, x+width, y, color);
	drawVerticalLine(window, y, y+height, x+width, color);
	drawHorizontalLine(window, x, x + width, y + height, color);
	drawVerticalLine(window, y, y + height, x, color);
}

void rasterizeRectangle(Winfo* window, uint32_t x, uint32_t y, uint32_t width, uint32_t height, const rgb & color)
{
	uint32_t* Pixel = ((uint32_t*)window->winPixMemory) + y * window->winWidth + x;

	for(uint32_t yindex = 0; yindex < height; ++yindex)
	{
		for(uint32_t xindex = 0; xindex < width; ++xindex)
		{
			setPixel(Pixel, color);
			++Pixel;
		}
		Pixel = ((uint32_t*)window->winPixMemory) + (y + yindex) * window->winWidth + x;
	}
}

void 
drawTriangle(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb & color)
{
	line curLine(x0, y0, x1, y1);	
	drawLine(window, curLine, color);
	curLine = line(x1, y1, x2, y2);
	drawLine(window, curLine, color);
	curLine = line(x2, y2, x0, y0);
	drawLine(window, curLine, color);
}

static void fillBottomFlatTriangle(Winfo* window, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb & color)
{
	/*
	//float invslope1 = ((float)(x1-x0))/((float)(y1-y0));  // Division and floats?
	//float invslope2 = ((float)(x2-x0))/((float)(y2-y0));  // Surely we don't need these.

	float currentx1 = x0;
	float currentx2 = x0;

	for(int lineY = y0; lineY <= y1; ++lineY)
	{
		DrawHorizontalLine(Buffer, ((int)currentx1), ((int)currentx2), lineY, red, green, blue);
		//BDrawLine(Buffer, ((int)currentx1), lineY, ((int)currentx2), lineY, red, green, blue);
		currentx1 += invslope1;
		currentx2 += invslope2;
	}


	bool leftLow;
	bool rightLow;
	if(abs(y1-y0)< abs(x1-x0)
	{
		leftLow = true;
	}
	else
	{
		leftLow = false;
	}
	if(abs(y2-y0)< abs(x2-x0)
	{
		rightLow = true;
	}
	else
	{
		rightLow = false;
	}
	*/
}

void 
rasterizeTriangle(Winfo* window, uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, const rgb & color)
{

}


void drawCircle(Winfo* window, uint32_t x, uint32_t y, uint32_t r, const rgb & color)
{
	uint32_t x0 = 0; 
	uint32_t y0 = r;
	int32_t d = 3 - 2*r;
	setPixelXY(window, x, y+r, color);
	setPixelXY(window, x+r, y, color);
	setPixelXY(window, x, y-r, color);
	setPixelXY(window, x-r, y, color);
	while(x0 <= y0)
	{
		if(d > 0)
		{
			d += 4*(x0-y0) + 10;
			--y0;
		}
		else
		{
			d += 4*x0 + 6;
		}
		++x0;
		setPixelXY(window, x+x0, y+y0, color);
		setPixelXY(window, x+y0, y+x0, color);
		setPixelXY(window, x+x0, y-y0, color);
		setPixelXY(window, x+y0, y-x0, color);
		setPixelXY(window, x-x0, y+y0, color);
		setPixelXY(window, x-y0, y+x0, color);
		setPixelXY(window, x-x0, y-y0, color);
		setPixelXY(window, x-y0, y-x0, color);
	}
}


void rasterizeCircle(Winfo* window, uint32_t x, uint32_t y, uint32_t r, const rgb & color)
{
	uint32_t x0 = 0; 
	uint32_t y0 = r;
	int32_t d = 3 - 2*r;
	while(x0 <= y0)
	{
		if(d > 0)
		{
			// I ruined this by moving x in front of y, because why would y ever go first?
			drawHorizontalLine(window, x+x0, x-x0, y+y0, color);
			drawVerticalLine(window, y-x0, y+x0, x+y0, color);
			drawHorizontalLine(window, x+x0, x-x0, y-y0, color);
			drawVerticalLine(window, y-x0, y+x0, x-y0, color);
			d += 4*(x0-y0) + 10;
			--y0;
		}
		else
		{
			d += 4 * x0 + 6;
		}
		++x0;
	}
	rasterizeRectangle(window, x-x0, y-y0, 2*x0, 2*x0, color);	
}

void drawSpreadVerticalLines(Winfo* window, int numberOfLines, const rgb & color)
{
	if (numberOfLines < 1)
		return;
	int spread = window->winWidth / (numberOfLines+1);
	for(int i  = 0; i < numberOfLines; ++i)
	{
		drawVerticalLine(window, 0, window->winHeight-1, (i+1)*spread ,color);
	}
}

void drawPaddlesRegistrationScreen(Winfo* window,Player * players, int numberOfPlayers, const rgb & color)
{
	if (numberOfPlayers < 1)
		return;
	int positionWidth = window->winWidth/numberOfPlayers;
	int halfPositionWidth = positionWidth/2;

	for(int i = 0; i < numberOfPlayers; ++i)
	{
		rasterizeRectangle(window, positionWidth*i + halfPositionWidth, (window->winHeight / 2) + players[i].position, 5, 15, color);
	}

}