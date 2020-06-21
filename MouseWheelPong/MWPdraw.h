#pragma once
#include "MWPheader.h"

inline void setPixel(uint32_t *pixel, uint32_t red, uint32_t green, uint32_t blue);
void background(Winfo* window, char red, char green, char blue);
void randomBackground(Winfo* window);
void drawHorizontalLine(Winfo* window, uint32_t x0, uint32_t x1, uint32_t y, char red, char green, char blue);
void drawVerticalLine(Winfo* window, uint32_t y0, uint32_t y1, uint32_t x, char red, char green, char blue);
void drawRectangle(Winfo* window, int x, int y, int width, int height, uint32_t red, uint32_t green, uint32_t blue);