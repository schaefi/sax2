/**************
FILE          : vnc.cpp
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
#include "vnc.h"

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCVNC::SCCVNC (
	Q3WidgetStack* stack,Q3Dict<QString>* text,
	Q3Dict<SaXImport> section, QWidget* parent
) : SCCDialog ( stack,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create VNC dialog
	//-------------------------------------
	mVNCTab = new QTabWidget  ( mDialogFrame );

	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mDialogLayout -> addWidget  ( mVNCTab );

	//=====================================
	// create basic VNC tab
	//-------------------------------------
	mVNCDisplay = new SCCVNCDisplay ( text,section,0,this );
	mVNCTab -> addTab (
		mVNCDisplay, mText["VNC"]
	);
	//=====================================
	// initialize basic VNC tab
	//-------------------------------------
	mVNCDisplay -> init();
	mVNCDisplay -> import();
}
//=====================================
// init
//-------------------------------------
void SCCVNC::init ( void ) {
	//====================================
	// set init state
	//------------------------------------
	needInitData = false;
}
//=====================================
// import
//-------------------------------------
void SCCVNC::import ( void ) {
	//====================================
	// set import state
	//------------------------------------
	needImportData = false;
}
//=====================================
// exportData
//-------------------------------------
bool SCCVNC::exportData ( void ) {
	//=====================================
	// check password...
	//-------------------------------------
	if ( (mVNCDisplay->isEnabled()) && (mVNCDisplay->isPWDProtected()) ) {
	if (! mVNCDisplay->checkPassword()) {
		return false;
	}
	}
	//=====================================
	// create manipulators...
	//-------------------------------------
	SaXManipulateVNC mVNC (
		mSection["Card"],mSection["Pointers"],mSection["Keyboard"],
		mSection["Layout"],mSection["Path"]
	);
	//=====================================
	// remove VNC configuration
	//-------------------------------------
	mVNC.disableVNC();
	mVNC.disableHTTPAccess();
	mVNC.disablePasswordProtection();
	mVNC.allowMultipleConnections (false);
	mVNC.removeVNCKeyboard();
	mVNC.removeVNCMouse();
	
	//=====================================
	// setup VNC configuration
	//-------------------------------------
	if ( mVNCDisplay->isEnabled()) {
		mVNC.enableVNC();
		mVNC.addVNCKeyboard();
		mVNC.addVNCMouse();

		if ( mVNCDisplay->isHTTPEnabled() ) {
			int HTTPPort = mVNCDisplay->getHTTPPort();
			mVNC.enableHTTPAccess ( HTTPPort );
		}
		if ( mVNCDisplay->isPWDProtected() ) {
			mVNC.enablePasswordProtection (
				mVNCDisplay->getPassword()
			);
		}
		if ( mVNCDisplay->isShared() ) {
			mVNC.allowMultipleConnections (true);
		}
		SCCMessage* mMessageBox = new SCCMessage (
			this, mTextPtr, SaXMessage::OK,
			"InfoVNC","InfoCaption"
		);
		mMessageBox -> showMessage();
	}
	return true;
}
} // end namespace
