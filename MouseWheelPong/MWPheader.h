#pragma once
#include <windows.h>
#include <iostream>

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

// our window
// Note: globals (anything outside of a function) are initialized to zero automatically)
Winfo globalWindow;

int globalWinWidth  = 800;
int globalWinHeight = 600;

bool running = true;

// initiallizes our window with width and height
void winfoInit(Winfo *, int, int);
static void background(Winfo* window, char red, char green, char blue);
LRESULT CALLBACK eventHandler(HWND winHandle, UINT eventType, WPARAM inputW, LPARAM inputM);

// Where our winPixMemory is located in memory
#define WIN_MEM_START 0
