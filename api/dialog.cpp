/**************
FILE          : dialog.cpp
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
              : - dialog.h: SaXGUI::SCCDialog header definitions
              : - dialog.cpp: build base class for all dialogs
              : ----
              :
STATUS        : Status: Development
**************/
#include "dialog.h"
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3Frame>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCDialog::SCCDialog (
	Q3WidgetStack* stack, Q3Dict<QString>* text,
	Q3Dict<SaXImport> section, QWidget* parent
) {
	//====================================
	// add frame and layout to stack
	//------------------------------------
	if (stack) {
		mDialogStack = stack;
		mDialogFrame  = new Q3Frame ( mDialogStack );
		mDialogLayout = new Q3VBoxLayout ( mDialogFrame );
		mDialogID = mDialogStack -> addWidget ( mDialogFrame );
	} else {
		mDialogID = -1;
	}

	//====================================
	// set init and import flags to true
	//------------------------------------
	needImportData = true;
	needInitData   = true;

	//====================================
	// save imported sections
	//------------------------------------
	mSection = section;

	//====================================
	// save base frame pointer
	//------------------------------------
	mTextPtr = text;

	//====================================
	// save parent widget pointer
	//------------------------------------
	mParent = parent;
}

//====================================
// init
//------------------------------------
void SCCDialog::init ( void ) {
	// .../
	// default init method, should be overloaded to implement
	// the initializing task. This includes setting up all
	// the widget contents of the dialog for selections
	// popups and dynamic widget creation
	// ----
	log (L_WARN,"SCCDialog::Warning: default init() method called\n");
	needInitData = false;
}

//====================================
// import
//------------------------------------
void SCCDialog::import ( void ) {
	// .../
	// default import method, should be overloaded to implement
	// displaying the current configuration according to the
	// use of this dialog
	// ----
	log (L_WARN,"SCCDialog::Warning: default import() method called\n");
	needImportData = false;
}

//====================================
// getDialogID
//------------------------------------
int SCCDialog::getDialogID ( void ) {
	return mDialogID;
}

//====================================
// needInit
//------------------------------------
bool SCCDialog::needInit ( void ) {
	return needInitData;
}

//====================================
// needImport
//------------------------------------
bool SCCDialog::needImport ( void ) {
	return needImportData;
}
//============================================
// Center window according to parent
//--------------------------------------------
QPoint SCCDialog::getPosition ( int w, int h ) {
	QPoint result ( 0,0 );
	if ( qApp->mainWidget() ) {
		int x0 = qApp->mainWidget()->x();
		int y0 = qApp->mainWidget()->y();
		result.setX (
			x0 + ((qApp->mainWidget()->width()  - w) / 2)
		);
		result.setY (
			y0 + ((qApp->mainWidget()->height() - h) / 2)
		);
	}
	return result;
}
} // end namespace
