#pragma once
#include "MWPheader.h"
#include "MWPdevice.h"


struct Player
{
	//playerNumber will just be the index into the array of players now
	int position;
	HANDLE device;
	//int score;
};
