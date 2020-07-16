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
	unsigned int assignedPlayer;
	union
	{
		MWPmouse mouse;
	};
};


void createDeviceList(std::map<HANDLE, MWPdevice>& out);
//create a device first then register it
void createDevice(std::map<HANDLE, MWPdevice>& out, int playerNumber, HANDLE deviceHandle, MWPdeviceType type);
void registerDevice(std::map<HANDLE, MWPdevice>& out, std::map<int, HANDLE>& reverseOut, int playerNumber, HANDLE deviceHandle);