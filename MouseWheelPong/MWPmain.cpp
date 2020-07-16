#include "MWPheader.h"
#include "MWPinput.h"
#include "MWPutil.h"
#include "MWPdraw.h"
#include "MWPdevice.h"
#include "MWPplayer.h"
#include "MWPgameInfo.h"



// our window
// Note: globals (anything outside of a function) are initialized to zero automatically)
Winfo globalWindow;
gameInfo GlobalGameInfo;

int globalWinWidth = 800;
int globalWinHeight = 600;

bool running = true;
bool startup = true;

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
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;  // look at the these Redraw Events
	WindowClass.lpfnWndProc = eventHandler;  // use this event handler to handle those redraw events

	// Associates our window with our program
	WindowClass.hInstance = ProgramInstance;

	// Giving the name of our window class in unicode.
	std::wstring className = L"MWPwindowClass";
	WindowClass.lpszClassName = className.c_str();

	RegisterClass( & WindowClass );

	initializeMiceToRawInput();
	initGameInfo(&GlobalGameInfo);

	// Make an actual window ?
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	// Note on window dimensions: Height = titlebar + (2*border + shadow); Width = 2*(border+shadow); Guesstimate: titlebar=30, border=1, shadow = 8.  Find programatically?
	HWND windowHandle = CreateWindowEx(0,WindowClass.lpszClassName, L"Mouse Wheel Pong", WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, globalWindow.winWidth+16, globalWindow.winHeight+39, 0, 0, ProgramInstance, 0);
	
	HDC MonitorContext = GetDC(windowHandle);

	//setMWPSeeds(MWPrand() * 1000, MWPrand() * 1000, MWPrand() * 1000);
	//randomBackground(&globalWindow);

	rgb BackgroundColor(255, 0, 0);

	background(&globalWindow, BackgroundColor);

	int everyCertainFrame = 0;
	int everyCertainFrameCount = 10;

	while(startup)
	{
		KeyPresses key = ProcessEvents(GlobalGameInfo.deviceList);
		if(key.device != NULL)
		{
			MWPdevice & device = GlobalGameInfo.deviceList[key.device];
			if (key.leftButtonUp)
				device.mouse.leftButtonPressed = false;
			else if(key.leftButtonDown)
				device.mouse.leftButtonPressed = true;
			else if(key.rightButtonUp)
				device.mouse.rightButtonPressed = false;
			else if(key.rightButtonDown)
				device.mouse.rightButtonPressed = true;



			if (device.mouse.rightButtonPressed && device.mouse.leftButtonPressed && device.assignedPlayer > MAX_PLAYERS && GlobalGameInfo.playerCount < MAX_PLAYERS)
			{
				registerDevice(GlobalGameInfo.deviceList, GlobalGameInfo.reverseDeviceList, GlobalGameInfo.playerCount, key.device);
				Player player = { 0 };
				GlobalGameInfo.players[GlobalGameInfo.playerCount] = player;
				++GlobalGameInfo.playerCount;
				background(&globalWindow, rgb(255, 0, 0));
				drawSpreadVerticalLines(&globalWindow, GlobalGameInfo.playerCount - 1, rgb(255, 255, 0));
				drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, rgb(0, 0, 0));

			}
			
			if (device.assignedPlayer < MAX_PLAYERS)
			{
				if (key.scrolledUp)
				{
					drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, BackgroundColor);
					if((GlobalGameInfo.players[device.assignedPlayer].position-30 + (globalWindow.winHeight / 2))>0)
						GlobalGameInfo.players[device.assignedPlayer].position -= 30;
					drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, rgb(0, 0, 0));
				}
				else if(key.scrolledDown)
				{
					drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, BackgroundColor);
					if (globalWindow.winHeight> (GlobalGameInfo.players[device.assignedPlayer].position + 30 + GlobalGameInfo.paddleHeight+ (globalWindow.winHeight / 2)))
						GlobalGameInfo.players[device.assignedPlayer].position += 30;
					drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, rgb(0, 0, 0));
				}
			}
			
		}


		if (key.accept)
		{
			startup = false;
		}
		else if(key.quit)
		{
			return 42;
		}
		else
		{
			updateScreen(windowHandle, MonitorContext);
		}

		if (everyCertainFrame == (everyCertainFrameCount - 1))
		{
			//update device list
			//check for disconnects
			//maybe do this https://stackoverflow.com/questions/1437634/can-windows-detect-when-a-monitor-mouse-keyboard-is-disconnected
			std::vector<int> disconnectedPlayer = DetectPlayerMouseConnectsAndDisconnect(&GlobalGameInfo);
			if (disconnectedPlayer.size() > 0)
			{
				RemoveDisconnectedPlayers(disconnectedPlayer, &GlobalGameInfo);
				background(&globalWindow, rgb(255, 0, 0));
				drawSpreadVerticalLines(&globalWindow, GlobalGameInfo.playerCount - 1, rgb(255, 255, 0));
				drawPaddlesRegistrationScreen(&globalWindow, GlobalGameInfo.players, GlobalGameInfo.playerCount, rgb(0, 0, 0));
			}
			//need a way to remove disconnected mice that were not registered to a player
		}
		everyCertainFrame = (everyCertainFrame + 1) % everyCertainFrameCount;

	}

	bool paused = false;
	while (running)
	{
		KeyPresses key = ProcessEvents(GlobalGameInfo.deviceList);
		if (!paused)
		{
			
			// TODO: Change line struct to accept 2 const point structs instead of 4 uint32_ts?
			line curLine(50, 0, 0, 25);
			drawLine(&globalWindow, curLine, rgb(200, 200, 0));
			
			curLine = line(50, 0, 25, 100);
			drawLine(&globalWindow, curLine, rgb(200, 200, 0));

			curLine = line(50, 0, 75, 100);
			drawLine(&globalWindow, curLine, rgb(200, 200, 0));

			curLine = line(50, 0, 100, 25);
			drawLine(&globalWindow, curLine, rgb(200, 200, 0));

			
			setPixelXY(&globalWindow,  50,   0, rgb(7, 7, 7));
			setPixelXY(&globalWindow,   0,  25, rgb(7, 7, 7));
			setPixelXY(&globalWindow,  25, 100, rgb(7, 7, 7));
			setPixelXY(&globalWindow,  75, 100, rgb(7, 7, 7));
			setPixelXY(&globalWindow, 100,  25, rgb(7, 7, 7));
			

			drawHorizontalLine(&globalWindow, 100, 200, 200, rgb(0, 255, 0));
			drawVerticalLine(&globalWindow, 100, 300, 150, rgb(0, 10, 150));
			drawRectangle(&globalWindow, 200, 200, 100, 100, rgb(0, 255, 255));
			drawTriangle(&globalWindow, 500, 100, 400, 400, 550, 250, rgb(100, 255, 255));
			rasterizeCircle(&globalWindow, 300, 300, 50, rgb(0, 0, 255));

			//write to global buffer here

			if (everyCertainFrame == (everyCertainFrameCount - 1))
			{
				//update device list
				//check for disconnects
				std::vector<int> disconnectedPlayer = DetectPlayerMouseConnectsAndDisconnect(&GlobalGameInfo);
			}
			everyCertainFrame = (everyCertainFrame + 1) % everyCertainFrameCount;

			updateScreen(windowHandle, MonitorContext);
		}
	}

	return 42;
}

LRESULT CALLBACK eventHandler(HWND winHandle, UINT eventType, WPARAM inputW, LPARAM inputM)
{
	switch (eventType)
	{
		case WM_SIZE:
		{
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
			HDC winDeviceContext = BeginPaint(winHandle, & paintInfo);
			//unused
			//int newPaintHeight = paintInfo.rcPaint.bottom - paintInfo.rcPaint.top;
			//int newPaintWidth = paintInfo.rcPaint.right - paintInfo.rcPaint.left;
			RECT clientRect;
			GetClientRect(winHandle, &clientRect);
			
			int newHeight = clientRect.bottom - clientRect.top;
			int newWidth = clientRect.right - clientRect.left;

			// copies pixel data from our memory to the screen's memory
			// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/nf-wingdi-stretchdibits
			StretchDIBits(winDeviceContext,0,0,newWidth,newHeight,0,0,globalWindow.winWidth,globalWindow.winHeight,globalWindow.winPixMemory,&(globalWindow.winProperties),DIB_RGB_COLORS,SRCCOPY);
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
	inWindow->winPixMemory = VirtualAlloc(WIN_MEM_START, inWindow->bytesPerRow * winHeight, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);


	// Set Window Information
	inWindow->winProperties.bmiHeader.biSize  = sizeof(inWindow->winProperties.bmiHeader);
	
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
void getMonitorInfo( MONITORINFO * monitorDim )
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