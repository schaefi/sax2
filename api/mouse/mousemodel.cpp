/**************
FILE          : mousemodel.cpp
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
              : - mousemodel.h: SaXGUI::SCCMouseModel implementation
              : - mousemodel.cpp: configure mouse model
              : ----
              :
STATUS        : Status: Development
**************/
#include "mousemodel.h"
#include <QListIterator>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMouseModel::SCCMouseModel (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section, const QString& title,
	int display, QWidget* parent
) : SCCDialog ( 0,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
    
	//=====================================
	// create layout for this widget
	//-------------------------------------
	mModelTab  = new Q3VBox ( this );
   
	//=====================================
	// prepare tabed dialog
	//-------------------------------------
	mModelDialog = new Q3TabDialog  ( this,0,true );
	mModelDialog -> setCaption  ( mText["MouseCaption"] );
	mModelDialog -> setOkButton ( mText["Ok"] );
	mModelDialog -> setCancelButton ( mText["Cancel"] );
	mModelDialog -> addTab ( mModelTab, title );

	//=====================================
	// set import flag
	//-------------------------------------
	mSaxMouse    = 0;
	mNeedImport  = true;
	mOptionState = true;
	mDisplay = display;

	//=====================================
	// create macro widgets [Model]
	//-------------------------------------
	mModelTab -> setMargin ( 20 );
	mModelVendorGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["MouseSelection"],mModelTab
	);
	mVendorList = new Q3ListBox ( mModelVendorGroup );
	mModelList  = new Q3ListBox ( mModelVendorGroup );

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mModelDialog , SIGNAL (applyButtonPressed ( void )),
		this         , SLOT   (slotOk ( void ))
	);
	QObject::connect (
		mVendorList  , SIGNAL (selectionChanged (Q3ListBoxItem *)),
		this         , SLOT   (slotVendor       (Q3ListBoxItem *))
	);
}
//=====================================
// show tabed dialog
//-------------------------------------
void SCCMouseModel::show (void) {
	mModelDialog->setGeometry  ( 0,0,640,480 );
	mModelDialog->move (
		getPosition (mModelDialog->width(),mModelDialog->height())
	);
	import();
	mModelDialog->show();
	mModelDialog->raise();
}
//====================================
// init
//------------------------------------
void SCCMouseModel::init ( void ) {
	//====================================
	// create manipulators... 
	//------------------------------------   
	mSaxMouse = new SaXManipulateMice (
		mSection["Pointers"]
	);
	//====================================
	// select pointer
	//------------------------------------
	mSaxMouse -> selectPointer ( mDisplay );

	//====================================
	// insert CDB mice
	//------------------------------------
	mCDBMouseVendors = mSaxMouse->getMouseVendorList();
	QString it;
	foreach (it, mCDBMouseVendors){
		mVendorList -> insertItem(it);
	}
	mVendorList -> sort();
}
//====================================
// import
//------------------------------------
void SCCMouseModel::import ( void ) {
	if ( mNeedImport ) {
		//=====================================
		// check manipulator access
		//-------------------------------------
		if (! mSaxMouse ) {
			return;
		}
		//====================================
		// handle mouse vendor/model name
		//------------------------------------
		QString mouseModel  = mSection["Pointers"]->getItem("Name");
		QString mouseVendor = mSection["Pointers"]->getItem("Vendor");
		if (mouseVendor != "Sysp") {
			mouseModel=mouseVendor+";"+mouseModel;
		}
		if (! mouseModel.isEmpty()) {
			QStringList vnlist = QStringList::split ( ";", mouseModel );
			if (vnlist.count() == 2) {
				mSelectedMouseVendor = vnlist.first();
				mSelectedMouseName = vnlist.last();
			}
		}
		//====================================
		// import done set flag
		//------------------------------------
		mNeedImport = false;
	}
	//====================================
	// setup vendor name listboxes
	//------------------------------------
	Q3ListBoxItem* vendor = mVendorList -> findItem ( mSelectedMouseVendor );
	if ( vendor ) {
		mVendorList -> setSelected ( vendor, true );
		slotVendor ( vendor );
		Q3ListBoxItem* name = mModelList -> findItem ( mSelectedMouseName );
		if ( name ) {
			mModelList -> setSelected ( name, true );
		}
	}
}
//====================================
// getVendorName
//------------------------------------
QString SCCMouseModel::getVendorName ( void ) {
	return mSelectedMouseVendor;
}
//====================================
// getModelName
//------------------------------------
QString SCCMouseModel::getModelName ( void ) {
	return mSelectedMouseName;
}
//====================================
// getOptionState
//------------------------------------
bool SCCMouseModel::getOptionState ( void ) {
	return mOptionState;
}
//====================================
// slotVendor
//------------------------------------
void SCCMouseModel::slotVendor ( Q3ListBoxItem* item ) {
	if (! mSaxMouse ) {
		return;
	}
	mModelList -> clear();
	mCDBMouseModels = mSaxMouse->getMouseModelList (
		item->text()
	);
	QString it;
	foreach (it,mCDBMouseModels) {
		mModelList -> insertItem (it);
	}
	mModelList -> sort();
		
}
//====================================
// slotOk
//------------------------------------
void SCCMouseModel::slotOk ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// get parent SCCMouseDisplay ptr
	//-------------------------------------
	SCCMouseDisplay* pDisplay = (SCCMouseDisplay*)mParent;

	//=====================================
	// save data to dialog export variables
	//-------------------------------------
	if ((! mVendorList->selectedItem()) || (! mModelList->selectedItem())) {
		return;
	}
	mSelectedMouseVendor = mVendorList -> selectedItem()->text();
	mSelectedMouseName   = mModelList  -> selectedItem()->text();

	//=====================================
	// check parents options
	//-------------------------------------
	Q3Dict<QString> selectedData = mSaxMouse->getMouseData (
		mSelectedMouseVendor,mSelectedMouseName
	);
	pDisplay -> setWheelEmulationEnabled  ( false );
	pDisplay -> setButtonEmulationEnabled ( false );
	pDisplay -> setWheelEnabled ( false );
	Q3DictIterator<QString> it (selectedData);
	for (; it.current(); ++it) {
		QString key = it.currentKey();
		QString val = *it.current();
		if (key == "EmulateWheel") {
			pDisplay -> setWheelEmulationEnabled ( true );
			pDisplay -> setWheelButton ( val.toInt() );
		}
		if (key == "ZAxisMapping") {
			pDisplay -> setWheelEnabled ( true );
		}
		if (key == "Emulate3Buttons") {
		if (val == "on") {
			pDisplay -> setButtonEmulationEnabled ( true );
		}
		}
	}
	//=====================================
	// check availability of options box
	//-------------------------------------
	mOptionState = true;
	pDisplay -> setMouseOptionState ( true );
	Q3Dict<QString> mouseData = mSaxMouse -> getMouseData (
		mSelectedMouseVendor,mSelectedMouseName
	);
	if (mouseData["Driver"]) {
	if (*mouseData["Driver"] == "synaptics") {
		pDisplay -> setMouseOptionState ( false );
		mOptionState = false;
	}
	}
	//=====================================
	// update parent mouse name label
	//-------------------------------------
	QString mouseName;
	QTextOStream (&mouseName) <<
		mText["Mouse"]<< ": " <<
		mSelectedMouseVendor << " " << mSelectedMouseName;
	pDisplay -> setMouseName ( mouseName );
}
} // end namespace
