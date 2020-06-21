#include "MWPheader.h"
#include "MWPinput.h"

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

	// Make an actual window ?
	// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-createwindowexw
	HWND windowHandle = CreateWindowEx(0,WindowClass.lpszClassName, L"Mouse Wheel Pong", WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, ProgramInstance, 0);
	
	HDC MonitorContext = GetDC(windowHandle);

	while (running)
	{
		ProcessEvents();

		background(&globalWindow, 255, 0, 0);


		//write to global buffer here



		RECT clientRect;
		GetClientRect(windowHandle, &clientRect);

		int newHeight = clientRect.bottom - clientRect.top;
		int newWidth = clientRect.right - clientRect.left;
		//Draw to the screen
		StretchDIBits(MonitorContext, 0, 0, newWidth, newHeight, 0, 0, globalWindow.winWidth, globalWindow.winHeight, globalWindow.winPixMemory, &(globalWindow.winProperties), DIB_RGB_COLORS, SRCCOPY);
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

		case WM_QUIT:
		case WM_CLOSE:
		{
			running = false;
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

static void
background(Winfo* window, char red, char green, char blue)
{
	char* Row = (char*)window->winPixMemory;
	for (int Y = 0; Y < window->winHeight; ++Y)
	{
		uint32_t* Pixel = (uint32_t*)Row;
		for (int X = 0; X < window->winWidth; ++X)
		{
			/*              0  1  2  3
			 Pixel in mem: BB GG RR xx

			 loaded into reg
			 0x xxRRGGBB
			 */
			*Pixel++ = ((red << 16) | (green << 8) | blue);

		}
		Row += window->bytesPerRow;
	}
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