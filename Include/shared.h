#pragma once

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NIL (0x0)

struct Dimension2D {
	int x, y;
};

struct Shared {
	Display *display;
	Window RootWindow;
	
	int RootScreen;
	
	Dimension2D ScreenSize;

	Window PanelWindow;

	unsigned long WhitePixel, BlackPixel;
	
	bool isRunning;
};