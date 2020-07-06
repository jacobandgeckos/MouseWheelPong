#include "MWPplayer.h"

std::vector<HANDLE> DetectPlayerMouseDisconnect(std::map<HANDLE, Player> & mouseMapping)
{
	for(std::map<HANDLE, Player>::iterator it =  mouseMapping.begin(); it != mouseMapping.end(); ++it)
	{
		it->second.isMouseConnected = false;
	}

	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0) { return std::vector<HANDLE>(); }
	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL) { return std::vector<HANDLE>(); }
	if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == ((UINT)-1)) { return std::vector<HANDLE>(); }
	// do the job...
	for (int i = 0; i < nDevices; ++i)
	{
		if (pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			//problem is that this also detects trackpad, maybe need a start screen where each player clicks to register themselves
			// then use list to detect disconnects
			if(mouseMapping.find(pRawInputDeviceList[i].hDevice) != mouseMapping.end())
			{
				mouseMapping[pRawInputDeviceList[i].hDevice].isMouseConnected = true;
			}
		}
	}
	free(pRawInputDeviceList);
	std::vector<HANDLE> outPlayers;
	for (std::map<HANDLE, Player>::iterator it = mouseMapping.begin(); it != mouseMapping.end(); ++it)
	{
		if (it->second.isMouseConnected == false)
			outPlayers.push_back(it->first);
	}
	return outPlayers;
}