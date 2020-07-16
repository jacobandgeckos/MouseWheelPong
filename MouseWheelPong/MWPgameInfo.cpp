#include "MWPgameInfo.h"


void initGameInfo(gameInfo * gI)
{
	gI->paddleWidth = 5;
	gI->paddleHeight = 15;
	gI->playerCount = 0;
	createDeviceList(gI->deviceList);
}

std::vector<int> DetectPlayerMouseConnectsAndDisconnect(gameInfo *gI)
{

	for (int i = 0; i < gI->playerCount; ++i)
	{
		gI->players[i].isMouseConnected = false;
	}

	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) { return std::vector<int>(); }
	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) { return std::vector<int>(); }
	if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == ((UINT)-1)) { return std::vector<int>(); }
	// do the job...
	for (unsigned int i = 0; i < nDevices; ++i)
	{
		if (pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			//use list to detect disconnects
			if (gI->deviceList.find(pRawInputDeviceList[i].hDevice) != gI->deviceList.end())
			{
				if(gI->deviceList[pRawInputDeviceList[i].hDevice].assignedPlayer < MAX_PLAYERS)
					gI->players[gI->deviceList[pRawInputDeviceList[i].hDevice].assignedPlayer].isMouseConnected = true;
			}
			else
			{
				createDevice(gI->deviceList, MAX_PLAYERS + 1, pRawInputDeviceList[i].hDevice, MWPMOUSE);
			}
		}
	}
	free(pRawInputDeviceList);
	std::vector<int> outPlayers;

	for (int i = 0; i < gI->playerCount; ++i)
	{
		if (gI->players[i].isMouseConnected == false)
			outPlayers.push_back(i);
	}

	return outPlayers;
}

void RemoveDisconnectedPlayers(std::vector<int> disconnectedList, gameInfo *gI)
{
	for (unsigned int i = 0; i < disconnectedList.size(); ++i)
	{
		if (disconnectedList[i] == gI->playerCount - 1)
		{
			gI->deviceList.erase(gI->reverseDeviceList[disconnectedList[i]]);
			gI->reverseDeviceList.erase(disconnectedList[i]);
		}
		else
		{
			HANDLE lastDevice = gI->reverseDeviceList[gI->playerCount - 1];
			gI->reverseDeviceList[disconnectedList[i]] = lastDevice;
			gI->players[disconnectedList[i]] = gI->players[gI->playerCount - 1];
			gI->deviceList[lastDevice].assignedPlayer = disconnectedList[i];
			gI->deviceList.erase(gI->reverseDeviceList[disconnectedList[i]]);
			gI->reverseDeviceList.erase(gI->playerCount - 1);
		}
		--gI->playerCount;
	}
}