/**************
FILE          : mouseevent.h
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
              : - mouseevent.h: SaXGUI::SCCMouseEvent header defs
              : - mouseevent.cpp: catch mouse events
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMOUSE_EVENT_H
#define SCCMOUSE_EVENT_H 1

//====================================
// Includes...
//------------------------------------
#include <qwidget.h>
#include <qtimer.h>
//Added by qt3to4:
#include <QEvent>

//====================================
// Definitions...
//------------------------------------
#define WHEEL_DELTA 120

namespace SaXGUI {
//====================================
// Class Definitions...
//------------------------------------
class SCCMouseEvent : public QWidget {
	Q_OBJECT

	private:
	QTimer *timer;
	int mBtn;
	int doubleClick;
	int mouseReleased;

	public:
	SCCMouseEvent ( QWidget* = 0);

    public:
    virtual bool eventFilter ( QObject*, QEvent* );

	public:
	virtual void doubleClickEvent (int);
	virtual void singleClickEvent (int);
	virtual void wheelEvent (int);
	virtual void pressEvent (int);
	virtual void releaseEvent (int);
	virtual void moveEvent (int,int);

	public slots:
	void timerDone (void);
};
} // end namespace
#endif
