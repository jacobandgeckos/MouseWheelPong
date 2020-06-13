#pragma once
#include <windows.h>
#include <iostream>

struct Winfo 
{
	int winHeight;
	int winWidth;

	int bytesPerPixel;
	int bytesPerRow;

	void * winPixMemory;
	BITMAPINFO winProperties;
};