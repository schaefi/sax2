/**************
FILE          : vncdisplay.h
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
              : - vncdisplay.h: SaXGUI::SCCVNCDisplay header defs
              : - vncdisplay.cpp: configure VNC system
              : ----
              :
STATUS        : Status: Development
**************/
#ifndef SCCVNC_DISPLAY_H
#define SCCVNC_DISPLAY_H 1

//=====================================
// Includes
//-------------------------------------
#include <qcheckbox.h>
#include <qlabel.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <qspinbox.h>
#include <q3vbox.h>

//=====================================
// Includes
//-------------------------------------
#include "dialog.h"

namespace SaXGUI {
//====================================
// Class SCCTabletSelection
//------------------------------------
class SCCVNCDisplay : public SCCDialog {
	Q_OBJECT

	private:
	//====================================
	// private dialog data for exporting
	//------------------------------------
	bool           mEnabled;

	private:
	//====================================
	// private widget data contents
	//------------------------------------
	QString        mPWD;

	private:
	Q3BoxLayout*    mMainLayout;

	private:
	QCheckBox*     mCheckVNC;
	Q3ButtonGroup*  mOptions;
	QCheckBox*     mCheckPWD;
	Q3VBox*         mEntries;
	QLineEdit*     mPWD1;
	QLineEdit*     mPWD2;
	QCheckBox*     mCheckShared;
	QCheckBox*     mCheckHTTP;
	Q3VBox*         mHTTPEntries;
	QSpinBox*      mHTTP;

	public:
	bool isEnabled      ( void );
	bool isHTTPEnabled  ( void );
	bool isPWDProtected ( void );
	bool isShared       ( void );
	int  getHTTPPort    ( void );
	bool checkPassword  ( void );
	QString getPassword ( void );

	public:
	void init   ( void );
	void import ( void );

	public slots:
	void slotInput1Changed ( const QString& );
	void slotActivateVNC   ( bool );
	void slotPassword      ( bool );
	void slotHTTP          ( bool );

	public:
	SCCVNCDisplay (
		Q3Dict<QString>*, Q3Dict<SaXImport>,
		int, QWidget*
	);
};
} // end namespace

#endif
