#pragma once
#include <windows.h>
#include <iostream>
#include <math.h>
#include <map>
#include <vector>
#include <iterator>

struct Winfo
{
	int winWidth;
	int winHeight;

	int bytesPerPixel;
	int bytesPerRow;

	// Pointer to Memory for window pixels.
	void * winPixMemory;

	BITMAPINFO winProperties;
};

// initiallizes our window with width and height
void winfoInit(Winfo *, int, int);
LRESULT CALLBACK eventHandler(HWND winHandle, UINT eventType, WPARAM inputW, LPARAM inputM);
void initializeMiceToRawInput();

inline void updateScreen(HWND windowHandle, HDC MonitorContext);

// Where our winPixMemory is located in memory
#define WIN_MEM_START 0
