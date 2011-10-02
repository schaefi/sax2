/**************
FILE          : touchselection.cpp
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
              : - touchselection.h: SaXGUI::SCCTouchSelection implementation
              : - touchselection.cpp: configure touchscreen system
              : ----
              :
STATUS        : Status: Development
**************/
#include "touchselection.h"
//Added by qt3to4:
#include <Q3VBoxLayout>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCTouchSelection::SCCTouchSelection (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section,
	int display, QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create layout for this widget 
	//-------------------------------------
	mMainLayout = new Q3VBoxLayout ( this );

	//=====================================
	// create macro widgets
	//-------------------------------------
	QString displayNr;
	QTextOStream (&displayNr) << (display + 1);
	QString translation = mText["AssignToucher"];
	translation.replace ( QRegExp("\%1"),displayNr );
	mCheckEnable = new QCheckBox ( translation,this );
	mModelVendorGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["TouchSelection"],this
	);
	mVendorList = new Q3ListBox ( mModelVendorGroup );
	mModelList  = new Q3ListBox ( mModelVendorGroup );
	mPortGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["ConnectionPort"],this
	);
	mPortBox = new QComboBox ( mPortGroup );

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mCheckEnable , SIGNAL (clicked             ( void )),
		this         , SLOT   (slotActivateToucher ( void ))
	);
	QObject::connect (
		mVendorList  , SIGNAL (selectionChanged ( Q3ListBoxItem* )),
		this         , SLOT   (slotVendor       ( Q3ListBoxItem* ))
	);
	QObject::connect (
		mModelList   , SIGNAL (selectionChanged ( Q3ListBoxItem* )),
		this         , SLOT   (slotName         ( Q3ListBoxItem* ))
	);
	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mMainLayout -> setMargin  ( 15 );
	mMainLayout -> addWidget  ( mCheckEnable );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mModelVendorGroup );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mPortGroup ); 

	//=====================================
	// save display
	//-------------------------------------
	mDisplay = display;
}
//====================================
// init
//------------------------------------
void SCCTouchSelection::init ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// create manipulators... 
	//------------------------------------   
	mSaxToucher = new SaXManipulateTouchscreens (
		mSection["Pointers"]
	);
	//====================================
	// insert CDB touchscreen vendors
	//------------------------------------
	mCDBTouchScreenVendors = mSaxToucher->getPanelVendorList();
	QString it;
	foreach (it,mCDBTouchScreenVendors) {
		mVendorList -> insertItem (it);
	}
	mVendorList -> sort();

	//=====================================
	// insert available port device names
	//-------------------------------------
	mPortBox -> insertItem ( mText["USB"] );
	mPortBox -> insertItem ( mText["Serial1"] );
	mPortBox -> insertItem ( mText["Serial2"] );
	mPortBox -> insertItem ( mText["Serial3"] );
	mPortBox -> insertItem ( mText["Serial4"] );
	mPortBox -> insertItem ( mText["Serial5"] );

	//====================================
	// update widget status...
	//------------------------------------
	slotActivateToucher();
}
//====================================
// import
//------------------------------------
void SCCTouchSelection::import ( void ) {
	//=====================================
	// search touchscreen ID's
	//-------------------------------------
	int n = 0;
	int x = 0;
	int touchID[mSection["Pointers"]->getCount()];
	for (int i=SAX_CORE_POINTER;i<mSection["Pointers"]->getCount();i+=2) {
		touchID[x] = 0;
		if (mSaxToucher->selectPointer (i)) {
		if (mSaxToucher->isTouchpanel()) {
			touchID[n] = i;
			n++;
		}
		}
		x++;
	}
	//=====================================
	// no touchers found... return
	//-------------------------------------
	if (touchID[mDisplay] == 0) {
		return;
	}
	//=====================================
	// select touchscreen
	//-------------------------------------
	mSaxToucher -> selectPointer ( touchID[mDisplay] );

	//=====================================
	// select vendor and name
	//-------------------------------------
	QString vendorName = mSaxToucher -> getVendor();
	QString modelName  = mSaxToucher -> getName(); 
	if ((! vendorName.isEmpty()) && (! modelName.isEmpty())) {
		Q3ListBoxItem* vendor = mVendorList -> findItem ( vendorName );
		if ( vendor ) {
			mVendorList -> setSelected ( vendor, true );
			slotVendor ( vendor );
			Q3ListBoxItem* name = mModelList -> findItem ( modelName );
			if ( name ) {
				mModelList -> setSelected ( name, true );
				slotName ( name );
			}
		}
	}
	//=====================================
	// setup port
	//-------------------------------------
	QString device = mSaxToucher -> getDevice();

	QRegExp identifier ("/dev/input/(elo|evtouch_event|mice|touchscreen)");
	if (identifier.search (device) >= 0) {
		mPortBox -> setCurrentItem ( 0 );
		mPortBox -> setCurrentText (device);
	}
	if (device == "/dev/ttyS0") {
		mPortBox -> setCurrentItem ( 1 );
	}
	if (device == "/dev/ttyS1") {
		mPortBox -> setCurrentItem ( 2 );
	}
	if (device == "/dev/ttyS2") {
		mPortBox -> setCurrentItem ( 3 );
	}
	if (device == "/dev/ttyS3") {
		mPortBox -> setCurrentItem ( 4 );
	} 
	if (device == "/dev/ttyS4") {
		mPortBox -> setCurrentItem ( 5 );
	} 

	//=====================================
	// setup touchscreen display state
	//-------------------------------------
	SaXManipulateLayout saxLayout ( mSection["Layout"],mSection["Card"] );
	QList<QString> inputLayout = saxLayout.getInputLayout();
	QString it;
	bool foundID = false;
	foreach (it,inputLayout) {
		if (touchID[mDisplay] == it.toInt()) {
			foundID = true;
			break;
		}
	}
	if ( foundID ) {
		mCheckEnable -> setChecked ( true );
		slotActivateToucher();
	}
}
//====================================
// slotActivateToucher
//------------------------------------
void SCCTouchSelection::slotActivateToucher ( void ) {
	if ( mCheckEnable -> isChecked()) {
		mModelVendorGroup -> setDisabled ( false );
		mPortGroup -> setDisabled ( false );
		mEnabled = true;
	} else {
		mModelVendorGroup -> setDisabled ( true );
		mPortGroup -> setDisabled ( true );
		mEnabled = false;
	}
}
//====================================
// slotVendor
//------------------------------------
void SCCTouchSelection::slotVendor ( Q3ListBoxItem* item ) {
	if (! mSaxToucher ) {
		return;
	}
	mModelList -> clear();
	mCDBTouchScreenModels = mSaxToucher->getPanelModelList (
		item->text()
	);
	QString it;
	foreach (it,mCDBTouchScreenModels) {
		mModelList -> insertItem (it);
	}
	mModelList -> sort();
}
//====================================
// slotName
//------------------------------------
void SCCTouchSelection::slotName ( Q3ListBoxItem* item ) {
	if (! mSaxToucher ) {
		return;
	}
	Q3Dict<QString> dataDict = mSaxToucher->getPanelData (
		mVendorList->currentText(),item->text()
	);
	if (dataDict["Device"]) {
		QString device = *dataDict["Device"];
		QRegExp identifier ("/dev/input/(elo|evtouch_event|mice|touchscreen)");
		if (identifier.search (device) >= 0) {
			mPortBox -> setCurrentItem ( 0 );
			mPortBox -> setCurrentText (device);
		}
		if (device == "/dev/ttyS0") {
			mPortBox -> setCurrentItem ( 1 );
		}
		if (device == "/dev/ttyS1") {
			mPortBox -> setCurrentItem ( 2 );
		}
		if (device == "/dev/ttyS2") {
			mPortBox -> setCurrentItem ( 3 );
		}
		if (device == "/dev/ttyS3") {
			mPortBox -> setCurrentItem ( 4 );
		} 
		if (device == "/dev/ttyS4") {
			mPortBox -> setCurrentItem ( 5 );
		} 
	}
}
//====================================
// getVendor
//------------------------------------
QString SCCTouchSelection::getVendor ( void ) {
	if (mVendorList->selectedItem()) {
		return mVendorList->selectedItem()->text();
	}
	QString* nope = new QString;
	return *nope;
}
//====================================
// getModel
//------------------------------------
QString SCCTouchSelection::getModel ( void ) {
	if (mModelList->selectedItem()) {
		return mModelList->selectedItem()->text();
	}
	QString* nope = new QString;
	return *nope;
}
//====================================
// getPortName
//------------------------------------
QString SCCTouchSelection::getPortName ( void ) {
	return mPortBox->currentText();
}
//====================================
// isEnabled
//------------------------------------
bool SCCTouchSelection::isEnabled ( void ) {
	return mEnabled;
}
} // end namespace
