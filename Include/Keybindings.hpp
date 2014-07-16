#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <utility>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <string>

struct Keybind
{
	KeySym key;
	unsigned int modifier;
	std::string cmd;
};

class Keybindings
{
public:
	Keybindings();
	void Add(KeySym, unsigned int, std::string);
	void Remove(Keybind);
	std::string getCommand(KeySym, unsigned int);
	~Keybindings();
private:
	std::vector<Keybind> Container;
	std::vector<std::pair <std::string, void(*)()> > ReservedKeybindings;
};