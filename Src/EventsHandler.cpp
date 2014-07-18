#include "EventsHandler.hpp"
#include <iostream>
#include <string.h>

EventsHandler::EventsHandler(Shared & shared, Keybindings KB) 
	: isRunning(true), mShared(shared), mKeyBindings(KB)  {
}

void
EventsHandler::handleKeyPress(XEvent* event) {
	
	std::string cmd = mKeyBindings.getCommand(
		XLookupKeysym(&event->xkey, 0), 
		event->xkey.state
		);

	if(cmd == "kill_btwm")
		isRunning = false;
	else if(cmd != "") executeCMD(cmd);
}

void
EventsHandler::handleButtonPress(XEvent* event) {
	if(event -> xbutton.subwindow != None && event -> xbutton.state == Mod1Mask) {
		XGrabPointer(mShared.display, event -> xbutton.subwindow, True,
			PointerMotionMask|ButtonReleaseMask, GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
		XGetWindowAttributes(mShared.display, event -> xbutton.subwindow, &attributes);

		buttonEvent = event -> xbutton;
	}
}

void
EventsHandler::handleMotion(XEvent* event) {
	int xdiff, ydiff;
	
	while (XCheckTypedEvent(mShared.display, MotionNotify, event));
	xdiff = event->xbutton.x_root - buttonEvent.x_root;
	ydiff = event->xbutton.y_root - buttonEvent.y_root;

	XMoveResizeWindow(mShared.display, event -> xmotion.window, 
		attributes.x + (buttonEvent.button == 1 ? xdiff : 0),
		attributes.y + (buttonEvent.button == 1 ? ydiff : 0),
		MAX(1, attributes.width + (buttonEvent.button == 3 ? xdiff : 0)),
		MAX(1, attributes.height +(buttonEvent.button == 3 ? ydiff : 0))
		);
}

void
EventsHandler::executeCMD (std::string cmd) {
	if (!fork()) {
		cmd = std::string("exec ") + cmd;
		execl("/bin/sh", "/bin/sh", "-c", cmd.c_str(), NULL);
		exit(1);
	}
}

void
EventsHandler::Add(int where, handlerFunction * function) {
	EventsHandler::handlers.push_back(std::make_pair(where, function));
}

void
EventsHandler::RunHandlers() {
	for (handlerContainer::iterator i = handlers.begin(); i != handlers.end(); ++i)
		if((*i).first == event.type) {
			((*i).second)(&event, mShared);
		}
}

void
EventsHandler::Run() {
	XSelectInput(mShared.display, mShared.RootWindow, 
		SubstructureRedirectMask|SubstructureNotifyMask|ButtonPressMask
|EnterWindowMask|LeaveWindowMask|StructureNotifyMask|PropertyChangeMask|KeyPressMask);

	while (isRunning) {
		XNextEvent(mShared.display, &event);
		RunHandlers();
		std::cout << event.type << std::endl;
		//std::cout << "ConfigureRequest is " << ConfigureRequest << std::endl;
		std::cout << "RootWindow:" << mShared.RootWindow << std::endl;

		switch (event.type) {
			case Expose:
			break;
			case ConfigureRequest:
			break;
			case KeyPress:
				handleKeyPress(&event);
			break;
			case ButtonPress:
				handleButtonPress(&event);
			break;
			case MotionNotify:
				handleMotion(&event);
			break;
			default:
				std::cout << event.type << std::endl;
			break;
		}
	
	}

	XCloseDisplay(mShared.display);
}