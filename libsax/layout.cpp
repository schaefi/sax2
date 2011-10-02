/**************
FILE          : layout.cpp
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
#include "layout.h"

namespace SaX {
//====================================
// Constructor...
//------------------------------------
SaXManipulateLayout::SaXManipulateLayout (SaXImport* layout,SaXImport* card ) {
	// .../
	//! An object of this type is used to configure the
	//! server layout. This includes mainly the configuration
	//! of the multihead arrangement and the multihead mode
	// ----
	if ((! layout) || (! card)) {
		excNullPointerArgument ();
		qError (errorString(),EXC_NULLPOINTERARGUMENT);
		return;
	}
	if ( layout->getSectionID() != SAX_LAYOUT ) {
		excLayoutImportBindFailed ( layout->getSectionID() );
		qError (errorString(),EXC_LAYOUTIMPORTBINDFAILED);
		return;
	}
	if ( card->getSectionID() != SAX_CARD ) {
		excCardImportBindFailed ( card->getSectionID() );
		qError (errorString(),EXC_CARDIMPORTBINDFAILED);
		return;
	}
	mLayout = layout;
	mCard   = card;
	mLayout -> setID ( 0 );
	mCard   -> setID ( 0 );
}

//====================================
// setMultiheadMode
//------------------------------------
void SaXManipulateLayout::setXOrgMultiheadMode ( int mode ) {
	// .../
	//! set the multihead mode which can be one of the
	//! following settings: SAX_XINERAMA, SAX_TRADITIONAL or
	//! SAX_CLONE
	// ----
	switch (mode) {
		case SAX_XINERAMA:
			mLayout -> setItem ("Clone","off");
			mLayout -> setItem ("Xinerama","on");
		break;
		case SAX_TRADITIONAL:
			mLayout -> setItem ("Clone","off");
			mLayout -> setItem ("Xinerama","off");
		break;
		case SAX_CLONE:
			mLayout -> setItem ("Clone","on");
			mLayout -> setItem ("Xinerama","on");
		break;
		default:
			excInvalidArgument (mode);
			qError (errorString(),EXC_INVALIDARGUMENT);
		break;
	}
}

//====================================
// setRelative
//------------------------------------
bool SaXManipulateLayout::setRelative (
	int relativeTo,int originForScreen, int x, int y
) {
	// .../
	//! These give the screen's location relative to another screen.
	//! When positioning  to the right or left, the top edges are aligned.
	//! When positioning above or below, the left  edges  are  aligned.
	//! The Relative  form  specifies  the offset of the screen's origin
	//! (upper left  corner)  relative  to  the origin of another screen.
	//! Example:
	//!   
	//!         1024
	//!     |---------- |
	//!     |           | 640
	//! 768 |     0     |-----|
	//!     |           |  1  | 480
	//!     |---------- |-----|
	//! 
	//! 
	//! To achieve this allignment the origin (upper left corner) of
	//! Screen[1] must be set to X=1024 Y=288 RELATIVE to Screen[0]
	//! This means the following function call
	//!
	//!     setRelative ( 0,1,1024,288 )
	//!
	// ----
	SaXManipulateCard cardData (mCard); 
	if (! cardData.selectCard (relativeTo)) {
		return false;
	}
	QString screenID; screenID.sprintf ("%d",relativeTo);
	QString origin; origin.sprintf ("Screen[%d],%d,%d",originForScreen,x,y);
	mLayout -> setItem (
		QString("Relative:Screen["+ screenID + "]"),origin
	);
	return true;	
}

//====================================
// removeRelative
//------------------------------------
bool SaXManipulateLayout::removeRelative ( int screen ) {
	// .../
	//! remove the Relative position information from the
	//! ServerLayout section if there is such an information
	// ----
	SaXManipulateCard cardData (mCard);
	if (! cardData.selectCard (screen)) {
		return false;
	}
	QString remove;
	remove.sprintf ("Relative:Screen[%d]",screen);
	mLayout -> removeEntry ( remove );
	return true;
}

//====================================
// setLayout
//------------------------------------
bool SaXManipulateLayout::setXOrgLayout (
	int screen, int left, int right, int top, int bottom
) {
	// .../
	//! set the layout for the given screen (screen). The parameters
	//! define the neighbour IDs at the positions left , right , top
	//! and bottom. If there is no neighbour at position X a negative
	//! value must be set
	// ----
	SaXManipulateCard cardData (mCard);
	if (! cardData.selectCard (screen)) {
		return false;
	}
	QString leftID   ("<none>");
	QString rightID  ("<none>");
	QString topID    ("<none>");
	QString bottomID ("<none>");
	if (left >= 0) {
		leftID.sprintf ("Screen[%d]",left);
	}
	if (right >= 0) {
		rightID.sprintf ("Screen[%d]",right);
	}
	if (top >= 0) {
		topID.sprintf ("Screen[%d]",top);
	}
	if (bottom >= 0) {
		bottomID.sprintf ("Screen[%d]",bottom);
	}
	QString screenID; screenID.sprintf ("%d",screen);
	mLayout -> setItem (
		QString("Screen:Screen["+ screenID + "]"),
		QString(leftID + " " + rightID + " " + topID + " " + bottomID)
	);
	return true;
}

//====================================
// getMultiheadMode
//------------------------------------
int SaXManipulateLayout::getMultiheadMode ( void ) {
	// .../
	//! return an identifier for the currently used multihead
	//! mode. Currently there are the following modes:
	//! SAX_TWINVIEW,SAX_TRADITIONAL,SAX_SINGLE_HEAD,SAX_CLONE
	//! and SAX_XINERAMA. The return of SAX_UNKNOWN_MODE should
	//! never happen
	// ----
	QString clone = mLayout -> getItem ("Clone");
	QString xrama = mLayout -> getItem ("Xinerama");
	//====================================
	// check for merged framebuffer...
	//------------------------------------
	if (mCard->getCount() == 1) {
		SaXManipulateCard cardData (mCard);
		for (int card=0;card<mCard->getCount();card++) {
			cardData.selectCard (card);
			Q3Dict<QString> options = cardData.getOptions();
			//====================================
			// nvidia...
			//------------------------------------
			if (options["TwinView"]) {
				if (*options["TwinView"] == "Clone") {
					return SAX_MERGED_FB_CLONE;
				} else {
					return SAX_MERGED_FB;
				}
			}
			//====================================
			// fglrx...
			//------------------------------------
			if (options["DesktopSetup"]) {
				if (*options["DesktopSetup"] == "Clone") {
					return SAX_MERGED_FB_CLONE;
				} else {
					return SAX_MERGED_FB;
				}
			}
			//====================================
			// intel...
			//------------------------------------
			if (options["Clone"]) {
				return SAX_MERGED_FB_CLONE;
			}
			//====================================
			// matrox / radeon...
			//------------------------------------
			if (options["MergedFB"]) {
				if (
					(*options["Monitor2Position"] == "Clone") ||
					(*options["CRT2Position"] == "Clone")
				) {
					return SAX_MERGED_FB_CLONE;
				} else {
					return SAX_MERGED_FB;
				}
			}
		}
	}
	//====================================
	// check for traditional mode...
	//------------------------------------
	if ((clone == "off") && (xrama == "off") && (mCard->getCount() > 1)) {
		return SAX_TRADITIONAL;
	}
	//====================================
	// check for singlehead mode...
	//------------------------------------
	if ((clone == "off") && (xrama == "off")) {
		return SAX_SINGLE_HEAD;
	}
	//====================================
	// check for clone mode...
	//------------------------------------
	if (clone == "on") {
		return SAX_CLONE;
	}
	//====================================
	// check for Xinerama mode...
	//------------------------------------
	if (xrama == "on") {
		return SAX_XINERAMA;
	}
	return SAX_UNKNOWN_MODE;
}

//====================================
// getLayout
//------------------------------------
QList<QString> SaXManipulateLayout::getXOrgLayout ( int screen ) {
	// .../
	//! return the layout for the given screen (screen). The
	//! returned list has four elements which defines the
	//! (left | right | top | bottom) neighbours of the requested
	//! screen
	// ----
	SaXManipulateCard cardData (mCard);
	if (! cardData.selectCard (screen)) {
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QString idString; idString.sprintf ("%d",screen);
	QString layout = mLayout -> getItem (
		QString("Screen:Screen["+ idString + "]")
	);
	if (layout.isEmpty()) {
		excGetScreenLayoutFailed (screen);
		qError (errorString(),EXC_GETSCREENLAYOUTFAILED);
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> layoutList;
	QStringList positions = QStringList::split ( " ", layout );
	for ( QStringList::Iterator in = positions.begin(); in != positions.end(); ++in	) {
		layoutList.append (*in);
	}
	return layoutList;
}

//====================================
// getInputLayout
//------------------------------------
QList<QString> SaXManipulateLayout::getInputLayout ( void ) {
	// .../
	//! return the layout of the pointer devices. The list
	//! contains the Identifier numbers of the activated devices
	// ----
	QString layout = mLayout -> getItem ("InputDevice");
	if (layout.isEmpty()) {
		excGetInputLayoutFailed ();
		qError (errorString(),EXC_GETINPUTLAYOUTFAILED);
		QList<QString>* nope = new QList<QString>;
		return *nope;
	}
	QList<QString> inputList;
	QStringList tokens = QStringList::split ( ",", layout );
	for ( QStringList::Iterator in = tokens.begin(); in != tokens.end(); ++in ) {
		QRegExp identifier ("\\[(.+)\\]");
		int pos = identifier.search (*in);
		if (pos >= 0) {
			inputList.append (identifier.cap(1));
		}
	}
	return inputList;
}

//====================================
// addInputLayout
//------------------------------------
void SaXManipulateLayout::addInputLayout ( int id ) {
	// .../
	//! add the given pointer ID to the InputDevice option of
	//! the layout section. 
	// ----
	QString mouseID;
	QString layout = mLayout -> getItem ("InputDevice");
	QTextOStream (&mouseID) << ",Mouse[" << id << "]";
	layout.append (mouseID);
	layout.replace (QRegExp("^,"),"");
	mLayout -> setItem (
		"InputDevice",layout
	);
}

//====================================
// removeInputLayout
//------------------------------------
void SaXManipulateLayout::removeInputLayout ( int id ) {
	// .../
	//! remove the given pointer ID from the InputDevice
	//! option of the layout section
	// ----
	QString mouseID;
	QString result ("");
	QString layout = mLayout -> getItem ("InputDevice");
	QTextOStream (&mouseID) << "Mouse[" << id << "]";
	QStringList tokens = QStringList::split ( ",", layout );
	for ( QStringList::Iterator
		in = tokens.begin(); in != tokens.end(); ++in
	) {
		QString item = *in;
		if ( item != mouseID ) {
			result.append (","+item);
		}
	}
	result.replace (QRegExp("^,"),"");
	mLayout -> setItem (
		"InputDevice",result
	);
}
} // end namespace
