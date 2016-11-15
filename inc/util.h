/***************************************************************************//**
@file
@brief interface for the utilityCentral
*******************************************************************************/
#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <queue>
#include <algorithm>
#include <sstream>
#include "util.h"
#include "game.h"
#include <GL/glut.h>

using namespace std;

/// convenience typedef
typedef unsigned char byte;

const float White[] = {1.0, 1.0, 1.0};
const float Red[] = {1.0, 0.0, 0.0};
const float Green[] = {0.0, 1.0, 0.0};
const float Blue[] = {0.0, 0.0, 1.0};
const float Black[] = {0.0, 0.0, 0.0};
const float Grey[] = {0.5, 0.5, 0.5};

/// Event enum to describing events
enum EventType {
	INIT,
	DISPLAY,
	KEYBOARD,
	MOUSECLICK,
	MOUSEDRAG,
    RESHAPE,
    TICKTOCK,
};

/// Symbolic constants for the backspace and ESC keys
static const int BSP = 8;
static const int ESC = 27;

void DrawRectangle(float x1, float y1, float x2, float y2, const float color[]);
void DrawFilledRectangle( float x1, float y1, float x2, float y2, const float color[] );
void DrawTextString(const char *string, int x, int y, const float color[]);

void drawPlayfield(Game game);

/*!
@brief Contains information about the event that is happening and constructors
that assign enums and data depending on what is passed in. Depending on the
complexity of the application, the event structure might need to be extended.
*/
struct Event {
	/** \brief type of event to handle */
	EventType event;
	/** \brief keyboard key pressed by user */
	byte key;
	/** \brief mouse button pressed */
	int button;
	/** \brief mouse button position (up or down) */
	int state;
	/** \brief x-coordinate of mouseclick */
	int x;
	/** \brief x-coordinate of mouseclick */
	int y;
    /** Filename */
    const char *fileName;
    
	/** \brief if there are no other params, display routine */
	Event() :
		event(DISPLAY) {}
    /** \brief the timer event */
    Event(EventType e) : event(e) {}
	/** \brief the call from initOpenGL */
	Event(const char *fName) : 
		event(INIT), fileName(fName) {}
	/** \brief keyboard event */
	Event(const byte k, const int xPos, const int yPos) :
		event(KEYBOARD), key(k), x(xPos), y(yPos) {}
	/** \brief mouse click in place */
	Event(const int b, const int s, const int xPos, const int yPos) :
		event(MOUSECLICK), button(b), state(s), x(xPos), y(yPos) {}
	/** \brief mouse drag across screen */
	Event(const EventType e, const int xStart, const int yStart) :
		event(e), x(xStart), y(yStart) {}
};

/// The main hub for collecting the various events into a central location.
void utilityCentral(const Event & event);
#endif /* UTIL_H_INCLUDED */
