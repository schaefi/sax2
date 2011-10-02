/**************
FILE          : mouse.cpp
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
              : - mouse.h: SaXGUI::SCCMouse header definitions
              : - mouse.cpp: configure mouse system
              : ----
              :
STATUS        : Status: Development
**************/

#include <QListIterator>

#include "mouse.h"

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMouse::SCCMouse (
	Q3WidgetStack* stack,Q3Dict<QString>* text,
	Q3Dict<SaXImport> section, QWidget* parent
) : SCCDialog ( stack,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// check for mouse devices
	//-------------------------------------
	mMouse = 0;
	SaXManipulateMice saxMouse ( mSection["Pointers"] );
	saxMouse.selectPointer (SAX_CORE_POINTER);
	for (int i=SAX_CORE_POINTER;i<mSection["Pointers"]->getCount();i+=2) {
		if (saxMouse.selectPointer (i)) {
		if (saxMouse.isMouse()) {
			mMouse++;
		}
		}
	}
	//=====================================
	// create mouse dialog
	//-------------------------------------
	mMouseTab = new QTabWidget ( mDialogFrame );

	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mDialogLayout -> addWidget ( mMouseTab );

	//=====================================
	// create basic mouse display tabs
	//-------------------------------------
	int mouseID = SAX_CORE_POINTER;
	for (int n=0;n<mMouse;n++) {
		QString displayName;
		QString tabNumber;
		QString translation = mText["MouseTab"];
		QTextOStream (&tabNumber) << (n + 1);
		translation.replace ( QRegExp("\%1"), tabNumber);
		QTextOStream (&displayName) << translation;
		SCCMouseDisplay* mouse = new SCCMouseDisplay (
			text,section,mouseID,this
		);
		mMouseTab -> addTab ( mouse, displayName );
		mouse -> init();
		mouse -> import();
		//=====================================
		// connect mouse activate checkbox
		//-------------------------------------
		QObject::connect (
			mouse , SIGNAL (sigActivate  ( void )),
			this  , SLOT   (slotActivate ( void ))
		);
		mMouseDisplay.append ( mouse );
		mouseID += 2;
	}
}
//====================================
// init
//------------------------------------
void SCCMouse::init ( void ) {
	//====================================
	// set init state
	//------------------------------------
	needInitData = false;
}
//====================================
// import
//------------------------------------
void SCCMouse::import ( void ) {
	//====================================
	// create manipulators
	//------------------------------------
	SaXManipulateLayout saxLayout (
		mSection["Layout"],mSection["Card"]
	);
	//====================================
	// handle mouseDisplay's state
	//------------------------------------	
	int mouseCT = 0;
	int mouseID = SAX_CORE_POINTER;
	SaXManipulateMice saxMouse ( mSection["Pointers"] );
	QList<QString> inputLayout = saxLayout.getInputLayout();
	for (int i=mouseID;i<mSection["Pointers"]->getCount();i+=2) {
		if (saxMouse.selectPointer (mouseID)) {
		if (saxMouse.isMouse()) {
			bool foundID = false;
			QString it;
			foreach (it,inputLayout) {
			if (mouseID == it.toInt()) {
				foundID = true;
				break;
			}
			}
			if (! foundID) {
				mMouseDisplay.at(mouseCT)->setEnabled ( false );
			}
		}
		}
		mouseID += 2;
		mouseCT++;
	}
	//====================================
	// set import state
	//------------------------------------
	needImportData = false;
}
//====================================
// slotActivate
//------------------------------------
void SCCMouse::slotActivate ( void ) {
	int count = 0;
	SCCMouseDisplay* it;
	foreach (it,mMouseDisplay) {
	if (it->isEnabled()) {
		count++;
	}
	}
	if ( count == 0) {
		SCCMessage* mMessageBox = new SCCMessage (
			qApp->mainWidget(), mTextPtr, SaXMessage::OK, "PageFault",
			"MessageCaption", SaXMessage::Warning
		);
		SCCMouseDisplay* current; 
		current = (SCCMouseDisplay*)mMouseTab->currentPage();
		mMessageBox -> showMessage();
		current -> setEnabled();
	}
	init();
}
//====================================
// exportData
//------------------------------------
bool SCCMouse::exportData ( void ) {
	int mouseID = 0;
	SCCMouseDisplay* it;
	foreach (it,mMouseDisplay) {
		//====================================
		// check for mouse display's
		//------------------------------------
		SCCMouseDisplay* display = it;
		//====================================
		// create manipulators...
		//------------------------------------
		SaXManipulateMice saxMouse (
			mSection["Pointers"]
		);
		SaXManipulateLayout saxLayout (
			mSection["Layout"],mSection["Card"]
		);
		//====================================
		// select pointer device
		//------------------------------------
		mouseID = display->getDisplay();
		saxMouse.selectPointer ( mouseID );

		//====================================
		// save mouse selection data
		//------------------------------------
		SCCMouseModel* modelData = display->getModelData();
		QString vendor = modelData -> getVendorName(); 
		QString model  = modelData -> getModelName();
		QString zmap;
		if (! model.isEmpty() ) {
			Q3Dict<QString> mData = mSection["Pointers"]->getCurrentTable();
			Q3DictIterator<QString> it (mData);
			for (; it.current() ; ++it) {
				QString key = it.currentKey();
				QString val = *it.current();
				if (key == "Identifier") {
					continue;
				}
				if (key == "ZAxisMapping") {
					zmap = val;
				}
				//mSection["Pointers"]->removeEntry ( key );
			}
			saxMouse.setMouse ( vendor,model );
		}
		//====================================
		// save 3-Button emulation data
		//------------------------------------
		saxMouse.disable3ButtonEmulation();
		if (display->isButtonEmulationEnabled()) {
			saxMouse.enable3ButtonEmulation();
		}
		//====================================
		// save mouse wheel data
		//------------------------------------
		saxMouse.disableWheel();
		if (display->isWheelEnabled()) {
			if (! zmap.isEmpty()) {
				QStringList tokens = QStringList::split ( " ",zmap );
				int b1 = tokens.first().toInt();
				int b2 = tokens.last().toInt();
				saxMouse.enableWheelOn (b1,b2);
			} else {
				saxMouse.enableWheel();
			}
		}
		//====================================
		// save mouse X axis invertation data
		//------------------------------------
		saxMouse.disableXAxisInvertation();
		if (display->isXInverted()) {
			saxMouse.enableXAxisInvertation();
		}
		//====================================
		// save mouse Y axis invertation data
		//------------------------------------
		saxMouse.disableYAxisInvertation();
		if (display->isYInverted()) {
			saxMouse.enableYAxisInvertation();
		}
		//====================================
		// save mouse wheel emulation data
		//------------------------------------
		saxMouse.disableWheelEmulation();
		if (display->isWheelEmulationEnabled()) {
			saxMouse.enableWheelEmulation (display->getWheelButton());
		}
		//====================================
		// save mouse left hand mapping data
		//------------------------------------
		saxMouse.disableLeftHandMapping();
		if (display->isLeftHanded()) {
			saxMouse.enableLeftHandMapping();
		}
		//====================================
		// save input layout
		//------------------------------------
		saxLayout.removeInputLayout (mouseID);
		if (display->isEnabled()) {
			saxLayout.addInputLayout (mouseID);
		}
	}
	return true;
}
} // end namespace
