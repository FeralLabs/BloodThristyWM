#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <utility>
#include <unistd.h>
#include <cstdlib>
#include <string>
#include <map>

#include "shared.h"
#include "Keybindings.hpp"

typedef void handlerFunction(XEvent* event, Shared &);
typedef std::vector< std::pair <int, handlerFunction*> > handlerContainer;

class EventsHandler
{
public:
	EventsHandler(Shared &, Keybindings);
	
	XEvent event;

	void handleKeyPress(XEvent *);
	//void handleKeyRelease(XEvent *);

	void handleButtonPress(XEvent *);
	void handleButtonRelease(XEvent *);
	void Add(int, handlerFunction*);
	void handleMotion(XEvent *);

	void RunHandlers();

	void Run();


	void executeCMD(std::string);
	~EventsHandler();
	
private:	
	bool 																isRunning;
	Shared 																mShared;
	Keybindings 														mKeyBindings;
	handlerContainer										 			handlers;
	XButtonEvent 														buttonEvent;
	XWindowAttributes													attributes;
};