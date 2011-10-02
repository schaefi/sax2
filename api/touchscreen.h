/**************
FILE          : touchscreen.h
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
              : - touchscreen.h: SaXGUI::SCCTouchScreen header definitions
              : - touchscreen.cpp: configure touchscreen system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCTOUCHSCREEN_H
#define SCCTOUCHSCREEN_H 1

//=====================================
// Includes
//-------------------------------------
#include <qtabwidget.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "touchscreen/touchselection.h"

namespace SaXGUI {
//====================================
// Class SCCTouchScreen
//------------------------------------
class SCCTouchScreen : public SCCDialog {
	Q_OBJECT

	private:
	int mCard;

	private:
	QList<SCCTouchSelection*>  mTouchDisplay;

	private:
	QTabWidget* mTouchscreenTab;

	public:
	bool exportData ( void );
	void init   ( void );
	void import ( void );

	public:
	SCCTouchScreen (
		Q3WidgetStack*,Q3Dict<QString>*,
		Q3Dict<SaXImport>, QWidget*
	);
};
} // end namespace
#endif
