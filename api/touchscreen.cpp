/**************
FILE          : touchscreen.cpp
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
#include "touchscreen.h"

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCTouchScreen::SCCTouchScreen (
	Q3WidgetStack* stack,Q3Dict<QString>* text,
	Q3Dict<SaXImport> section, QWidget* parent
) : SCCDialog ( stack,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// check for graphics cards
	//-------------------------------------
	SaXManipulateCard saxCard ( mSection["Card"] );
	mCard = saxCard.getDevices();

	//=====================================
	// create touchscreen dialog
	//-------------------------------------
	mTouchscreenTab = new QTabWidget  ( mDialogFrame );

	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mDialogLayout -> addWidget  ( mTouchscreenTab );

	//=====================================
	// create basic touchscreen tab
	//-------------------------------------
	for ( int n=0;n<mCard;n++) {
		QString toucherName;
		QString tabNumber;
		QString translation = mText["Touchscreen"];
		QTextOStream (&tabNumber) << (n + 1);
		translation.replace ( QRegExp("\%1"), tabNumber);
		QTextOStream (&toucherName) << translation;
		SCCTouchSelection* toucher = new SCCTouchSelection  (
			text,section,n,this
		);
		mTouchDisplay.append ( toucher );
		mTouchscreenTab -> addTab (
			toucher, toucherName
		);
		toucher -> init();
		toucher -> import();
	}
}
//=====================================
// init
//-------------------------------------
void SCCTouchScreen::init ( void ) {
	//====================================
	// set init state
	//------------------------------------
	needInitData = false;
}
//=====================================
// import
//-------------------------------------
void SCCTouchScreen::import ( void ) {
	//====================================
	// set import state
	//------------------------------------
	needImportData = false;
}
//=====================================
// exportData
//-------------------------------------
bool SCCTouchScreen::exportData ( void ) {
	//====================================
	// create manipulators... 
	//------------------------------------
	SaXManipulateTouchscreens saxToucher (
		mSection["Pointers"]
	);
	SaXManipulateDevices saxDevice (
		mSection["Pointers"],mSection["Layout"]
	);
	//====================================
	// search and remove touchpanel(s)
	//------------------------------------
	int inputCount = mSection["Pointers"]->getCount();
	for (int i=inputCount;i>=0;i--) {
		if (saxToucher.selectPointer (i)) {
		if (saxToucher.isTouchpanel()) {
			saxDevice.removeInputDevice (i);
		}
		}
	}
	//====================================
	// add touchpanel if enabled
	//------------------------------------
	SCCTouchSelection* it;
	foreach (it,mTouchDisplay) {
		SCCTouchSelection* toucher = it;
		if (toucher->isEnabled()) {
			QString vendor = toucher->getVendor();
			QString model  = toucher->getModel();
			if (model.isEmpty()) {
				continue;
			}
			int touchID = saxDevice.addInputDevice (SAX_INPUT_TOUCHPANEL);
			saxToucher.selectPointer ( touchID );
			saxToucher.setTouchPanel ( vendor,model );
			Q3Dict<QString> touchDict = saxToucher.getPanelData (
                                vendor,model
                        );
			//====================================
			// save touchpanel connection port
			//------------------------------------
			QString port = toucher->getPortName();
			if (port.contains ("ttyS0")) {
				saxToucher.setDevice ( "/dev/ttyS0" );
			}
			if (port.contains ("ttyS1")) {
				saxToucher.setDevice ( "/dev/ttyS1" );
			}
			if (port.contains ("ttyS2")) {
				saxToucher.setDevice ( "/dev/ttyS2" );
			}
			if (port.contains ("ttyS3")) {
				saxToucher.setDevice ( "/dev/ttyS3" );
			}
			if (port.contains ("ttyS4")) {
				saxToucher.setDevice ( "/dev/ttyS4" );
			}
			if (port.contains ("USB")) {
				QString _device = *touchDict["Device"];
				if (! _device.isEmpty()) {
					saxToucher.setDevice ( _device );
				} else {
					saxToucher.setDevice ( "/dev/input/mice" );
				}
			}			
		}
	}
	return true;
}
} // end namespace
