#pragma once
#include "MWPheader.h"
#include "MWPdevice.h"
#include "MWPplayer.h"

struct gameInfo
{
	int playerCount = 0;
	int paddleWidth = 0;
	int paddleHeight = 0;
	std::map<HANDLE, MWPdevice> deviceList;
	std::map<int, HANDLE> reverseDeviceList;
	Player players[MAX_PLAYERS];
};

void initGameInfo(gameInfo * gI);
std::vector<int> DetectPlayerMouseConnectsAndDisconnect(gameInfo *gI);
void RemoveDisconnectedPlayers(std::vector<int> disconnectedList, gameInfo *gI);
