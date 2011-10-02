/**************
FILE          : mousetest.cpp
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
              : - mousetest.h: SaXGUI::SCCMouseTest implementation
              : - mousetest.cpp: test widget for mouse events
              : ----
              :
STATUS        : Status: Development
**************/
#include "mousetest.h"
//Added by qt3to4:
#include <QPixmap>
#include <Q3VBoxLayout>
#include <QPaintEvent>

namespace SaXGUI {
//====================================
// Constructor...
//------------------------------------
SCCMouseTest::SCCMouseTest ( 
	Q3Dict<QString>* textPtr, QWidget* parent 
) : SCCMouseEvent ( parent ) {
	mX = 0;
	mY = 0;
	mTextPtr = textPtr;
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	setBackgroundMode( Qt::NoBackground );
	Q3BoxLayout* layer1 = new Q3VBoxLayout (this);
	mStatus = new QStatusBar ( this );
	mStatus -> setSizeGripEnabled (false);
	//mStatus -> setBackgroundColor (QColor(white));
	layer1 -> addStretch (20);
	layer1 -> addWidget (mStatus);
	mPixmap[0] = new QPixmap (PIX_DEFAULT);
	mPixmap[1] = new QPixmap (PIX_LEFT);
	mPixmap[2] = new QPixmap (PIX_MIDDLE);
	mPixmap[3] = new QPixmap (PIX_RIGHT);
	mPixmap[4] = new QPixmap (PIX_FRONT);
	mPixmap[5] = new QPixmap (PIX_BACK);
	mTimer = new QTimer( this );
	connect (
		mTimer, SIGNAL (timeout()), this, SLOT (timerDone())
	);
	mID = 0;
	mBuffer = new QPixmap ();
	setFixedWidth  (mPixmap[0]->width() + 20);
	//setFixedHeight (mPixmap[0]->height() + mStatus->height());
	mStatus -> message (mText["MouseTestReady"]);
	paintEvent (0);
	show();
}

//====================================
// paintEvent...
//------------------------------------
void SCCMouseTest::paintEvent( QPaintEvent* ) {
	QPainter pDC;
	QColor   pColor = mStatus -> backgroundColor();
	// ...
	// Init buffer and device context
	// ---
	if (mID == 0) {
		mBuffer -> resize ( width(),height() );
		mBuffer -> fill ( pColor );
	}
	pDC.begin( mBuffer );
	// ...
	// draw pixmap and save current position as
	// old position for next repaint
	// ---
	pDC.drawPixmap ( mX,mY,*mPixmap[mID] );
	bitBlt( this, 0, 0, mBuffer );
	pDC.end();
}

//====================================
// set new pixmap...
//------------------------------------
void SCCMouseTest::setPixmap (int id) {
	mID = id; repaint();
}

//====================================
// single click...
//------------------------------------
void SCCMouseTest::singleClickEvent (int) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mStatus -> message (mText["SingleClick"]);
}

//====================================
// double click...
//------------------------------------
void SCCMouseTest::doubleClickEvent (int) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mStatus -> message (mText["DoubleClick"]);
}

//====================================
// wheel click...
//------------------------------------
void SCCMouseTest::wheelEvent (int btn) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	if (btn > 0) {
		setPixmap (4);
	} else {
		setPixmap (0);
		setPixmap (5);
	}
	mStatus -> message (mText["WheelMoved"]);
	mTimer->start ( 500, TRUE );
}

//====================================
// press event...
//------------------------------------
void SCCMouseTest::pressEvent (int btn) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mStatus -> message (mText["ButtonPressed"]);
	switch (btn) {
	case 1:
		setPixmap (1); break;
	case 4:
		setPixmap (2); break;
	case 2:
		setPixmap (3); break;
	}
}

//====================================
// release event...
//------------------------------------
void SCCMouseTest::releaseEvent (int) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mStatus -> message (mText["ButtonReleased"]);
	setPixmap (0);
}

//====================================
// move event...
//------------------------------------
void SCCMouseTest::moveEvent (int x,int y) {
	QString position;
	QString xPos;
	QString yPos;
	if (x >= 0) {
		xPos.sprintf("+%d",x);
	} else {
		xPos.sprintf("%d",x);
	}
	if (y >= 0) {
		yPos.sprintf("+%d",y);
	} else {
		yPos.sprintf("%d",y);
	}
	position.sprintf("%s , %s",
		xPos.ascii(),yPos.ascii()
	);
	mStatus -> message (position);
}

//====================================
// Timer for double clicks...
//------------------------------------
void SCCMouseTest::timerDone ( void ) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mStatus -> message (mText["WheelReleased"]);
	setPixmap (0);
}
} // end namespace
