#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "shared.h"

#include <string>
#include <vector>

struct Tag
{
	std::string title;
};

class Tags
{
public:
	Tags();
	~Tags();
private:
	std::vector<char> v;
};