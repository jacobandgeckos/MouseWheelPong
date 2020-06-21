#pragma once
#include "MWPheader.h"

struct KeyMappings
{
	int up;
	int down;
	int left;
	int right;
	int hit;
};

struct KeyPresses
{
	int playerNum;
	bool isUpPressed;
	bool isDownPressed;
	bool isLeftPressed;
	bool isRightPressed;
	bool isHitPressed;
};

void ProcessEvents();


KeyMappings keys = {'W','S','A','D', VK_SPACE};
