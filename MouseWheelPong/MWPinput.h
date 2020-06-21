#pragma once
#include "MWPheader.h"

// struct KeyMappings;

struct KeyPresses
{
	HANDLE device;
	int playerNum;
	int scrolled;
	bool leftButtonUp;
	bool leftButtonDown;
	bool rightButtonUp;
	bool rightButtonDown;
	bool quit;
};

KeyPresses ProcessEvents();

