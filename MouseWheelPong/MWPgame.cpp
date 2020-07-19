#include "MWPgame.h"

extern Winfo globalWindow;
extern gameInfo GlobalGameInfo;

extern int globalWinWidth;
extern int globalWinHeight;


void Game(KeyPresses * key)
{
	static int everyCertainFrame = 0;
	static int everyCertainFrameCount = 10;

	//rgb BackgroundColor(255, 0, 0);
	background(&globalWindow, rgb(255, 70, 70));

	fillTriangle(&globalWindow, point(50, 50), point(50, 150), point(100, 200), rgb(0, 0, 0));

	rasterizeCircle(&globalWindow, 300, 300, 50, rgb(0, 0, 255));

	//write to global buffer here

	if (everyCertainFrame == (everyCertainFrameCount - 1))
	{


	}
	everyCertainFrame = (everyCertainFrame + 1) % everyCertainFrameCount;
}