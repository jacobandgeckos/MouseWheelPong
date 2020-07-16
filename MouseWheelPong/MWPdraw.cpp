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

void drawVerticalLine(Winfo* window, uint32_t y0, uint32_t y1, uint32_t x, const rgb &color)
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
	// TODO: Change line struct to accept 2 const point structs instead of 4 uint32_ts?

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

void fillTriangle(const point & vertex1, const point & vertex2, const point & vertex3) {
	//First, set 3 points: high, mid and low.
	//point* vHi;
	//point* vMid;
	//point* vLo;

	// just make these lines instead?
	// TODO: Change line struct to accept 2 const point structs instead of 4 uint32_ts?

	// TODO: sort/set point pointers, or sort points into lines

	//line lHi(vHi, vMid);   
	//line lLo(vMid, vLo);   // declare this later?, or assign it's value to lHi (rename to lHalf?) after it's done being used on the first half.
	//line lFull(vHi, vMid);

	//will there be a left and right version?
	//bool midLeft = vMid->x <= vHi->x; // is this the right de-pointerizing syntax?  Will it help us always start lines from the left?

	//can left/right be resolved by drawHorizontalLine?


	//uint32_t lastLeftX = vHi->x;
	//uint32_t lastRightX = vHi->x;

	while (true/*between high and mid y*/)
	{
		// after y of each side advances by 1, draw the horizontal line between the points
		// (when 1 y advances first, wait for the other)

	}
	//posibly one horizontal Line in between, not sure
	while (true/*between mid and low y*/)
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
	float invslope1 = ((float)(x1-x0))/((float)(y1-y0));
	float invslope2 = ((float)(x2-x0))/((float)(y2-y0));

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