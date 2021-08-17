#include "MWPgame.h"

extern Winfo globalWindow;
extern gameInfo GlobalGameInfo;

extern int globalWinWidth;
extern int globalWinHeight;

extern struct PNG p;
extern struct PNG p2;

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

	drawPNG(&globalWindow, point(0, 0), p);

	LinkedList poly1 = createLinkedList();
	addTailLL(&poly1, new point(40, -5));
	addTailLL(&poly1, new point(60, -5));
	addTailLL(&poly1, new point(80, 40));
	addTailLL(&poly1, new point(20, 40));

	LinkedList poly2 = shClipToScreen(&globalWindow, poly1);

	drawPoly(&globalWindow, poly2, rgb(0, 255, 255));


	LinkedList poly3 = createLinkedList();
	addTailLL(&poly3, new point(100, 20));
	addTailLL(&poly3, new point(50, 30));
	addTailLL(&poly3, new point(70, 70));
	addTailLL(&poly3, new point(40, 90));
	addTailLL(&poly3, new point(110, 120));
	addTailLL(&poly3, new point(130, 80));
	addTailLL(&poly3, new point(120, 50));

	rgb p3color(0, 0, 0);
	fillPoly(&globalWindow, poly3, p3color);
	drawPoly(&globalWindow, poly3, rgb(0, 255, 255));



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