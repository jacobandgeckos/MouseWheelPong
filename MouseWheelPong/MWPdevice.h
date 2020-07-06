#pragma once
#include "MWPheader.h"

enum MWPdeviceType
{
	MWPMOUSE,
	MWPKBD
};

struct MWPmouse
{
	bool leftButtonPressed;
	bool rightButtonPressed;
	bool middleButtonPressed;
	int scrollAmount;
};

struct MWPdevice
{
	HANDLE deviceHandle;
	MWPdeviceType type;
	union
	{
		MWPmouse mouse;
	};
};


std::map<HANDLE,MWPdevice> createDeviceList();