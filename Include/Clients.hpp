#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "shared.h"

#include <vector>

struct Client
{
	Window window;	
};

class Clients
{
public:
	Clients(Shared &);
	size_t Add(Window);
	~Clients();
private:
	Shared 	mShared;
	std::vector <Client> mClients;
};