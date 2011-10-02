/**************
FILE          : xidle.h
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
              : - xidle.h: MIT screensaver timer control widget
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCIDLE_H
#define SCCIDLE_H 1

#include <qtimer.h>
#include <qlcdnumber.h>
#include <locale.h>
#include <libintl.h>

//===================================
// Defines...
//-----------------------------------
#define XSS_TIMEOUT 30
#define TDOMAIN     "/usr/share/locale"

namespace SaXGUI {
//===================================
// Signal prototype...
//-----------------------------------
void startSignal (int);

//===================================
// SCCTimeElapsed class...
//-----------------------------------
class SCCIdleTimeElapsed : public QWidget {
	Q_OBJECT

	private:
	int mPID;
	QLCDNumber* LCD;

	private:
	int mTimeout;
	int mTime;
	Display* dpy;
	
	public:
	SCCIdleTimeElapsed (QString* =0,QWidget* =0, const char* =0);
	void buildWidget (void);

	protected:
	void timerEvent (QTimerEvent*);
};
} // end namespace
#endif
