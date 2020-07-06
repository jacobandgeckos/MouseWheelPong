#pragma once
#include "MWPheader.h"
#include "MWPdevice.h"
#include "MWPplayer.h"

struct gameInfo
{
	int playerCount;
	int paddleWidth;
	int paddleHeight;
	std::map<HANDLE, MWPdevice> deviceList;
	std::map<HANDLE, Player> mouseMapping;
};

void initGameInfo(gameInfo * gI);
