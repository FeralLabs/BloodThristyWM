#include "Colors.hpp"

Colors::Colors(Shared & shared) : mShared(shared) {

}

void
Colors::Define(const char * colorname, const char * color) {
	XColor temp;

	XParseColor(mShared.display, mShared.ColorMap, color, &temp);
	XAllocColor(mShared.display, mShared.ColorMap, &temp);

	ColorContainer[colorname] = temp;
}

XColor
Colors::Get(const char * colorname) {
	return ColorContainer[colorname];
}