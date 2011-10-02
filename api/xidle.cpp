/**************
FILE          : xidle.cpp
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
              : - xidle.cpp: MIT screensaver timer control implementation
              : ----
              :
STATUS        : Status: Development
**************/
#include "xidle.h"

#include <qapplication.h>
#include <qtimer.h>
#include <qlcdnumber.h>
#include <qlayout.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <X11/Xcms.h>
#include <X11/extensions/scrnsaver.h>

#include "common/log.h"

namespace SaXGUI {
//===================================
// Globals
//-----------------------------------
bool gotStartSignal = false;

//===================================
// SCCIdleTimeElapsed constructor...
//-----------------------------------
SCCIdleTimeElapsed::SCCIdleTimeElapsed (
	QString* pid,QWidget *parent,const char* name) : 
	QWidget (parent,name)
{
	dpy = x11Display();
	int timeout,interval;
	int prefer_blank,allow_exp;
	int ss_event;
	int ss_error;

	mPID = 0;
	if (pid) {
		mPID = pid->toInt();
	}

	mTimeout = XSS_TIMEOUT;
	buildWidget();

	signal (SIGUSR2 , startSignal);
	while (1) {
	if (gotStartSignal) {
		break;
	}
	sleep (1);
	}
	if (! XScreenSaverQueryExtension (dpy, &ss_event, &ss_error)) {
	log ( L_ERROR,
		"XTimeElapsed::could not init MIT extension\n"
	);
	}
	XSync (dpy, false);
	XGetScreenSaver (
		dpy,&timeout,&interval,
		&prefer_blank,&allow_exp
	);
	timeout = mTimeout;
	XSetScreenSaver (
		dpy,timeout,interval,prefer_blank,allow_exp
	);
	startTimer (1000);
}

//===================================
// SCCIdleTimeElapsed build widget...
//-----------------------------------
void SCCIdleTimeElapsed::buildWidget (void) {
	setGeometry(20,20,150,70);

	setlocale (LC_ALL,"");
	bindtextdomain ("sax", TDOMAIN);
	textdomain ("sax");

	QString caption = gettext("Idle timer...");

	setCaption (QString::fromLocal8Bit(caption));
	QBoxLayout* layout = new QVBoxLayout (this);
	LCD = new QLCDNumber ( 2, this );
	LCD -> display ( mTimeout );
	layout -> addWidget ( LCD );
}

//===================================
// SCCIdleTimeElapsed timer event...
//-----------------------------------
void SCCIdleTimeElapsed::timerEvent (QTimerEvent*) {
	mTime = mTimeout;
	XScreenSaverInfo *info;
	info = XScreenSaverAllocInfo();
	XScreenSaverQueryInfo (
		dpy, DefaultRootWindow(dpy), info
	);
	mTime = (int)(info->til_or_since/1000);
	LCD -> display (mTime);
	if (mTime == 0) {
		if (mPID > 0) {
			kill (mPID,15);
		} else {
			kill (getppid(),15);
		}
		exit (0);
	}
}

//===================================
// startSignal
//-----------------------------------
void startSignal (int) {
	gotStartSignal = true;
}
} // end namespace
