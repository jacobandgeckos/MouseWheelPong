#include "MWPgameInfo.h"


void initGameInfo(gameInfo * gI)
{
	gI->paddleWidth = 5;
	gI->paddleHeight = 15;
	gI->playerCount = 1;
	gI->deviceList = createDeviceList();
}