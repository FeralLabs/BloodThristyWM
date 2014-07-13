#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "shared.h"

#include <map>

class Colors
{
public:
	Colors	(Shared &);
	void 	Define(const char*, const char*);
	XColor	Get(const char*);
	~Colors();
private:
	Shared mShared;
	std::map <const char *, XColor> ColorContainer;
};