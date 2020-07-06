#pragma once
#include "MWPheader.h"
#include "MWPdevice.h"

struct Player
{
	int playerNumber;
	int position;
	bool isMouseConnected;
};

std::vector<HANDLE> DetectPlayerMouseDisconnect(std::map<HANDLE, Player> & mouseMapping);