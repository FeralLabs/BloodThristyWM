#pragma once

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NIL (0x0)

class Colors;

struct Dimension2D {
	int x, y;
};

struct Shared {
	Display *display;
	Window RootWindow;
	
	int RootScreen;
	
	Colors *Color;

	XFontStruct *Font;

	Dimension2D ScreenSize;
	
	Window PanelWindow;
	GC	   PanelGC;
	
	Colormap ColorMap;

	unsigned long WhitePixel, BlackPixel;
	
	bool isRunning;
};