#include "Clients.hpp"

Clients::Clients(Shared & shared)
: mShared(shared) {

}

size_t
Clients::Add(Window window)
{
	Client c;
	c.window = window;
	mClients.push_back(c);
	return mClients.size();
}