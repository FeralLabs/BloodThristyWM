#include "Keybindings.hpp"

Keybindings::Keybindings() {

}

void
Keybindings::Add(KeySym key, unsigned int Modifier = 0, std::string cmd = "") {
	Keybind KB;
	KB.key = key;
	KB.modifier = Modifier;
	KB.cmd = cmd;

	Container.push_back(KB);
}

void
Keybindings::Remove(Keybind key) {
	// Not at the moment
}

std::string
Keybindings::getCommand(KeySym key, unsigned int Modifier = -1) {
	for (std::vector<Keybind>::iterator i = 
		Container.begin(); i != Container.end(); ++i)
	{
		if ((*i).key == key && Modifier == (*i).modifier) {
			return (*i).cmd;
		}
	}

	return "";
}


Keybindings::~Keybindings() {
	
}