/**************
FILE          : vnc.h
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
              : - vnc.h: SaXGUI::SCCVNC header definitions
              : - vnc.cpp: configure VNC system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCVNC_H
#define SCCVNC_H 1

//=====================================
// Includes
//-------------------------------------
#include <qtabwidget.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "vnc/vncdisplay.h"

namespace SaXGUI {
//====================================
// Class SCCVNC
//------------------------------------
class SCCVNC : public SCCDialog {
	Q_OBJECT

	private:
	QTabWidget*     mVNCTab;
	SCCVNCDisplay*  mVNCDisplay;
	
	public:
	bool exportData ( void );
	void init   ( void );
	void import ( void );

	public:
	SCCVNC (
		Q3WidgetStack*,Q3Dict<QString>*,
		Q3Dict<SaXImport>, QWidget*
	);
};
} // end namespace
#endif
