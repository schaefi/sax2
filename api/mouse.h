/**************
FILE          : mouse.h
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
              : - mouse.h: SaXGUI::SCCMouse header definitions
              : - mouse.cpp: configure mouse system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCMOUSE_H
#define SCCMOUSE_H 1

//=====================================
// Includes
//-------------------------------------
#include <qpushbutton.h>
#include <qtabwidget.h>
#include <qlist.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "mouse/mousedisplay.h"

namespace SaXGUI {
//====================================
// Class SCCMouse
//------------------------------------
class SCCMouse : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	int  mMouse;

	private:
	QList<SCCMouseDisplay*>  mMouseDisplay;

	private:
	QTabWidget* mMouseTab;

	public:
	bool exportData ( void );
	void init   ( void );
	void import ( void );

	public slots:
	void slotActivate   ( void );

	public:
	SCCMouse (
		Q3WidgetStack*,Q3Dict<QString>*,
		Q3Dict<SaXImport>, QWidget*
	);
};
} // end namespace
#endif
