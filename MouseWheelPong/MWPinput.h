#pragma once
#include "MWPheader.h"
#include "MWPdevice.h"

// struct KeyMappings;

struct KeyPresses
{
	HANDLE device;
	int playerNum; //maybe remove plz
	bool scrolledUp;
	bool scrolledDown;
	bool leftButtonUp;
	bool leftButtonDown;
	bool rightButtonUp;
	bool rightButtonDown;
	bool accept;
	bool quit;
};

KeyPresses ProcessEvents(std::map<HANDLE, MWPdevice> & devices);

