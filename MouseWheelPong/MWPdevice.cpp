#include "MWPdevice.h"

void createDevice(std::map<HANDLE, MWPdevice>& out, int playerNumber, HANDLE deviceHandle, MWPdeviceType type)
{
	MWPdevice device = { 0 };
	device.assignedPlayer = playerNumber; //assign number greater than the max number of players
	device.deviceHandle = deviceHandle;
	device.isMouseConnected = true;
	device.type = type;
	out[deviceHandle] = device;
}


void registerDevice(std::map<HANDLE, MWPdevice>& out,int playerNumber, HANDLE deviceHandle)
{
	out[deviceHandle].assignedPlayer = playerNumber;
}


void createDeviceList(std::map<HANDLE, MWPdevice>& out)
{
	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0)
	{
		return;
	}
	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL)
	{
		return;
	}
	if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == ((UINT)-1)) 
	{ 
		return; 
    }
	// do the job...
	for (unsigned int i = 0; i < nDevices; ++i)
	{
		if (pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			createDevice(out, MAX_PLAYERS + 1, pRawInputDeviceList[i].hDevice, MWPMOUSE);
		}
	}
	free(pRawInputDeviceList);
}