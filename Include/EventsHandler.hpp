#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <utility>
#include <unistd.h>
#include <cstdlib>
#include <string>

#include "shared.h"
#include "Keybindings.hpp"

class EventsHandler
{
public:
	EventsHandler(Shared &, Keybindings);
	
	void handleKeyPress(XEvent *);
	//void handleKeyRelease(XEvent *);

	void handleButtonPress(XEvent *);
	void handleButtonRelease(XEvent *);

	void handleMotion(XEvent *);

	void Run();
	void executeCMD(std::string);
	~EventsHandler();
	
private:	
	bool 				isRunning;
	Shared 				mShared;
	Keybindings 		mKeyBindings;
	XEvent 				event;
	XButtonEvent 		buttonEvent;
	XWindowAttributes	attributes;
};