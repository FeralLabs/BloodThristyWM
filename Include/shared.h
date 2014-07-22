#pragma once

#include <vector>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NIL (0x0)

class Colors;

struct Client
{
	Window window;
	bool   isMaster;	
};

struct Tag
{
	const char * Name;
};


struct Dimension2D {
	int x, y;
};

struct Shared {
	Display *display;
	Window RootWindow;
	
	int RootScreen;
	
	Colors *Color;
	std::vector <Client> Clients;
	std::vector <Tag>	Tags;

	XFontStruct *Font;

	Dimension2D ScreenSize;
	
	Window PanelWindow;
	GC	   PanelGC;
	
	Colormap ColorMap;

	unsigned long WhitePixel, BlackPixel;
	
	bool isRunning;
};