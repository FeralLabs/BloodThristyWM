#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>
#include "Colors.hpp"
#include "Keybindings.hpp"
#include "EventsHandler.hpp"
#include "shared.h"

int
loadShared(Shared * shared) {
	// Opens up the display
	shared -> display      = XOpenDisplay  (NIL);

	if (!shared -> display) {
		std::cout << "Cannot open X display" << std::endl;
		return 1;
	}

	shared -> RootWindow   = DefaultRootWindow (shared->display);
	
	// Gets screensize (eg. 1024x768);
	shared -> ScreenSize.y = DisplayHeight (shared->display, 0);
	shared -> ScreenSize.x = DisplayWidth  (shared->display, 0);

	shared -> BlackPixel   = BlackPixel(shared -> display, 0);
	shared -> WhitePixel   = WhitePixel(shared -> display, 0);
	shared -> ColorMap 	   = DefaultColormap (shared -> display, 0);
	return 0;
}

void
createPanel(Shared * shared, Colors * Color)
{
	shared -> PanelWindow = XCreateSimpleWindow(
		shared->display, shared->RootWindow, 
		0, 0, 
		shared->ScreenSize.x, 21, 
		0, shared->WhitePixel, Color -> Get ("bgcolor").pixel);

	XMapWindow(shared->display, shared->PanelWindow);

	GC gc = XCreateGC(shared->display, shared->RootWindow, NIL, NULL);
	XSetForeground(shared->display, gc, Color -> Get("selected").pixel);
	XDrawRectangle(shared->display, shared->PanelWindow, gc, 0, 0, 21, 40);
	XFillRectangle(shared->display, shared->PanelWindow, gc, 0, 0, 21, 40);
}

int
main(int argc, char const *argv[])
{
	// Loads up the shared objects container
	// It contains the ScreenSize, Window, Display Size etc.
	Shared shared;
	if(loadShared (&shared) != 0) 
		return (3);

	Colors * Color = new Colors(shared);
	Color -> Define ("bgcolor", "#2c3e50");
	Color -> Define ("selected", "#FF0000");
	
	createPanel(&shared, Color);

	Keybindings mKeybindings;
	
	mKeybindings.Add(XK_t, Mod1Mask, "terminator");
	mKeybindings.Add(XK_p, Mod1Mask, "dmenu_run");
	mKeybindings.Add(XK_w, Mod1Mask, "firefox");

	mKeybindings.Add(XK_q, 0x9, "kill_btwm");

	EventsHandler * mEventsHandler = new EventsHandler(shared, mKeybindings);
	mEventsHandler -> Run();

	return 0;
}
