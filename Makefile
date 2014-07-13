CC=gcc
CFLAGS=-Os -pedantic -Wall
PREFIX?=/usr/X11R6

all:
	$(CXX) $(CFLAGS) -I$(PREFIX)/include -I./Include ./Src/Colors.cpp ./Src/Clients.cpp ./Src/Keybindings.cpp ./Src/EventsHandler.cpp ./Src/btwm.cpp -L$(PREFIX)/lib -lX11 -o bloodthirsty
