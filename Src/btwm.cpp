#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>
#include <string.h>
#include <time.h>
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
		shared->ScreenSize.x, 18, 
		0, shared->WhitePixel, Color -> Get ("bgcolor").pixel);

	XMapWindow(shared->display, shared->PanelWindow);
	shared -> PanelGC = XCreateGC(shared->display, shared->PanelWindow, NIL, NULL);

}

void
Temporary (XEvent * event) {

}

int
main(int argc, char const *argv[])
{
	// Loads up the shared objects container
	// It contains the ScreenSize, Window, Display Size etc.
	Shared shared;
	if(loadShared (&shared) != 0) 
		return (3);

	shared.Color = new Colors(shared);
	shared.Color -> Define ("bgcolor",  	 "#111111");
	shared.Color -> Define ("selected", 	 "#f22863");
	shared.Color -> Define ("bgfg",			 "#fefefe");
	shared.Color -> Define ("selected_text", "#555555");
	shared 	     .  Font = XLoadQueryFont(shared . display,"-*-helvetica-*-r-*-*-10-*-*-*-*-*-*-*");

	createPanel(&shared, shared.Color);

	Keybindings mKeybindings;
	
	mKeybindings.Add(XK_t, Mod1Mask, "terminator");
	mKeybindings.Add(XK_p, Mod1Mask, "dmenu_run");
	mKeybindings.Add(XK_w, Mod1Mask, "firefox");

	mKeybindings.Add(XK_q, 0x9, "kill_btwm");

	EventsHandler * mEventsHandler = new EventsHandler(shared, mKeybindings);
	
	mEventsHandler -> Add(ButtonRelease, [] (XEvent * event, Shared & mShared) {
		XUngrabPointer(mShared.display, CurrentTime);
	});

	mEventsHandler -> Add(KeyPress, [] (XEvent * event, Shared & mShared) {
		XSetWindowBorder(mShared . display, event -> xkey . window, mShared . Color -> Get ("selected").pixel);
		//XMoveResizeWindow(mShared . display, event -> xmaprequest . window, 200, 200, 0, 0);
	});

	mEventsHandler -> Add(Expose, [] (XEvent * event, Shared & mShared) {
			XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("selected").pixel);

			time_t rawtime;
			rawtime = time(NULL);
			struct tm * curtime = localtime(&rawtime);

			char * Test = asctime(curtime);

			XDrawRectangle(mShared.display, mShared.PanelWindow, mShared.PanelGC, 27, 0, 10, 50);
			XFillRectangle(mShared.display, mShared.PanelWindow, mShared.PanelGC, 27, 0, 10, 50);
			
			XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("bgfg").pixel);
			XSetFont(mShared.display, mShared.PanelGC, mShared.Font->fid);

			//XDrawString(mShared.display, mShared.PanelWindow, mShared.PanelGC, 30, 13, Test, sizeof(Test) - 1);

			XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("selected_text").pixel);
			XDrawString(mShared.display, mShared.PanelWindow, mShared.PanelGC, mShared.ScreenSize.x - 150, 13, Test, strlen(Test) - 1);

			XFlush(mShared.display);

	});

	mEventsHandler -> Run();

	return 0;
}
