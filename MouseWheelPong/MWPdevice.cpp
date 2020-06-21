#include "MWPdevice.h"

std::map<HANDLE,MWPdevice> createDeviceList()
{
	std::map<HANDLE, MWPdevice> out;

	UINT nDevices;
	PRAWINPUTDEVICELIST pRawInputDeviceList;
	if (GetRawInputDeviceList(NULL, &nDevices, sizeof(RAWINPUTDEVICELIST)) != 0)
	{
		return out;
	}
	if ((pRawInputDeviceList = (PRAWINPUTDEVICELIST)malloc(sizeof(RAWINPUTDEVICELIST) * nDevices)) == NULL)
	{
		return out;
	}
	if (GetRawInputDeviceList(pRawInputDeviceList, &nDevices, sizeof(RAWINPUTDEVICELIST)) == ((UINT)-1)) 
	{ 
		return out; 
    }
	// do the job...
	for (unsigned int i = 0; i < nDevices; ++i)
	{
		if (pRawInputDeviceList[i].dwType == RIM_TYPEMOUSE)
		{
			MWPdevice mouse = {0};
			mouse.deviceHandle = pRawInputDeviceList[i].hDevice;
			mouse.type = MWPMOUSE;
			out[pRawInputDeviceList[i].hDevice] = mouse;
		}
	}
	free(pRawInputDeviceList);
	return out;
}