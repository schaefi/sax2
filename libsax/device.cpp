/**************
FILE          : device.cpp
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
#include "device.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXManipulateDevices::SaXManipulateDevices (
	SaXImport* desktop ,SaXImport* card ,SaXImport* layout
) {
	// .../
	//! an object of this type is used to create or remove new
	//! desktops for further configuration. A desktop is defined
	//! by the Monitor,Modes,Screen and ServerLayout sections.
	//! Adding a new desktop will add a new Monitor,Modes and Screen
	//! section. The section gets activated by chaning the global
	//! ServerLayout section. Removing a desktop will do the same
	//! vice versa
	// ----
	desktopHandlingAllowed = false;
	inputHandlingAllowed   = false;
	if ( (!desktop) || (!card) || (!layout)) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if ( desktop->getSectionID() != SAX_DESKTOP ) {
		excDesktopImportBindFailed ( desktop->getSectionID() );
		qError (errorString(),EXC_DESKTOPIMPORTBINDFAILED);
		return;
	}
	if ( card->getSectionID() != SAX_CARD ) {
		excCardImportBindFailed ( card->getSectionID() );
		qError (errorString(),EXC_CARDIMPORTBINDFAILED);
		return;
	}
	if ( layout->getSectionID() != SAX_LAYOUT ) {
		excLayoutImportBindFailed ( layout->getSectionID() );
		qError (errorString(),EXC_LAYOUTIMPORTBINDFAILED);
		return;
	}
	desktopHandlingAllowed = true;
	mDesktop = desktop;
	mCard    = card;
	mLayout  = layout;
	mInput   = 0;
}

//====================================
// Constructor...
//------------------------------------
SaXManipulateDevices::SaXManipulateDevices (
	SaXImport* input, SaXImport* layout
) {
	// .../
	//! an object of this type is used to create or remove new
	//! input devices. An input device is a mouse,tablet,touchscreen
	//! or keyboard. An input device is defined by its InputDevice
	//! section and is activated within the global ServerLayout
	//! section. Adding an input device requires setting the InputFashion
	//! which is the type of the device.
	// ----
	desktopHandlingAllowed = false;
	inputHandlingAllowed   = false;
	if ((!input) || (!layout)) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if (layout->getSectionID() != SAX_LAYOUT) {
		excLayoutImportBindFailed ( layout->getSectionID() );
		qError (errorString(),EXC_LAYOUTIMPORTBINDFAILED);
		return;
	}
	if (
		(input->getSectionID() != SAX_POINTERS) &&
		(input->getSectionID() != SAX_KEYBOARD)
	) {
		excPointerImportBindFailed ( input->getSectionID() );
		qError (errorString(),EXC_POINTERIMPORTBINDFAILED);
		return;
	}
	inputHandlingAllowed = true;
	mInput   = input;
	mLayout  = layout;
	mDesktop = 0;
	mCard    = 0;
}

//====================================
// addDesktopDevice
//------------------------------------
int SaXManipulateDevices::addDesktopDevice (void) {
	// .../
	//! add a new desktop at the end of the current desktop
	//! list. The new desktop ID is returned
	// ---
	if (! desktopHandlingAllowed) {
		return -1;
	}
	//====================================
	// determine new card/desktop ID...
	//------------------------------------
	int newID = mCard->getCount();
	if (! mCard->addID ( newID )) {
		return -1;
	}
	if (! mDesktop->addID ( newID )) {
		return -1;
	}
	//====================================
	// add new card/desktop sections...
	//------------------------------------
	QString newIDstring;
	newIDstring.sprintf ("%d",newID);
	mCard    -> setItem ("Identifier",QString("Device[" + newIDstring + "]"));
	mDesktop -> setItem ("Device"    ,QString("Device[" + newIDstring + "]"));
	mDesktop -> setItem ("Identifier",QString("Screen[" + newIDstring + "]"));
	mDesktop -> setItem ("Monitor"   ,QString("Monitor["+ newIDstring + "]"));
	//====================================
	// set some defaults...
	//------------------------------------
	mCard -> setItem ("Driver","fbdev");
	//====================================
	// update screen layout definition...
	//------------------------------------
	if (newID == 0) {
		mLayout -> setItem ("Screen:Screen[0]","<none> <none> <none> <none>");
		return mDesktop -> getCurrentID();
	}
	//====================================
	// update screen layout definition...
	//------------------------------------
	updateLayout (newID);
	return mDesktop -> getCurrentID();
}

//====================================
// addInputDevice
//------------------------------------
int SaXManipulateDevices::addInputDevice (const char* fashion) {
	// .../
	//! add a new input device at the end of the current input
	//! device list. The new input device ID is returned. pointer
	//! input devices are following the odd order (1,3,5,7...)
	//! whereas keyboard input devices use the even order (0,2,4,6...)
	// ----
	if (! inputHandlingAllowed) {
		return -1;
	}
	//====================================
	// check fashion string...
	//------------------------------------
	QString inputFashion (fashion);
	if (
		(inputFashion != SAX_INPUT_TOUCHPANEL) &&
		(inputFashion != SAX_INPUT_TABLET)     &&
		(inputFashion != SAX_INPUT_PEN)        &&
		(inputFashion != SAX_INPUT_TOUCH)      &&
		(inputFashion != SAX_INPUT_PAD)        &&
		(inputFashion != SAX_INPUT_ERASER)     &&
		(inputFashion != SAX_INPUT_MOUSE)      &&
		(inputFashion != SAX_INPUT_VNC)        &&
		(inputFashion != SAX_INPUT_KEYBOARD)
	) {
		excWrongInputFashion (fashion);
		qError (errorString(),EXC_WRONGINPUTFASHION);
		return -1;
	}
	//====================================
	// determine new input device ID...
	//------------------------------------
	QString baseID ("Mouse");
	QString baseDriver ("mouse");
	if (fashion == (char*)SAX_INPUT_VNC) {
		baseDriver = "rfbmouse";
	}
	Q3Dict<QString>* data = mInput->getTablePointer (0);
	int newID = mInput->getCount (true) * 2 + 1;
	if ((data) && (! data->isEmpty())) {
		baseDriver ="kbd";
		if (fashion == (char*)SAX_INPUT_VNC) {
			baseDriver = "rfbkeyb";
		}
		baseID = "Keyboard";
		newID  = mInput->getCount (true) * 2;
	}
	if (! mInput -> addID (newID)) {
		return -1;
	}
	//====================================
	// add new input device section...
	//------------------------------------
	QString newIDstring;
	newIDstring.sprintf ("%d",newID);
	mInput -> setItem ("Identifier",baseID + "[" + newIDstring + "]");
	mInput -> setItem ("InputFashion",fashion);
	//====================================
	// set some defaults...
	//------------------------------------
	mInput -> setItem ("Driver",baseDriver);
	//====================================
	// update server layout
	//------------------------------------
	if (baseID == "Mouse") {
		QString inputDevice; QTextOStream (&inputDevice) 
			<< mLayout -> getItem ("InputDevice")
			<< ",Mouse[" << newIDstring << "]";
		mLayout -> setItem ("InputDevice",inputDevice);
	}
	if (baseID == "Keyboard") {
		QString inputDevice; QTextOStream (&inputDevice)
			<< mLayout -> getItem ("Keyboard")
			<< ",Keyboard[" << newIDstring << "]";
		mLayout -> setItem ("Keyboard",inputDevice);
	}
	return mInput -> getCurrentID();
}

//====================================
// removeDesktopDevice
//------------------------------------
int SaXManipulateDevices::removeDesktopDevice (int id) {
	// .../
	//! remove the desktop of the given id (id) and return the
	//! previous desktop id. The current desktop is set to this
	//! previous desktop id. If the desktop does not exist or
	//! the desktop id is the core (0) desktop the method will
	//! return (-1)
	// ----
	if (! desktopHandlingAllowed) {
		return -1;
	}
	//====================================
	// don't allow removing the core entry	
	//------------------------------------
	if (id <= 0) {
		excInvalidArgument (id);
		qError (errorString(),EXC_INVALIDARGUMENT);
		return -1;
	}
	//====================================
	// remove desktop...
	//------------------------------------
	if (! mCard->delID (id)) {
		return -1;
	}
	if (! mDesktop->delID (id)) {
		return -1;
	}
	int newID = id - 1;
	//====================================
	// select previous desktop
	//------------------------------------
	mCard    -> setID (newID);
	mDesktop -> setID (newID);
	//====================================
    // update screen layout definition...
    //------------------------------------
	updateLayout (mDesktop->getCount());
	return mDesktop -> getCurrentID();
}

//====================================
// removeInputDevice
//------------------------------------
int SaXManipulateDevices::removeInputDevice (int id) {
	// .../
	//! remove the input device of the given id (id) and return the
	//! previous input device id. The current input device id is set
	//! to this previous input device id. If the input device does
	//! not exist or the input device id is the core (0|1) mouse or
	//! keyboard the method will return (-1)
	// ----
	if (! inputHandlingAllowed) {
		return -1;
	}
	//====================================
	// don't allow removing the core entry   
	//------------------------------------
	if ((id <= 1) && (mInput->getCount() <= 2)) {
		excInvalidArgument (id);
		qError (errorString(),EXC_INVALIDARGUMENT);
		return -1;
	}
	//====================================
	// remove input devices...
	//------------------------------------
	if (! mInput->delID (id)) {
		return -1;
	}
	//====================================
	// select previous input device
	//------------------------------------
	for (int i=1;i<=2;i++) {
		Q3Dict<QString>* data = mInput->getTablePointer (i);
		if ((data) && (! data->isEmpty())) {
			mInput->setID (i);
			break;
		}
	}
	//====================================
	// check input device type
	//------------------------------------
	bool isMouse = true;
	QString baseItem ("InputDevice");
	QString baseID   ("Mouse");
	if (mInput->getCurrentID() % 2 == 0) {
		isMouse  = false;
		baseItem = "Keyboard";
		baseID   = "Keyboard";
	}
	//====================================
	// update server layout
	//------------------------------------
	QString result;
	QString IDstring;
	IDstring.sprintf ("%d",id);
	QString deviceList  = mLayout -> getItem (baseItem);
	QStringList optlist = QStringList::split ( ",", deviceList );
	for ( QStringList::Iterator
		in = optlist.begin(); in != optlist.end(); ++in
	) {
		QString item (*in);
		if (item == QString(baseID + "["+IDstring+"]")) {
			continue;
		}
		QRegExp identifier ("\\[(.+)\\]");
		int pos = identifier.search (item);
		if (pos >= 0) {
			int curID = identifier.cap(1).toInt();
			if ( curID > id ) {
				QString newMK;
				newMK.sprintf ("%s[%d],",baseID.ascii(),curID - 2);
				result.append ( newMK );
			} else {
				result.append (item+",");
			}
		}
	}
	result.remove (result.length()-1,result.length());
	mLayout -> setItem (baseItem,result);
	return mInput -> getCurrentID();
}

//====================================
// updateLayout
//------------------------------------
void SaXManipulateDevices::updateLayout (int newID) {
	// .../
	//! update the ServerLayout while adding or deleting a desktop
	//! this method is called only privately and is not published.
	//! Adding or deleting a desktop will overwrite the current
	//! layout specification to use a standard horizontal left to
	//! right layout specification.
	// ----
	//====================================
	// remove current screen layout
	//------------------------------------
	for (int n=0;n<=newID;n++) {
		QString idString; idString.sprintf ("%d",n);
		mLayout -> removeEntry (QString("Screen:Screen["+ idString + "]"));
	}
	//====================================
	// check number of existing screens
	//------------------------------------
	int existing = newID;
	for (int n=0;n<=newID;n++) {
		if (! mCard -> getTablePointer (n)) {
			existing--;
		}
	}
	//====================================
	// set standard layout if existing=0
	//------------------------------------
	if (existing == 0) {
		mLayout -> setItem (
			"Screen:Screen[0]","<none> <none> <none> <none>"
		);
		return;
	}
	//====================================
	// create standard horizontal layout
	//------------------------------------
	for (int n=0;n<=existing;n++) {
		QString baseID,val;
		baseID.sprintf ("%d",n);
		QString key ("Screen:Screen[" + baseID + "]");
		for (int pos=0;pos<4;pos++) {
			int posID = 0;
			QString idString;
			switch (pos) {
				case 0:
					posID = n - 1;
				break;
				case 1:
					posID = n + 1;
				break;
				default:
					idString = "<none>";
				break;
			};
			if (idString.isEmpty()) {
			if ((posID < 0) || (posID > newID)) {
				idString.sprintf ("<none>");
			} else {
				idString.sprintf ("Screen[%d]",posID);
			}
			}
			QTextOStream (&val)
				<< val << " " << idString;
		}
		val = val.stripWhiteSpace();
		mLayout -> setItem (key,val);
	}
}
} // end namespace
