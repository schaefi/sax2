/**************
FILE          : tabletpadproperty.cpp
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
              : - tabletpadproperty.h: SaXGUI::SCCTabletPadProperty implement.
              : - tabletpadproperty.cpp: configure tablet system
              : ----
              :
STATUS        : Status: Development
**************/
#include "tabletpadproperty.h"

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCTabletPadProperty::SCCTabletPadProperty (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section,
	const QString& penname, int display,QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	// ...
	log (L_INFO,"*** not yet handled: %s on display %d\n",
		penname.ascii(),display
	);
}
//====================================
// getID
//------------------------------------
int SCCTabletPadProperty::getID ( void ) {
	return mDisplay;
}
//====================================
// setID
//------------------------------------
void SCCTabletPadProperty::setID ( int id ) {
	mDisplay = id;
}
} // end namespace
