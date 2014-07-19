#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>
#include <string.h>
#include <time.h>
#include <vector>

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

void remap(Shared & mShared) {
	size_t count = mShared.Clients.size();

	if (count != 1) {
		unsigned int x = 0 ;
		for (std::vector<Client>::iterator i = mShared.Clients.begin(); i != mShared.Clients.end(); ++i)
		{
			if(x == count - 1) {
				XSetWindowBorder(mShared.display, (*i).window, mShared.Color -> Get ("selected").pixel);
				XMoveResizeWindow(mShared.display, (*i).window, 5, 20, 
					(mShared.ScreenSize.x - 10), 
					(mShared.ScreenSize.y - (mShared.ScreenSize.y / 2) - 20));
			} else {
				XSetWindowBorder(mShared.display, (*i).window, mShared.Color -> Get ("selected_text").pixel);
				XMoveResizeWindow(mShared.display, (*i).window, 
					((mShared.ScreenSize.x / (count - 1)) * (x) + 5), 
					mShared.ScreenSize.y / 2 + 10, 
					((mShared.ScreenSize.x / (count - 1)) - 10),
					(mShared.ScreenSize.y - (mShared.ScreenSize.y / 2) - 30)
					);
			}
			
			x++;
		}
	} else {
		XSetWindowBorder(mShared.display,  (*mShared.Clients.begin()).window, mShared.Color -> Get ("selected").pixel);
		XMoveResizeWindow(mShared.display, (*mShared.Clients.begin()).window, 5, 20, 
			(mShared.ScreenSize.x - 10), 
			(mShared.ScreenSize.y - 20));
	} 
}

void
drawBar (XEvent * event, Shared & mShared) {
	XClearWindow(mShared.display, mShared.PanelWindow);

	XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("selected").pixel);

	XDrawRectangle(mShared.display, mShared.PanelWindow, mShared.PanelGC, 27, 0, 10, 50);
	XFillRectangle(mShared.display, mShared.PanelWindow, mShared.PanelGC, 27, 0, 10, 50);
	
	XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("bgfg").pixel);
	XSetFont(mShared.display, mShared.PanelGC, mShared.Font->fid);

	char *rootname;
	int works = XFetchName (mShared.display, mShared.RootWindow, &rootname);

	if(works) {
		XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("selected_text").pixel);
		XDrawString(mShared.display, mShared.PanelWindow, mShared.PanelGC, mShared.ScreenSize.x - 150, 13, rootname, strlen(rootname));
	}
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
	shared.Color -> Define ("bgcolor",  	 "#000");
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

	mEventsHandler -> Add(UnmapNotify, [] (XEvent * event, Shared & mShared) {
		for (std::vector<Client>::iterator i = mShared.Clients.begin(); i != mShared.Clients.end(); ++i)
			if((*i).window == event -> xunmap.window) {
				mShared.Clients.erase(i);
				break;
			}

		remap(mShared);
	});

	mEventsHandler -> Add(ConfigureRequest, [] (XEvent * event, Shared & mShared) {
		std::cout << "Configuring Window " << event -> xconfigure . window << std::endl;

		Client Current;
		Current.window = event -> xconfigure . window;
		Current.isMaster = true;
		mShared.Clients . push_back(Current);

		XWindowChanges values;
		values.border_width = 1;
		XConfigureWindow(mShared.display, event -> xconfigure . window, CWBorderWidth, &values);

		remap(mShared);

		XSelectInput(mShared.display, event -> xconfigure . window, KeyPressMask | ButtonReleaseMask );
    	XSync(mShared . display, False);
	});

	mEventsHandler -> Add(MapRequest, [] (XEvent * event, Shared & mShared) {
		XSetWindowBorder(mShared . display, event -> xmaprequest . window, mShared . Color -> Get ("selected").pixel);
		XMapWindow(mShared . display, event -> xmaprequest . window);
		XSync(mShared . display, False);
	});

	mEventsHandler -> Add(DestroyNotify, [] (XEvent * event, Shared & mShared) {
		for (std::vector<Client>::iterator i = mShared.Clients.begin(); i != mShared.Clients.end(); ++i)
			if((*i).window == event -> xdestroywindow.window) {
				mShared.Clients.erase(i);
				break;
			}

		remap(mShared);
	});

	mEventsHandler -> Add(CreateNotify, *drawBar);
	mEventsHandler -> Add(MapRequest, *drawBar);
	mEventsHandler -> Add(Expose, *drawBar);
	mEventsHandler -> Add(PropertyNotify, *drawBar);
	mEventsHandler -> Add(DestroyNotify, *drawBar);

	mEventsHandler -> Run();

	return 0;
}
