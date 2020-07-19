#pragma once
#include "MWPheader.h"
#include "MWPdevice.h"
#include "MWPplayer.h"
#include "MWPdraw.h"


struct gameInfo
{
	int playerCount = 0;
	int paddleWidth = 0;
	int paddleHeight = 0;
	std::map<HANDLE, MWPdevice> deviceList;
	Player players[MAX_PLAYERS];
};

void initGameInfo(gameInfo * gI);
std::vector<HANDLE> DetectPlayerMouseConnectsAndDisconnect(gameInfo *gI);
void RemoveDisconnectedPlayers(std::vector<HANDLE> disconnectedList, gameInfo *gI);
