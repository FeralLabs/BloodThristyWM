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

#define GAP 10

int selected = 2;

void
add_tag (Shared & mShared, const char * a) {
	Tag temp;
	temp.Name = a;

	mShared.Tags.push_back(temp);
}

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
	shared -> ScreenSize.y = DisplayHeight (shared->display, 0) - 15;
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
		shared->ScreenSize.x, 15, 
		0, shared->WhitePixel, Color -> Get ("panel_background").pixel);

	XMapWindow(shared->display, shared->PanelWindow);
	shared -> PanelGC = XCreateGC(shared->display, shared->PanelWindow, NIL, NULL);

}

void remap(Shared & mShared) {
	size_t count = mShared.Clients.size();

	if (count != 1) {
		unsigned int x = 0 ;
		for (std::vector<Client>::iterator i = mShared.Clients.begin(); i != mShared.Clients.end(); ++i)
		{
			if(x == count - 1) { // master
				XMoveResizeWindow(mShared.display, (*i).window, 0, 15, 
					(mShared.ScreenSize.x - 2), 
					(mShared.ScreenSize.y - (mShared.ScreenSize.y / 2) - 1));
			} else {
				XMoveResizeWindow(mShared.display, (*i).window, 
					mShared.ScreenSize.x / (count - 1) * (x), 
					mShared.ScreenSize.y / 2 + 15, 
					mShared.ScreenSize.x / (count - 1),
					mShared.ScreenSize.y - (mShared.ScreenSize.y / 2) - 2
					);
			}
			
			x++;
		}
	} else {
		XMoveResizeWindow(mShared.display, (*mShared.Clients.begin()).window, 0, 15, 
			mShared.ScreenSize.x - 2, 
			mShared.ScreenSize.y - 2);
	} 
}

void
Temporary (XEvent * event, Shared & mShared) {
	XClearWindow(mShared.display, mShared.PanelWindow);

	//const char *  tags [] = {"Test 1", "Test 2", "Stuff", "Coding"};

	int x = 20;
	for (std::vector<Tag>::iterator i = mShared.Tags.begin(); i != mShared.Tags.end(); ++i)
	{
		unsigned int fontlen = XTextWidth(mShared.Font, (*i).Name, strlen((*i).Name));

		XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("panel_selected").pixel );
		
		if ( (*i).Name == "Hacks" ) {
			XDrawRectangle(mShared.display, mShared.PanelWindow, mShared.PanelGC, x - 10, 0, fontlen + 20, 50);
			XFillRectangle(mShared.display, mShared.PanelWindow, mShared.PanelGC, x - 10, 0, fontlen + 20, 50);

			XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("panel_selected_text").pixel);
 			XDrawString(mShared.display, mShared.PanelWindow, mShared.PanelGC, x, 11, (*i).Name, strlen((*i).Name));
		} else {
			XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("panel_full").pixel);
			XSetFont(mShared.display, mShared.PanelGC, mShared.Font->fid);

			XDrawString(mShared.display, mShared.PanelWindow, mShared.PanelGC, x, 11, (*i).Name, strlen((*i).Name)); 
		}
		
		x += fontlen + 20;
	}


	XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("panel_text").pixel);
	XDrawString(mShared.display, mShared.PanelWindow, mShared.PanelGC, x, 11, "[bStack]", strlen("[bStack]"));

	char *rootname;
	int works = XFetchName (mShared.display, mShared.RootWindow, &rootname);

	if(works) {
		XSetForeground(mShared.display, mShared.PanelGC, (mShared.Color) -> Get("panel_text").pixel);
		XDrawString(mShared.display, mShared.PanelWindow, 
			mShared.PanelGC, 
			mShared.ScreenSize.x - XTextWidth(mShared.Font, rootname, strlen(rootname)) - 20, 11, rootname, strlen(rootname));
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
	shared.Color -> Define ("panel_full",           "#dddddd");
	shared.Color -> Define ("panel_background", 	"#080b0e");
	shared.Color -> Define ("panel_selected",		"#ff9500");
	shared.Color -> Define ("panel_text",	 		"#c7c7cc");
	shared.Color -> Define ("panel_selected_text", 	"#ffffff");

	shared 	     .  Font = XLoadQueryFont(shared . display,"-*-helvetica-*-r-*-*-10-*-*-*-*-*-*-*");

	createPanel(&shared, shared.Color);

	Keybindings mKeybindings;
	
	mKeybindings.Add(XK_t, Mod1Mask, "terminator");
	mKeybindings.Add(XK_p, Mod1Mask, "dmenu_run");
	mKeybindings.Add(XK_w, Mod1Mask, "midori");

	mKeybindings.Add(XK_q, 0x9, "kill_btwm");

	add_tag(shared, "Browser");
	add_tag(shared, "Hacks");
	add_tag(shared, "Terminals");

	EventsHandler * mEventsHandler = new EventsHandler(shared, mKeybindings);
	
	mEventsHandler -> Add(ButtonRelease, [] (XEvent * event, Shared & mShared) {
		XUngrabPointer(mShared.display, CurrentTime);
	});

	mEventsHandler -> Add(KeyPress, [] (XEvent * event, Shared & mShared) {
		if((event -> xkey . keycode - 9) > 0 && (event -> xkey . keycode - 9) < 5 && (event -> xkey . state == Mod4Mask)) {
			selected = (event -> xkey . keycode - 9);
			Temporary(event, mShared);
		}
	});

	mEventsHandler -> Add(ConfigureNotify, [] (XEvent *event, Shared & mShared) {
	 	if (!event -> xconfigure.override_redirect)
	 		remap(mShared);
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
		if(event -> xconfigure . window != None || !event -> xconfigure . override_redirect) {
			Client Current;
			Current.window = event -> xconfigure . window;
			Current.isMaster = true;
			mShared.Clients . push_back(Current);

			XWindowChanges values;
			values.border_width = 1;	
			XConfigureWindow(mShared.display, event -> xconfigure . window, CWBorderWidth, &values);

			remap(mShared);

			XSelectInput(mShared.display, event -> xconfigure . window, KeyPressMask | ButtonReleaseMask );
		}

    	XSync(mShared . display, False);
	});

	mEventsHandler -> Add(MapRequest, [] (XEvent * event, Shared & mShared){
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

	mEventsHandler -> Add(CreateNotify, *Temporary);
	mEventsHandler -> Add(MapRequest, *Temporary);
	mEventsHandler -> Add(Expose, *Temporary);
	mEventsHandler -> Add(PropertyNotify, *Temporary);
	mEventsHandler -> Add(DestroyNotify, *Temporary);

	mEventsHandler -> Run();

	return 0;
}
