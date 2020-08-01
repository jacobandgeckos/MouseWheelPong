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
	if (x < window->winWidth && y < window->winHeight)
	{
		uint32_t* pixel = (uint32_t*)window->winPixMemory + window->winWidth * y + x;
		*pixel = ((color.red << 16) | (color.green << 8) | color.blue);
	}
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
	if (y < window->winHeight)
	{
		int index;
		int endX;
		uint32_t* Pixel = ((uint32_t*)window->winPixMemory) + window->winWidth * y;

		if (x0 < x1)
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
		for (; index <= endX; ++index)
		{
			if (index < window->winWidth)
			{
				setPixel(Pixel, color);
			}
			++Pixel;
		}
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

	uint32_t xLast1 = vLo->x;
	uint32_t xLast2 = vLo->x;
	uint32_t yLast  = vLo->y;


	bool leftPointing = (((vMid->x) * 2) < ((vHi->x) + (vLo->x)));

	bool HiLoLeftSlope = (lLo.inc == -1);
	bool FullLeftSlope = (lFull.inc == -1);

	setPixelXY(window, vLo->x, vLo->y, color);

	//Triangle is getting wider during this while loop
	while ( !((lLo.curY == lLo.yHi) && (lLo.curX == lLo.xHi)) )
	{
		// after y of each side advances by 1, draw the horizontal line between the points of the previous y

		xLast1 = lLo.curX;
		bLineNext(lLo);

		while (lLo.curY == yLast && !((lLo.curY >= lLo.yHi) && (((lLo.inc == 1) && (lLo.curX >= lLo.xHi)) || ((lLo.inc == -1) && (lLo.curX <= lLo.xHi) ) ) ) )
		{
			if (leftPointing == HiLoLeftSlope)
			{
				xLast1 = lLo.curX;
			}
			bLineNext(lLo);
		}
		
		xLast2 = lFull.curX;
		bLineNext(lFull);

		while (lFull.curY == yLast && !( (lFull.curY >= lFull.yHi) && ( ( (lFull.inc == 1) && (lFull.curX >= lFull.xHi) ) || ( (lFull.inc == -1) && (lFull.curX <= lFull.xHi) ) ) ) )
		{
			if (leftPointing != FullLeftSlope)
			{
				xLast2 = lFull.curX;
			}
			bLineNext(lFull);
		}
		drawHorizontalLine(window, xLast1, xLast2, yLast, color);
		yLast = lFull.curY;
	}

	setPixelXY(window, vMid->x, vMid->y, color);
	HiLoLeftSlope = (lHi.inc == -1);

	// Triangle is getting narrower during this while loop
	while ( !((lHi.curY == lHi.yHi) && (lHi.curX == lHi.xHi)) )
	{
		xLast1 = lHi.curX;
		bLineNext(lHi);

		while ( (lHi.curY == yLast) && ! ( (lHi.curY >= lHi.yHi) && ( ( (lHi.inc == 1) && (lHi.curX >= lHi.xHi) ) || ( (lHi.inc == -1) && (lHi.curX <= lHi.xHi) ) ) ) )
		{
			if (leftPointing != HiLoLeftSlope)
			{
				xLast1 = lHi.curX;
			}
			bLineNext(lHi);
		}

		xLast2 = lFull.curX;
		bLineNext(lFull);
		
		while (lFull.curY == yLast && !( (lFull.curY >= lFull.yHi) && ( ( (lFull.inc == 1) && (lFull.curX >= lFull.xHi) ) || ( (lFull.inc == -1) && (lFull.curX <= lFull.xHi) ) ) ) )
		{
			if (leftPointing != FullLeftSlope)
			{
				xLast2 = lFull.curX;
			}
			bLineNext(lFull);
		}
		drawHorizontalLine(window, xLast1, xLast2, yLast, color);
		yLast = lFull.curY;
	}

	setPixelXY(window, vHi->x, vHi->y, color);
}

void drawLine(Winfo* window, line & ln, const rgb & color)
{
	ln.startLine();
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

void drawNGon(Winfo* window, const int N, const int radius, double angleOffset, const point& center, const rgb& color)
{
	double angleIncrements = 2.0* 3.14159265358979323846 / N;
	int currX = center.x+radius*cos(angleOffset); int currY = center.y+radius*sin(angleOffset);
	int prevX; int prevY;
	int firstX = currX;
	int secondX = currY;
	for (int i = 1; i < N; ++i)
	{
		prevX = currX;
		prevY = currY;
		currX = center.x + radius * cos(i* angleIncrements + angleOffset);
		currY = center.y + radius * sin(i* angleIncrements + angleOffset);
		line l(prevX, prevY, currX, currY);
		drawLine(window, l, color);
	}
	line l(currX, currY, firstX, secondX);
	drawLine(window, l, color);
}


void drawSpreadVerticalLines(Winfo* window, int numberOfLines, const rgb & color)
{
	if (numberOfLines < 1)
		return;
	int spread = window->winWidth / (numberOfLines+1);
	for(int i  = 0; i < numberOfLines; ++i)
	{
		drawVerticalLine(window, (i + 1)*spread, 0,  window->winHeight-1 ,color);
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

/* x1 y1 is starting point, x2 y2 is end point, cx1 cy1 is the control point*/
void drawQuadraticBCurveNaive(Winfo* window, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t cx1, uint32_t cy1, float step, const rgb& color)
{
	//B(t) = (1-t)[(1-t)P0+tP1] + t[(1-t)P1+tP2] , 0<=t<=1
	// simplify to:
	//B(t) = P1 + (1-t)^2(P0-P1)+t^2(P2-P1) , 0<=t<=1
	int px1 = x1 - cx1;
	int py1 = y1 - cy1;
	int px2 = x2 - cx1;
	int py2 = y2 - cy1;
	for (float t = 0; t <= 1.0; t += step)
	{
		float oneMinT = 1 - t;
		int bx = cx1 + (oneMinT * oneMinT * px1) + (t * t * px2);
		int by = cy1 + (oneMinT * oneMinT * py1) + (t * t * py2);
		setPixelXY(window, bx, by, color);
	}
}