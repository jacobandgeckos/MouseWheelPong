#include "MWPgameInfo.h"


void initGameInfo(gameInfo * gI)
{
	gI->paddleWidth = 5;
	gI->paddleHeight = 15;
	gI->playerCount = 0;
	createDeviceList(gI->deviceList);
}

std::vector<HANDLE> DetectPlayerMouseConnectsAndDisconnect(gameInfo *gI)
{
	for (std::map<HANDLE, MWPdevice>::iterator it =  gI->deviceList.begin() ; it != gI->deviceList.end(); ++it)
	{
		it->second.isMouseConnected = false;
	}
	

	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) { return std::vector<HANDLE>(); }
	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) { return std::vector<HANDLE>(); }
	if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == ((UINT)-1)) { return std::vector<HANDLE>(); }
	// do the job...
	for (unsigned int i = 0; i < nDevices; ++i)
	{
		if (pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			//use list to detect disconnects
			if (gI->deviceList.find(pRawInputDeviceList[i].hDevice) != gI->deviceList.end())
			{
				gI->deviceList[pRawInputDeviceList[i].hDevice].isMouseConnected = true;	
			}
			else
			{
				createDevice(gI->deviceList, MAX_PLAYERS + 1, pRawInputDeviceList[i].hDevice, MWPMOUSE);
			}
		}
	}
	free(pRawInputDeviceList);
	std::vector<HANDLE> outPlayers;

	std::map<HANDLE, MWPdevice>::iterator it = gI->deviceList.begin();
	while (it != gI->deviceList.end())
	{
		if (it->second.isMouseConnected == false)
		{
			if (it->second.assignedPlayer < MAX_PLAYERS)
			{
				outPlayers.push_back(it->first);
				++it;
			}
			else
			{
				//make sure all disconnected unregistered mice are erased here (I don't think there is anywhere else we need to remove them)
				//also make sure any unregistered mice that are still connected are not erased
				//then you can delete these comments
				gI->deviceList.erase(it++);
			}
		}
		else
			++it;
	}


	return outPlayers;
}

void RemoveDisconnectedPlayers(std::vector<HANDLE> disconnectedList, gameInfo *gI)
{
	for (unsigned int i = 0; i < disconnectedList.size(); ++i)
	{
		int playerNum = gI->deviceList[disconnectedList[i]].assignedPlayer;
		if (playerNum == gI->playerCount - 1)
		{
			gI->deviceList.erase(disconnectedList[i]);
		}
		else
		{
			HANDLE lastDevice = gI->players[gI->playerCount - 1].device;
			gI->deviceList[lastDevice].assignedPlayer = playerNum;
			gI->players[playerNum] = gI->players[gI->playerCount - 1];
			gI->deviceList.erase(disconnectedList[i]);
		}
		--gI->playerCount;
	}
}