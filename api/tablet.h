/**************
FILE          : tablet.h
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
              : - tablet.h: SaXGUI::SCCTablet header definitions
              : - tablet.cpp: configure tablet system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCTABLET_H
#define SCCTABLET_H 1

//=====================================
// Includes
//-------------------------------------
#include <qpushbutton.h>
#include <qtabwidget.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"
#include "tablet/tabletselection.h"
#include "tablet/tabletconnection.h"

//=====================================
// Defines
//-------------------------------------
#define MACROS_DIR  "/usr/share/sax/api/macros"
#define USB_PORT    MACROS_DIR "/getUSBTabletPort"

namespace SaXGUI {
//====================================
// Pre-class definitions
//------------------------------------
class SCCTabletPens;

//====================================
// Class SCCTablet
//------------------------------------
class SCCTablet : public SCCDialog {
	Q_OBJECT

	private:
	QTabWidget*          mTabletTab;
	SCCTabletConnection* mTabletConnection;
	SCCTabletSelection*  mTabletSelection;
	SCCTabletPens*       mTabletPens;

	public:
	bool exportData ( void );
	void enablePens ( bool );
	void init   ( void );
	void import ( void );

	public:
	SCCTabletSelection*  getSelectionData  ( void );
	SCCTabletConnection* getConnectionData ( void );

	public slots:
	void slotActivate ( bool );

	public:
	SCCTablet (
		Q3WidgetStack*,Q3Dict<QString>*,
		Q3Dict<SaXImport>, QWidget*
	);
};
} // end namespace

//=====================================
// Includes
//-------------------------------------
#include "tablet/tabletpens.h"

#endif
