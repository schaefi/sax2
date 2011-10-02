/**************
FILE          : mouseevent.cpp
***************
PROJECT       : SaX2 - SuSE advanced X configuration
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : configuration tool for the X window system 
              : released under the GPL license
              :
DESCRIPTION   : SaX2 GUI system using libsax to provide
              : configurations for a wide range of X11 capabilities
              : //.../
              : - mouseevent.h: SaXGUI::SCCMouseEvent implementation
              : - mouseevent.cpp: event filter for mouse test
              : ----
              :
STATUS        : Status: Development
**************/
#include "mouseevent.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QEvent>
#include <QWheelEvent>
#include <stdio.h>

namespace SaXGUI {
//====================================
// Constructor...
//------------------------------------
SCCMouseEvent::SCCMouseEvent ( QWidget* parent ): QWidget (parent) {
	doubleClick   = FALSE;
	mouseReleased = FALSE;
	timer = new QTimer( this );
	connect ( 
		timer, SIGNAL (timeout()), this, SLOT (timerDone()) 
	);
	installEventFilter (this);
}

//====================================
// Event filter for application...
//------------------------------------
bool SCCMouseEvent::eventFilter ( QObject*, QEvent* event ) {
	QMouseEvent* mouse = (QMouseEvent*)event;
	QWheelEvent* wheel = (QWheelEvent*)event;
	if ((! mouse) && (! wheel)) {
		return (false);
	}
	switch (mouse->type()) {
	case QEvent::MouseButtonDblClick:
		timer->stop();
		doubleClick   = TRUE;
		mouseReleased = FALSE;
		mBtn = mouse->button();
		doubleClickEvent (mBtn);
	break;
	case QEvent::MouseButtonPress:
		mBtn = mouse->button();
		pressEvent (mBtn);
		mouseReleased = FALSE;
		timer->start ( 300, TRUE );
		doubleClick = FALSE;
	break;
	case QEvent::MouseButtonRelease:
		if (! doubleClick) {
			mouseReleased = TRUE;
			mBtn = mouse->button();
			releaseEvent (mBtn);
		}
	break;
	case QEvent::Wheel:
		wheelEvent ( wheel->delta()/WHEEL_DELTA );
	break;
	case QEvent::MouseMove:
		moveEvent (mouse->x(),mouse->y());
	break;
	default:
	break;
	}
	return (false);
}

//====================================
// Timer for double clicks...
//------------------------------------
void SCCMouseEvent::timerDone ( void ) {
	if (! doubleClick) {
	if (! mouseReleased) {
		timer->start ( 300, TRUE );
	} else {
		singleClickEvent (mBtn);
		mouseReleased = FALSE;
	}
	}
}

//====================================
// Callback function moving mouse
//------------------------------------
void SCCMouseEvent::moveEvent (int x,int y) {
	printf("moveEvent(%d:%d)...\n",x,y);
}

//====================================
// Callback function for double click
//------------------------------------
void SCCMouseEvent::doubleClickEvent (int) {
	printf("doubleClickEvent()...\n");
}

//====================================
// Callback function for single click
//------------------------------------
void SCCMouseEvent::singleClickEvent (int) {
	printf("singleClickEvent()...\n");
}

//====================================
// Callback function for wheel event
//------------------------------------
void SCCMouseEvent::wheelEvent (int) {
	printf("wheelEvent()...\n");
}

//====================================
// Callback function for press event
//------------------------------------
void SCCMouseEvent::pressEvent (int) {
	printf("pressEvent()...\n");
}

//====================================
// Callback function for release event
//------------------------------------
void SCCMouseEvent::releaseEvent (int) {
	printf("releaseEvent()...\n");
}
} // end namespace
