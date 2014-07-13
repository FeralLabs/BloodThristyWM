#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "shared.h"

#include <map>

class Clients
{
public:
	Clients(Shared &);
	void Add(Window);
	~Clients();
private:
	Shared 	mShared;
	//std::map <Window> mClients;
};