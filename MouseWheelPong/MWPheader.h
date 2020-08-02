#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <dbt.h>
#include <assert.h> 

#include <iostream>
#include <map>
#include <vector>
#include <iterator>
#include <string>

//#define NDEBUG

// Where our winPixMemory is located in memory
#define WIN_MEM_START 0

#define MAX_PLAYERS 100

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
bool DoRegisterDeviceInterfaceToHwnd(GUID InterfaceClassGuid, HWND hWnd, HDEVNOTIFY *hDeviceNotify);

inline void updateScreen(HWND windowHandle, HDC MonitorContext);

