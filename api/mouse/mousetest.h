/**************
FILE          : mousetest.h
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
              : - mousetest.h: SaXGUI::SCCMouseTest header defs
              : - mousetest.cpp: test widget for mouse events
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMOUSE_TEST_H
#define SCCMOUSE_TEST_H 1

//====================================
// Includes...
//------------------------------------
#include <qtimer.h>
#include <q3dict.h>
#include <qstatusbar.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QPaintEvent>

//====================================
// Includes...
//------------------------------------
#include "mouseevent.h"
#include "../wrapper.h"

//=====================================
// Defines
//-------------------------------------
#define PIX_DIR     "/usr/share/sax/api/figures"
#define PIX_DEFAULT PIX_DIR "/mouse_default.png"
#define PIX_FRONT   PIX_DIR "/mouse_front.png"
#define PIX_LEFT    PIX_DIR "/mouse_left.png"
#define PIX_MIDDLE  PIX_DIR "/mouse_middle.png"
#define PIX_RIGHT   PIX_DIR "/mouse_right.png"
#define PIX_BACK    PIX_DIR "/mouse_back.png"

namespace SaXGUI {
//=====================================
// Class Definitions
//-------------------------------------
class SCCMouseTest : public SCCMouseEvent {
	Q_OBJECT

	private:
	int mID;
	Q3Dict<QString>* mTextPtr;
	QStatusBar*     mStatus;
	QTimer*         mTimer;
	QPixmap*        mPixmap[6];
	QPixmap*        mBuffer;

	private:
	int mXold;
	int mYold;
	int mX;
	int mY;

	public:
	void setPixmap (int);
	SCCMouseTest ( Q3Dict<QString>*, QWidget* = 0 );
	virtual void paintEvent( QPaintEvent* );

	public:
	void singleClickEvent (int);
	void doubleClickEvent (int);
	void pressEvent (int);
	void releaseEvent (int);
	void wheelEvent (int);
	void moveEvent (int,int);

	public slots:
	void timerDone (void);
};
} // end namespace
#endif
