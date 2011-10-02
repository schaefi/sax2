/**************
FILE          : tabletpens.cpp
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
              : - tabletpens.h: SaXGUI::SCCTabletPens implementation
              : - tabletpens.cpp: configure tablet system
              : ----
              :
STATUS        : Status: Development
**************/
#include "tabletpens.h"
//Added by qt3to4:
#include <Q3VBoxLayout>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCTabletPens::SCCTabletPens (
	Q3Dict<QString>* text, Q3Dict<SaXImport> section,
	int, QWidget* parent
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
	mToolGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["TabletTools"],this
	);
	mPenBox    = new Q3HBox ( mToolGroup );
	mCheckPen = new QCheckBox (
		mText["AddPen"],mPenBox
	);
	mPenProperties = new QPushButton (
		mText["Properties"],mPenBox
	);
	mPenBox -> setStretchFactor ( mCheckPen,10 );
	mEraserBox = new Q3HBox ( mToolGroup );
	mCheckEraser = new QCheckBox (
		mText["AddEraser"],mEraserBox
	);
	mEraserProperties = new QPushButton (
		mText["Properties"],mEraserBox
	);
	mEraserBox -> setStretchFactor ( mCheckEraser,10 );
	mTouchBox = new Q3HBox ( mToolGroup );
	mCheckTouch = new QCheckBox (
		mText["AddTouch"],mTouchBox
	);
	mTouchProperties = new QPushButton (
		mText["Properties"],mTouchBox
	);
	mTouchBox -> setStretchFactor ( mCheckTouch,10 );
	mPadBox    = new Q3HBox ( mToolGroup );
	mCheckPad = new QCheckBox (
		mText["AddPad"],mPadBox
	);
	mPadBox -> setStretchFactor ( mCheckPad,10 );

	//=====================================
	// create toplevel dialogs 
	//-------------------------------------
	mPenPropertyDialog = new SCCTabletPenProperty (
		text,section,mText["PenProperties"],0,this
	);
	mEraserPropertyDialog = new SCCTabletPenProperty (
		text,section,mText["EraserProperties"],0,this
	);
	mTouchPropertyDialog = new SCCTabletPenProperty (
		text,section,mText["TouchProperties"],0,this
	);
	mPadPropertyDialog = new SCCTabletPadProperty (
		text,section,mText["PadProperties"],0,this
	);
	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mPenProperties    , SIGNAL ( clicked            ( void )),
		this              , SLOT   ( slotPen            ( void ))
	);
	QObject::connect (
		mEraserProperties , SIGNAL ( clicked            ( void )),
		this              , SLOT   ( slotEraser         ( void ))
	);
	QObject::connect (
		mTouchProperties  , SIGNAL ( clicked            ( void )),
		this              , SLOT   ( slotTouch          ( void ))
	);
	QObject::connect (
		mCheckPen         , SIGNAL ( clicked            ( void )),
		this              , SLOT   ( slotActivatePen    ( void ))
	);
	QObject::connect (
		mCheckEraser      , SIGNAL ( clicked            ( void )),
		this              , SLOT   ( slotActivateEraser ( void ))
	);
	QObject::connect (
		mCheckTouch       , SIGNAL ( clicked            ( void )),
		this              , SLOT   ( slotActivateTouch  ( void ))
	);
	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mMainLayout -> setMargin  ( 15 );
	mMainLayout -> addWidget  ( mToolGroup );
	mMainLayout -> addStretch ( 10 );
}
//====================================
// init
//------------------------------------
void SCCTabletPens::init ( void ) {
	//====================================
	// create manipulators... 
	//------------------------------------
	mSaxTablet = new SaXManipulateTablets (
		mSection["Pointers"],mSection["Layout"]
	);
	//=====================================
	// search and setup section ID's
	//-------------------------------------
	int penID    = 0;
	int eraserID = 0;
	int touchID  = 0;
	int padID    = 0;
	for (int i=SAX_CORE_POINTER;i<mSection["Pointers"]->getCount();i+=2) {
		if (mSaxTablet->selectPointer (i)) {
		if (mSaxTablet->isPen()) {
			penID = i;
		}
		if (mSaxTablet->isEraser()) {
			eraserID = i;
		}
		if (mSaxTablet->isTouch()) {
			touchID = i;
		}
		if (mSaxTablet->isPad()) {
			padID = i;
		}
		}
	}
	//=====================================
	// initialize toplevel dialogs
	//-------------------------------------
	mPenPropertyDialog    -> setID ( penID );
	mPenPropertyDialog    -> init();
	mEraserPropertyDialog -> setID ( eraserID );
	mEraserPropertyDialog -> init();
	mTouchPropertyDialog  -> setID ( touchID );
	mTouchPropertyDialog  -> init();
	mPadPropertyDialog    -> setID ( padID );

	//====================================
	// setup default pen/eraser buttons
	//------------------------------------
	mPenProperties    -> setDisabled ( true );
	mEraserProperties -> setDisabled ( true );
	mTouchProperties  -> setDisabled ( true );
}
//====================================
// import
//------------------------------------
void SCCTabletPens::import ( void ) {
	//=====================================
	// search tablet ID
	//-------------------------------------
	int tabletID = 0;
	for (int i=SAX_CORE_POINTER;i<mSection["Pointers"]->getCount();i+=2) {
		if (mSaxTablet->selectPointer (i)) {
		if (mSaxTablet->isTablet()) {
			tabletID = i;
		}
		}
	}
	if ( tabletID == 0 ) {
		return;
	}
	//=====================================
	// select tablet
	//-------------------------------------
	mSaxTablet->selectPointer ( tabletID );

	//=====================================
	// select vendor and name
	//-------------------------------------
	SCCTablet* parent = (SCCTablet*)mParent;
	parent -> getSelectionData() -> selectTablet (
		mSaxTablet -> getVendor(),mSaxTablet -> getName()
	);
	//=====================================
	// setup port
	//-------------------------------------
	parent -> getConnectionData() -> setPort (
		mSaxTablet->getDevice()
	);
	//=====================================
	// setup tablet options
	//-------------------------------------
	parent -> getConnectionData() -> setOptions (
		mSaxTablet->getOptions()
	);
	//=====================================
	// setup tablet mode
	//-------------------------------------
	parent -> getConnectionData() -> setMode (
		mSaxTablet->getMode()
	);
	//=====================================
	// setup availability of pen/eraser/pad
	//-------------------------------------
	if (mPenPropertyDialog -> getID() > 0) {
		mCheckPen -> setChecked ( true );
		slotActivatePen();
	}
	if (mEraserPropertyDialog -> getID() > 0) {
		mCheckEraser -> setChecked ( true );
		slotActivateEraser();
	} 
	if (mTouchPropertyDialog -> getID() > 0) {
		mCheckTouch -> setChecked ( true );
		slotActivateTouch();
	} 
	if (mPadPropertyDialog -> getID() > 0) {
		mCheckPad -> setChecked ( true );
	}
	//=====================================
	// call pen/eraser import() methods
	//-------------------------------------
	if (mPenPropertyDialog -> getID() > 0) {
		mPenPropertyDialog -> import();
	}
	if (mEraserPropertyDialog -> getID() > 0) {
		mEraserPropertyDialog -> import();
	}
	if (mTouchPropertyDialog -> getID() > 0) {
		mTouchPropertyDialog -> import();
	}
}
//====================================
// hasPen
//------------------------------------
bool SCCTabletPens::hasPen ( void ) {
	return mCheckPen->isChecked();
}
//====================================
// hasEraser
//------------------------------------
bool SCCTabletPens::hasEraser ( void ) {
	return mCheckEraser->isChecked();
}
//====================================
// hasTouch
//------------------------------------
bool SCCTabletPens::hasTouch ( void ) {
	return mCheckTouch->isChecked();
}
//====================================
// hasPad
//------------------------------------
bool SCCTabletPens::hasPad ( void ) {
	return mCheckPad->isChecked();
}
//====================================
// slotTablet
//------------------------------------
void SCCTabletPens::slotTablet (
	const QString& vendor,const QString& name
) {
	//====================================
	// retrieve data record for tablet
	//------------------------------------
	Q3Dict<QString> tabletDict = mSaxTablet->getTabletData ( vendor,name );

	//====================================
	// check for pen support
	//------------------------------------
	bool hasPen = false;
	mPenBox -> setDisabled ( true );
	if (tabletDict["StylusLink"]) {
		mPenPropertyDialog -> setupPen ( vendor, *tabletDict["StylusLink"] );
		mPenBox -> setDisabled ( false );
		mCheckPen -> setChecked ( false );
		slotActivatePen();
		hasPen = true;
	}
	//====================================
	// check for eraser support
	//------------------------------------
	bool hasEraser = false;
	mEraserBox -> setDisabled ( true );
	if (tabletDict["EraserLink"]) {
		mEraserPropertyDialog -> setupPen ( vendor, *tabletDict["EraserLink"] );
		mEraserBox -> setDisabled ( false );
		mCheckEraser -> setChecked ( false );
		slotActivateEraser();
		hasEraser = true;
	}
	//====================================
	// check for touch support
	//------------------------------------
	bool hasTouch = false;
	mTouchBox -> setDisabled ( true );
	if (tabletDict["TouchLink"]) {
		mTouchPropertyDialog -> setupPen ( vendor, *tabletDict["TouchLink"] );
		mTouchBox -> setDisabled ( false );
		mCheckTouch -> setChecked ( false );
		slotActivateTouch();
		hasTouch = true;
	}
	//====================================
	// check for pad support
	//------------------------------------
	bool hasPad = false;
	mPadBox -> setDisabled ( true );
	if (tabletDict["PadLink"]) {
		mPadBox -> setDisabled ( false );
		mCheckPad -> setChecked ( false );
		hasPad = true;
	}
	//====================================
	// setup state of Pen tab
	//------------------------------------
	SCCTablet* parent = (SCCTablet*)mParent;
	parent -> enablePens ( true );
	if ((! hasPen) && (! hasEraser)) {
		parent->enablePens ( false );
	}
}
//====================================
// slotPen
//------------------------------------
void SCCTabletPens::slotPen ( void ) {
	mPenPropertyDialog -> show();
}
//====================================
// slotEraser
//------------------------------------
void SCCTabletPens::slotEraser ( void ) {
	mEraserPropertyDialog -> show();
}
//====================================
// slotTouch
//------------------------------------
void SCCTabletPens::slotTouch ( void ) {
	mTouchPropertyDialog -> show();
}
//====================================
// slotActivatePen
//------------------------------------
void SCCTabletPens::slotActivatePen ( void ) {
	mPenProperties -> setDisabled ( true );
	if (mCheckPen->isChecked()) {
		mPenProperties -> setDisabled ( false );
	}
}
//====================================
// slotActivateEraser
//------------------------------------
void SCCTabletPens::slotActivateEraser ( void ) {
	mEraserProperties -> setDisabled ( true );
	if (mCheckEraser->isChecked()) {
		mEraserProperties -> setDisabled ( false );
	}
}
//====================================
// slotActivateTouch
//------------------------------------
void SCCTabletPens::slotActivateTouch ( void ) {
	mTouchProperties -> setDisabled ( true );
	if (mCheckTouch->isChecked()) {
		mTouchProperties -> setDisabled ( false );
	}
}
//====================================
// getPenPropertyData
//------------------------------------
SCCTabletPenProperty* SCCTabletPens::getPenPropertyData ( void ) {
	return mPenPropertyDialog;
}
//====================================
// getEraserPropertyData
//------------------------------------
SCCTabletPenProperty* SCCTabletPens::getEraserPropertyData ( void ) {
	return mEraserPropertyDialog;
}
//====================================
// getTouchPropertyData
//------------------------------------
SCCTabletPenProperty* SCCTabletPens::getTouchPropertyData ( void ) {
	return mTouchPropertyDialog;
}
} // end namespace
