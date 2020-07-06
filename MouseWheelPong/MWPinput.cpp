#include "MWPinput.h"

/*
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

KeyMappings keys = { 'W','S','A','D', VK_SPACE };

*/


extern bool running;

KeyPresses ProcessEvents(std::map<HANDLE, MWPdevice> & devices)
{
	MSG event;
	KeyPresses outputPresses = {0};
	while (PeekMessage(&event, 0, 0, 0, PM_REMOVE))
	{
		switch (event.message)
		{

		case WM_INPUT:
		{
			/* only registeres HIDs will go here*/
			UINT dwSize;
			GetRawInputData((HRAWINPUT)event.lParam, RID_INPUT, NULL, &dwSize,
				sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize]; //maybe want to look into creating static buffer to reduce new overhead
			GetRawInputData((HRAWINPUT)event.lParam, RID_INPUT, lpb, &dwSize,
				sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				outputPresses.device = raw->header.hDevice;
				if(devices.find(raw->header.hDevice) == devices.end())
				{
					MWPdevice mouse = { 0 };
					mouse.deviceHandle = raw->header.hDevice;
					mouse.type = MWPMOUSE;
					devices[raw->header.hDevice] = mouse;
				}
				if ((raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) == RI_MOUSE_WHEEL)
				{
					int scrolled = devices[raw->header.hDevice].mouse.scrollAmount + (short)(raw->data.mouse.usButtonData);
					int up = (scrolled / WHEEL_DELTA);
					if(up > 0)
					{
						outputPresses.scrolledUp = true;
					}
					else if(up < 0)
					{
						outputPresses.scrolledDown = true;
					}
					//devices[raw->header.hDevice].mouse.scrollAmount = scrolled % WHEEL_DELTA;
					devices[raw->header.hDevice].mouse.scrollAmount = 0;
					/*
					std::wstringstream s;
					s << "Mouse: " << raw->header.hDevice  //how to differentiate mice
						<< " scrolled " << scroll
						<< " amount " << scroll % WHEEL_DELTA << std::endl; //scroll amount
					OutputDebugString(s.str().c_str());
					*/
				}
				else if((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) == RI_MOUSE_LEFT_BUTTON_DOWN)
				{
					outputPresses.leftButtonDown = true;
				}
				else if((raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) == RI_MOUSE_LEFT_BUTTON_UP)
				{
					outputPresses.leftButtonUp = true;
				}
				else if((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) == RI_MOUSE_RIGHT_BUTTON_DOWN)
				{
					outputPresses.rightButtonDown = true;
				}
				else if((raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) == RI_MOUSE_RIGHT_BUTTON_UP)
				{
					outputPresses.rightButtonUp = true;
				}
			}
			delete[] lpb;
			return outputPresses;
		}
		case WM_QUIT:
		case WM_CLOSE:
		{
			outputPresses.quit = true;
			running = false;
			break;
		}

		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			unsigned int VKCode = (unsigned int)event.wParam;
			int WasDown = (event.lParam & (1 << 30)) != 0;
			int IsDown = (event.lParam & (1 << 31)) == 0;
			if (WasDown != IsDown)
			{

				if (VKCode == 'W')
				{

				}
				else if (VKCode == 'S')
				{

				}
				else if (VKCode == 'A')
				{

				}
				else if (VKCode == 'D')
				{

				}
				else if (VKCode == 'Q')
				{
					
				}
				else if (VKCode == 'E')
				{
					
				}
				else if (VKCode == 'X')
				{
					outputPresses.quit = true;
				}
				else if (VKCode == VK_UP)
				{

				}
				else if (VKCode == VK_DOWN)
				{

				}
				else if (VKCode == VK_RIGHT)
				{

				}
				else if (VKCode == VK_LEFT)
				{

				}
				else if (VKCode == VK_RETURN)
				{
					outputPresses.accept = true;
				}
				else if (VKCode == VK_ESCAPE)
				{
					running = false;
				}
				else if (VKCode == VK_SPACE)
				{

				}
			}
			int AltKeyWasDown = (event.lParam & (1 << 29));
			if (VKCode == VK_F4 && AltKeyWasDown)
			{
				outputPresses.quit = true;
				running = false;
			}
			return outputPresses;
		}
		default:
		{
			TranslateMessage(&event);
			DispatchMessage(&event);
		}
		}
	}
	return outputPresses;
}