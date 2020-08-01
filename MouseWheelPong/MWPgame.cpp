#include "MWPgame.h"

extern Winfo globalWindow;
extern gameInfo GlobalGameInfo;

extern int globalWinWidth;
extern int globalWinHeight;

// Animation Track
line ani1(0, 0, 800, 600);
line ani2(0, 100, 800, 400);
line ani3(200, 100, 100, 600);

bool blinkOn = false;

void Game(KeyPresses * key)
{
	static int everyCertainFrame = 0;
	static int everyCertainFrameCount = 10;

	static int srceen_x_it = 0;
	static int srceen_y_it = 0;

	rgb BackgroundColor(255, 0, 0);
	background(&globalWindow, BackgroundColor);

	fillTriangle(&globalWindow, point(125, 200), point(580, 120), point(230, 568), rgb(0, 0, 0));
	if (blinkOn)
	{
		drawTriangle(&globalWindow, 125, 200, 580, 120, 230, 568, rgb(0, 250, 0));
	}

	// Animation Track
	fillTriangle(&globalWindow, point(ani1.curX, ani1.curY), point(ani2.curX, ani2.curY), point(ani3.curX, ani3.curY), rgb(0, 0, 0));
	if (blinkOn)
	{
		drawTriangle(&globalWindow, ani1.curX, ani1.curY, ani2.curX, ani2.curY, ani3.curX, ani3.curY, rgb(0,250,0));
	}
	bLineNext(ani1);
	bLineNext(ani2);
	bLineNext(ani3);

	line ln1(0, 0, 800, 600);
	line ln2(0, 100, 800, 400);
	line ln3(200, 100, 100, 600);
	drawLine(&globalWindow, ln1, rgb(0, 0, 135));
	drawLine(&globalWindow, ln2, rgb(0, 0, 135));
	drawLine(&globalWindow, ln3, rgb(0, 0, 135));
	
	srceen_x_it = (srceen_x_it + 1) % globalWindow.winWidth;
	srceen_y_it = (srceen_y_it + 1) % globalWindow.winHeight;
	drawQuadraticBCurveNaive(&globalWindow, 10, 300, 300, 10, srceen_x_it, srceen_y_it, 0.0001, rgb(0, 255, 0));
	//drawNGon(&globalWindow, 15, min(globalWindow.winWidth/2, globalWindow.winHeight/2)-1, 0, point(globalWindow.winWidth/2, globalWindow.winHeight/2), rgb(0, 255, 0));

	rasterizeCircle(&globalWindow, 300, 300, 50, rgb(0, 0, 255));

	//write to global buffer here

	if (everyCertainFrame == (everyCertainFrameCount - 1))
	{
		blinkOn = !blinkOn;

	}
	everyCertainFrame = (everyCertainFrame + 1) % everyCertainFrameCount;
}