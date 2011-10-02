/**************
FILE          : extensions.cpp
***************
PROJECT       : SaX2 - library interface
              :
AUTHOR        : Marcus Schäfer <ms@suse.de>
              :
BELONGS TO    : SaX2 - SuSE advanced X11 configuration 
              : 
              :
DESCRIPTION   : native C++ class library to access SaX2
              : functionality. Easy to use interface for
              : //.../
              : - importing/exporting X11 configurations
              : - modifying/creating X11 configurations 
              : ---
              :
              :
STATUS        : Status: Development
**************/
#include "extensions.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXManipulateExtensions::SaXManipulateExtensions (SaXImport* in) {
	// .../
	//! An object of this type is used to configure the
	//! server extensions: XEVIE,Composite,DAMAGE and XFIXES
	// ----
	if ( ! in ) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if ( in->getSectionID() != SAX_EXTENSIONS ) {
		excExtensionsImportBindFailed ( in->getSectionID() );
		qError (errorString(),EXC_EXTENSIONSIMPORTBINDFAILED);
		return;
	}
	mImport = in;
	mExtensions = 0;
	mImport -> setID ( mExtensions );
}

//====================================
// More to come for the extensions...
//------------------------------------
// ...

//====================================
// Constructor...
//------------------------------------
SaXManipulateVNC::SaXManipulateVNC (
	SaXImport* card,SaXImport* pointer,SaXImport* kbd,
	SaXImport* layout,SaXImport* path,int id
) : SaXManipulateVNCIF (card,id) {
	// .../
	//! An object of this type is used to manipulate the
	//! configuration to be able to accesss the X-Server via VNC
	// ----
	mManipPointer  = new SaXManipulateDevices (pointer,layout);
	mManipKeyboard = new SaXManipulateDevices (kbd,layout);
	mManipModules  = new SaXManipulatePath    (path);
	mVNCMouse      = -1;
	mVNCKeyboard   = -1;
	if ( layout->getSectionID() != SAX_LAYOUT ) {
		excLayoutImportBindFailed ( layout->getSectionID() );
		qError (errorString(),EXC_LAYOUTIMPORTBINDFAILED);
		return;
	}
	mLayout   = layout;
	mPointer  = pointer;
	mKeyboard = kbd;
}

//====================================
// enableVNC
//------------------------------------
void SaXManipulateVNC::enableVNC (void) {
	// .../
	//! export the display using the VNC protocol. This method
	//! will enable VNC by loading the vnc module and applying the
	//! usevnc option into the Device section
	// ----
	removeCardOption ("usevnc");
	mManipModules->removeLoadableModule ("vnc");
	addCardOption ("usevnc","yes");
	mManipModules->addLoadableModule ("vnc");
}

//====================================
// disableVNC
//------------------------------------
void SaXManipulateVNC::disableVNC (void) {
	// .../
	//! disable VNC support by unloading the vnc module and
	//! switch off the service with: usevnc -> no
	//! NOTE: The X-Server must be restarted in order the
	//! changes to become effective
	// ----
	removeCardOption ("usevnc");
	addCardOption ("usevnc","no");
	mManipModules->removeLoadableModule ("vnc");
}

//====================================
// setPassword
//------------------------------------
void SaXManipulateVNC::setPassword (const QString& pwd) {
	// .../
	//! restricted access can be set up by creating a
	//! password file. This file is set within the rfbauth
	//! option. This method is used to create the password
	//! file by using the vncp program
	// ----
	if (! setLock()) {
		return;
	}
	SaXProcessCall* proc = new SaXProcessCall ();
	proc -> addArgument ( SAX_CREATE_VNC_PWD );
	proc -> addArgument ( pwd );
	if ( ! proc -> start() ) {
		excProcessFailed();
		qError (errorString(),EXC_PROCESSFAILED);
		unsetLock();
		return;
	}
	unsetLock();
}

//====================================
// enablePasswordProtection
//------------------------------------
void SaXManipulateVNC::enablePasswordProtection (const QString& pwd) {
	// .../
	//! enable the password protection by setting a password
	//! and activating it with the rfbauth option. A user who
	//! will access the display is asked for this password while
	//! connecting
	// ----
	setPassword (pwd);
	disablePasswordProtection();
	addCardOption ("rfbauth","/root/.vnc/passwd");
}

//====================================
// disablePasswordProtection
//------------------------------------
void SaXManipulateVNC::disablePasswordProtection (void) {
	// .../
	//! disable the password protection will not remove the
	//! passwd file. Only the rfbauth option is removed which
	//! will disable the password prompt while connecting
	// ----
	removeCardOption ("rfbauth");
}

//====================================
// enableHTTPAccess
//------------------------------------
void SaXManipulateVNC::enableHTTPAccess (int port) {
	// .../
	//! enable VNC HTTP access on the given port. This is
	//! done by setting up the httpdir option in combination
	//! with the httpport option
	// ----
	QString HTTPport;
	QTextOStream (&HTTPport) << port;
	disableHTTPAccess();
	addCardOption ("httpdir","/usr/share/vnc/classes");
	addCardOption ("httpport",HTTPport);
}

//====================================
// disableHTTPAccess
//------------------------------------
void SaXManipulateVNC::disableHTTPAccess (void) {
	// .../
	//! disable HTTP access by removing the httpdir and
	//! httpport options
	// ----
	removeCardOption ("httpdir");
	removeCardOption ("httpport");
}

//====================================
// allowMultipleConnections
//------------------------------------
void SaXManipulateVNC::allowMultipleConnections (bool allow) {
	// .../
	//! allow multiple connections at the same time. default
	//! is to allow only one connection at the same time
	// ----
	if (allow) {
		removeCardOption ("alwaysshared");
		addCardOption    ("alwaysshared",0);
	} else {
		removeCardOption ("alwaysshared");
	}
}

//====================================
// addVNCKeyboard
//------------------------------------
void SaXManipulateVNC::addVNCKeyboard (void) {
	// .../
	//! add the VNC keyboard to handle keyboard events
	//! properly
	// ----
	if (! mLayout) {
		return;
	}
	if (mVNCKeyboard == -1) {
		QString extraLink;
		mVNCKeyboard = mManipKeyboard->addInputDevice (SAX_INPUT_VNC);
		QTextOStream (&extraLink) << mVNCMouse << " " << mVNCKeyboard;
		mLayout -> setItem ( "VNC", extraLink );
	}
}

//====================================
// addVNCMouse
//------------------------------------
void SaXManipulateVNC::addVNCMouse (void) {
	// .../
	//! add the VNC mouse to handle mouse events properly
	// ----
	if (! mLayout) {
		return;
	}
	if (mVNCMouse == -1) {
		QString extraLink;
		mVNCMouse = mManipPointer->addInputDevice (SAX_INPUT_VNC);
		QTextOStream (&extraLink) << mVNCMouse << " " << mVNCKeyboard;
		mLayout -> setItem ( "VNC", extraLink );
	}
}

//====================================
// removeVNCMouse
//------------------------------------
void SaXManipulateVNC::removeVNCMouse (void) {
	// .../
	//! remove the VNC mouse -> no mouse events in VNC session
	//
	if (! mLayout) {
		return;
	}
	if (mVNCMouse == -1) {
		mVNCMouse = searchVNCDevice ( SAX_VNC_MOUSE );
	}
	mManipPointer->removeInputDevice (mVNCMouse);
	mLayout -> setItem ("VNC","");
	mVNCMouse = -1;
}

//====================================
// removeVNCKeyboard
//------------------------------------
void SaXManipulateVNC::removeVNCKeyboard (void) {
	// .../
	//! remove the VNC keyboard -> no kbd events in VNC session
	// ----
	if (! mLayout) {
		return;
	}
	if (mVNCKeyboard == -1) {
		mVNCKeyboard = searchVNCDevice ( SAX_VNC_KEYBOARD );
	}
	mManipKeyboard->removeInputDevice (mVNCKeyboard);
	mLayout -> setItem ("VNC","");
	mVNCKeyboard = -1;
}

//====================================
// searchVNCDevice
//------------------------------------
int SaXManipulateVNC::searchVNCDevice ( int flag ) {
	SaXManipulateMice mManipInput ( mPointer );
	SaXManipulateKeyboard mManipKeyboard ( mKeyboard );
	int input1Count = mPointer->getCount();
	int input2Count = mKeyboard->getCount();
	int pINPUT = mPointer->getCurrentID();
	int pKBD   = mKeyboard->getCurrentID();
	int result = -1;
	if (flag == SAX_VNC_MOUSE) {
		for (int i=input1Count;i>=0;i--) {
		if (mManipInput.selectPointer (i)) {
			QString driver = mManipInput.getDriver();
			if (driver == "rfbmouse") {
				result = i; break;
			}
		}
		}
	}
	if (flag == SAX_VNC_KEYBOARD) {
		for (int i=input2Count;i>=0;i--) {
		if (mManipKeyboard.selectKeyboard (i)) {
			QString driver = mManipKeyboard.getDriver();
			if (driver == "rfbkeyb") {
				result = i; break;
			}
		}
		}
	} 
	mManipInput.selectPointer ( pINPUT );
	mManipKeyboard.selectKeyboard ( pKBD );
	return result;
}

//====================================
// VNCEnabled
//------------------------------------
bool SaXManipulateVNC::isVNCEnabled (void) {
	// .../
	//! check if VNC has been enabled
	// ----
	Q3Dict<QString> options = getOptions();
	if ((options["usevnc"]) && (*options["usevnc"] == "yes")) {
		return true;
	}
	return false;
}

//====================================
// HTTPAccessEnabled
//------------------------------------
bool SaXManipulateVNC::isHTTPAccessEnabled (void) {
	// .../
	//! check if HTTP access is enabled
	// ----
	Q3Dict<QString> options = getOptions();
	if (options["httpport"]) {
		return true;
	}
	return false;
}

//====================================
// multiConnectEnabled
//------------------------------------
bool SaXManipulateVNC::isMultiConnectEnabled (void) {
	// .../
	//! check if multiple connections at the same
	//! time are allowed
	// ----
	Q3Dict<QString> options = getOptions();
	if (options["alwaysshared"]) {
		return true;
	}
	return false;
}

//====================================
// pwdProtectionEnabled
//------------------------------------
bool SaXManipulateVNC::isPwdProtectionEnabled (void) {
	// .../
	//! check if the password protection is enabled
	// ----
	Q3Dict<QString> options = getOptions();
	if (options["rfbauth"]) {
		return true;
	}
	return false;
}

//====================================
// getHTTPPort
//------------------------------------
int SaXManipulateVNC::getHTTPPort (void) {
	// .../
	//! check if HTTP access is activated and return
	//! the current port number set for HTTP access
	// ----
	Q3Dict<QString> options = getOptions();
	if (options["httpport"]) {
		return options["httpport"]->toInt();
    }
	return 0;
}
} // end namespace
