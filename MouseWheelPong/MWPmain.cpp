#include "MWPheader.h"
#include "MWPinput.h"
#include "MWPutil.h"
#include "MWPdraw.h"
#include "MWPdevice.h"
#include "MWPplayer.h"
#include "MWPgameInfo.h"
#include "MWPstateMachine.h"
#include "MWPregistration.h"




// our window
// Note: globals (anything outside of a function) are initialized to zero automatically)
Winfo globalWindow;
gameInfo GlobalGameInfo;
enum MWP_States MWP_State;
RegistrationScreenAttributes regScreen;

// This GUID is for all USB serial host PnP drivers, but you can replace it 
// with any valid device class guid.
GUID WceusbshGUID = { 0x4d1e55b2, 0xf16f, 0x11cf, { 0x88, 0xcb, 0x00, 0x11, 0x11, 0x00, 0x00, 0x30} };

HDEVNOTIFY hDeviceNotify;

int globalWinWidth = 800;
int globalWinHeight = 600;

bool running = true;

int CALLBACK WinMain(
	HINSTANCE ProgramInstance,
	HINSTANCE ParentProgramInstance,
	LPSTR Command,
	int ShowCode
)
{
	WNDCLASS WindowClass = { 0 };

	winfoInit(&globalWindow, globalWinWidth, globalWinHeight);

	// style is the device context (our area of the window that we can access without having to request permission every time we use it) 
	// + enable horizontal and vertical resize callbacks
	WindowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;  // look at the these Redraw Events
	WindowClass.lpfnWndProc = eventHandler;  // use this event handler to handle those redraw events

	// Associates our window with our program
	WindowClass.hInstance = ProgramInstance;

	// Giving the name of our window class in unicode.
	std::wstring className = L"MWPwindowClass";
	WindowClass.lpszClassName = className.c_str();

	RegisterClass(&WindowClass);

	initializeMiceToRawInput();
	initGameInfo(&GlobalGameInfo);
	initRegistrationAttributes(&regScreen);

	struct PNG p = loadPNG("test.png");


	// Make an actual window ?
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	// Note on window dimensions: Height = titlebar + (2*border + shadow); Width = 2*(border+shadow); Guesstimate: titlebar=30, border=1, shadow = 8.  Find programatically?
	HWND windowHandle = CreateWindowEx(0, WindowClass.lpszClassName, L"Mouse Wheel Pong", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, globalWindow.winWidth + 16, globalWindow.winHeight + 39, 0, 0, ProgramInstance, 0);

	if (!DoRegisterDeviceInterfaceToHwnd(
		WceusbshGUID,
		windowHandle,
		&hDeviceNotify))
	{
		// Terminate on failure.
		//ErrorHandler(TEXT("DoRegisterDeviceInterfaceToHwnd"));
		return -1;
	}

	HDC MonitorContext = GetDC(windowHandle);

	//setMWPSeeds(MWPrand() * 1000, MWPrand() * 1000, MWPrand() * 1000);
	//randomBackground(&globalWindow);

	rgb BackgroundColor(255, 0, 0);

	background(&globalWindow, BackgroundColor);

	MWP_State = MWP_REGISTRATION;
	


	while (running)
	{
		KeyPresses key = ProcessEvents(GlobalGameInfo.deviceList);
		if(ExecuteMWPStateMachine(&key) == 1)
		{
			running = false;
		}
		updateScreen(windowHandle, MonitorContext);
		
	}

	return 42;
}

LRESULT CALLBACK eventHandler(HWND winHandle, UINT eventType, WPARAM inputW, LPARAM inputM)
{
	//try to process events in MWPinput, only put cases in here if they can only be intercepted at the window level

	switch (eventType)
	{
	case WM_DEVICECHANGE:
	{
		switch (inputW)
		{
		case DBT_DEVICEARRIVAL:
			//OutputDebugStringA("Device arrived!");

			break;
		case DBT_DEVICEREMOVECOMPLETE:
		{
			std::vector<HANDLE> disconnectedPlayer = DetectPlayerMouseConnectsAndDisconnect(&GlobalGameInfo);
			if (disconnectedPlayer.size() > 0)
			{
				RemoveDisconnectedPlayers(disconnectedPlayer, &GlobalGameInfo);
				background(&globalWindow, rgb(255, 0, 0));
				drawSpreadVerticalLines(&globalWindow, GlobalGameInfo.playerCount - 1, rgb(255, 255, 0));
				drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, rgb(0, 0, 0));
			}
			//need a way to remove disconnected mice that were not registered to a player
		}

			break;
		default:
			break;
		}
		break;
	}
	case WM_SIZE:
	{
		break;
	}

	case WM_CLOSE:
	{
		running = false;
		break;
	}

	case WM_DESTROY:
	{
		break;
	}


	case WM_ACTIVATEAPP:
	{
		// inputW is false for deactivated, and true for activated
		// inputM is the identifier of the thread that owns the window
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT paintInfo;
		HDC winDeviceContext = BeginPaint(winHandle, &paintInfo);
		//unused
		//int newPaintHeight = paintInfo.rcPaint.bottom - paintInfo.rcPaint.top;
		//int newPaintWidth = paintInfo.rcPaint.right - paintInfo.rcPaint.left;
		RECT clientRect;
		GetClientRect(winHandle, &clientRect);

		int newHeight = clientRect.bottom - clientRect.top;
		int newWidth = clientRect.right - clientRect.left;

		// copies pixel data from our memory to the screen's memory
		// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-stretchdibits
		StretchDIBits(winDeviceContext, 0, 0, newWidth, newHeight, 0, 0, globalWindow.winWidth, globalWindow.winHeight, globalWindow.winPixMemory, &(globalWindow.winProperties), DIB_RGB_COLORS, SRCCOPY);
		EndPaint(winHandle, &paintInfo);
		break;
	}

	default:
	{
		return DefWindowProc(winHandle, eventType, inputW, inputM);
	}
}

return 0;
}

// initiallizes our window with width and height
void winfoInit(Winfo* inWindow, int winWidth, int winHeight)
{
	if (inWindow->winPixMemory != NULL)
	{
		VirtualFree(inWindow->winPixMemory, WIN_MEM_START, MEM_RELEASE);
	}

	inWindow->winWidth = winWidth;
	inWindow->winHeight = winHeight;

	inWindow->bytesPerPixel = 4; // 4 is for RGBA
	inWindow->bytesPerRow = winWidth * inWindow->bytesPerPixel;

	// Allocate Memory for each pixel of our window at address zero
	inWindow->winPixMemory = VirtualAlloc(WIN_MEM_START, inWindow->bytesPerRow * winHeight, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);


	// Set Window Information
	inWindow->winProperties.bmiHeader.biSize = sizeof(inWindow->winProperties.bmiHeader);

	inWindow->winProperties.bmiHeader.biWidth = winWidth;
	inWindow->winProperties.bmiHeader.biHeight = -winHeight;

	inWindow->winProperties.bmiHeader.biPlanes = 1; //Deprecated

	inWindow->winProperties.bmiHeader.biBitCount = inWindow->bytesPerPixel * 8; // 8 bits are in a byte

	inWindow->winProperties.bmiHeader.biCompression = BI_RGB; // RGB, no compression
}


void initializeMiceToRawInput()
{
	RAWINPUTDEVICE rawMice;
	rawMice.usUsagePage = 0x01;
	rawMice.usUsage = 0x02;
	rawMice.dwFlags = 0;
	rawMice.hwndTarget = 0;


	RegisterRawInputDevices(&rawMice, 1, sizeof(rawMice));
}


// used to get resolution info for window sizing.
// TODO: complete this function to allow for better fullscreen and window sizing
// also we will have to prototype this in the header
void getMonitorInfo(MONITORINFO* monitorDim)
{
	// for fullscreen, see these sources:
	// https://stackoverflow.com/questions/2382464/win32-full-screen-and-hiding-taskbar
	// https://docs.microsoft.com/en-us/windows/win32/learnwin32/creating-a-window
	// https://stackoverflow.com/questions/28968012/win32-add-black-borders-to-fullscreen-window
}

inline void updateScreen(HWND windowHandle, HDC MonitorContext)
{
	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);

	int newHeight = clientRect.bottom - clientRect.top;
	int newWidth = clientRect.right - clientRect.left;
	//Draw to the screen
	StretchDIBits(MonitorContext, 0, 0, newWidth, newHeight, 0, 0, globalWindow.winWidth, globalWindow.winHeight, globalWindow.winPixMemory, &(globalWindow.winProperties), DIB_RGB_COLORS, SRCCOPY);
}

//
// DoRegisterDeviceInterfaceToHwnd
//
bool DoRegisterDeviceInterfaceToHwnd(
	GUID InterfaceClassGuid,
	HWND hWnd,
	HDEVNOTIFY* hDeviceNotify
)
// Routine Description:
//     Registers an HWND for notification of changes in the device interfaces
//     for the specified interface class GUID. 

// Parameters:
//     InterfaceClassGuid - The interface class GUID for the device 
//         interfaces. 

//     hWnd - Window handle to receive notifications.

//     hDeviceNotify - Receives the device notification handle. On failure, 
//         this value is NULL.

// Return Value:
//     If the function succeeds, the return value is TRUE.
//     If the function fails, the return value is FALSE.

// Note:
//     RegisterDeviceNotification also allows a service handle be used,
//     so a similar wrapper function to this one supporting that scenario
//     could be made from this template.
{
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;

	ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	NotificationFilter.dbcc_classguid = InterfaceClassGuid;

	*hDeviceNotify = RegisterDeviceNotification(
		hWnd,                       // events recipient
		&NotificationFilter,        // type of device
		DEVICE_NOTIFY_WINDOW_HANDLE // type of recipient handle
	);

	if (NULL == *hDeviceNotify)
	{
		return FALSE;
	}

	return TRUE;
}