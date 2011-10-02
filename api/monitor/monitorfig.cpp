/**************
FILE          : monitorfig.cpp
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
              : - monitorfig.h: SaXGUI::SCCFig header definitions
              : - monitorfig.cpp: figure objects of a matrix widget
              : ----
              :
STATUS        : Status: Development
**************/
#include <stdio.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <q3frame.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>
#include <math.h>

#include "monitorfig.h"

namespace SaXGUI {
//====================================
// SCCFig Constructor
//------------------------------------
SCCFig::SCCFig (
	QWidget *p,int x,int y,int index,int id,bool multi
) : QPushButton( p ) {
	if (multi) {
		mPixmap = new QPixmap (FIGURE_MULTI);
	} else {
		mPixmap = new QPixmap (FIGURE_SINGLE);
	}
	tipText = new QString();
    setPixmap   (*mPixmap);
	setGeometry (mPixmap->rect());
	setFlat (TRUE);
	mCurrentx = x;
	mCurrenty = y;
	mIndex    = index;
	mID       = id;
	move (x-(width()/2),y-(height()/2));
}

//====================================
// SCCFig Destructor
//------------------------------------
SCCFig::~SCCFig ( void ) {
	QToolTip::remove (this);
}

//====================================
// SCCFig set tool tip for this widget
//------------------------------------
void SCCFig::setToolTip ( const QString& text ) {
	if (!text.isNull()) {
	tipText->sprintf("%s",text.ascii());
	QToolTip::add (this,text);
	}
}

//====================================
// SCCFig set pixmap for this widget
//------------------------------------
void SCCFig::setFigurePixmap ( QPixmap pix ) {
	*mPixmap = pix;
}

//====================================
// receive mouse move event
//------------------------------------
void SCCFig::mouseMoveEvent ( QMouseEvent* m ) {
	// ...
	// move the button widget acording to the mouse movement
	// and save the current position as middle point
	// coordinates
	// ------------
	QRect geo = frameGeometry();
	int x = (geo.x() + m->x()) - mStartx;
	int y = (geo.y() + m->y()) - mStarty;
	move (mStartg.x()+x,mStartg.y()+y);
	mCurrentx = geo.x() + ( width()  / 2 );
	mCurrenty = geo.y() + ( height() / 2 );
}

//====================================
// receive mouse press event
//------------------------------------
void SCCFig::mousePressEvent ( QMouseEvent* m ) {
	// ...
	// save the start point if we receive a press
	// event from the mouse (drag start)
	// ----------------------------------
	QRect geo = frameGeometry();
	mStartx = geo.x() + m->x();
	mStarty = geo.y() + m->y();
	mStartg = geo;
}

//====================================
// receive mouse double click event
//------------------------------------
void SCCFig::mouseDoubleClickEvent ( QMouseEvent* ) {
	sigPressed ( mID );
}

//====================================
// receive release event
//------------------------------------
void SCCFig::mouseReleaseEvent ( QMouseEvent* ) {
	// ...
	// send signal with the SCCFig object used
	// for this drag action
	// ---------------------
	sigReleased (this);
}

//====================================
// paint me: a flat button :-)
//------------------------------------
void SCCFig::paintEvent ( QPaintEvent * p ) {
	QPushButton::paintEvent (p);
}

//====================================
// get current middle point
//------------------------------------
QPoint SCCFig::getPosition ( void ) {
	QPoint pos;
	pos.setX ((int)mCurrentx);
	pos.setY ((int)mCurrenty);
	return (pos);
}

//====================================
// calculate dist between me and X/Y
//------------------------------------
int SCCFig::getDistance ( int x, int y ) {
	if ((x == 0) || (y == 0)) {
		return (-1);
	}
	float kx = mCurrentx - x;
	float ky = mCurrenty - y;
	float dist = sqrt ((kx*kx) + (ky*ky));
	return ((int)dist);
}

//====================================
// calculate distance QPoint wrapper
//------------------------------------
int SCCFig::getDistance ( QPoint p ) {
	return (getDistance (p.x(),p.y()));
}

//====================================
// move widget to middle point x/y
//------------------------------------
void SCCFig::moveTo ( int x, int y) {
    move (x - width()/2, y - height()/2);
    mCurrentx = x;
    mCurrenty = y;
}

//====================================
// set index number for this widget
//------------------------------------
void SCCFig::setIndex ( int index ) {
	mIndex = index;
}

//====================================
// get currently used tool tip text
//------------------------------------
const char* SCCFig::getTip ( void ) {
	return (tipText->ascii());
}

//====================================
// get index number 
//------------------------------------
int SCCFig::getIndex ( void ) {
	return (mIndex);
}

//====================================
// get widget ID
//------------------------------------
int SCCFig::getID ( void ) {
	return (mID);
}
} // end namespace
