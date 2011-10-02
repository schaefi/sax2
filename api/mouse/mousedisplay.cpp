/**************
FILE          : mousedisplay.cpp
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
              : - mousedisplay.h: SaXGUI::SCCMouseDisplay header definitions
              : - mousedisplay.cpp: configure mouse system
              : ----
              :
STATUS        : Status: Development
**************/
#include "mousedisplay.h"
//Added by qt3to4:
#include <QLabel>
#include <Q3VBoxLayout>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCMouseDisplay::SCCMouseDisplay (
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
	// toplevel activate check box
	mCheckEnable = new QCheckBox ( mText["ActivateMouse"],this );
	// first group with mouse name
	mMouseNameGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,"",this
	);
	Q3HBox* mouseBox = new Q3HBox ( mMouseNameGroup );
	mLabelMouseName = new QLabel ( mText["Mouse"],mouseBox );
	mChangeMouse    = new QPushButton ( mText["Change"],mouseBox);
	mouseBox -> setStretchFactor ( mLabelMouseName, 20 );
	// create stack widget for different mouse option dialogs
	mOptionStack    = new Q3WidgetStack ( this );
	// second group with mouse options
	Q3HBox* splitMouseBox = new Q3HBox ( mOptionStack );
	// insert standard mouse option dialog
	Q3VBox* leftMouseBox  = new Q3VBox ( splitMouseBox );
	mMouseOptionGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["MouseOptions"],leftMouseBox
	);
	mCheck3BtnEmulation = new QCheckBox (
		mText["ButtonEmulation"],mMouseOptionGroup
	);
	mCheckMouseWheel    = new QCheckBox (
		mText["ActivateWheel"],mMouseOptionGroup
	);
	mCheckInvertXAxis   = new QCheckBox (
		mText["InvertXAxis"],mMouseOptionGroup
	);
	mCheckInvertYAxis   = new QCheckBox (
		mText["InvertYAxis"],mMouseOptionGroup
	);
	mCheckLeftHand      = new QCheckBox (
		mText["LeftHand"],mMouseOptionGroup
	);
	Q3HBox* wheelBox = new Q3HBox ( mMouseOptionGroup );
	mCheckEmulateWheel = new QCheckBox (
		mText["EmulateWheel"],wheelBox
	);
	wheelBox -> setSpacing ( 10 );
	mEmulateWheelButton = new QSpinBox ( 1,10,1,wheelBox );
	// third group with mouse test field
	splitMouseBox -> setSpacing ( 15 );
	Q3VBox* rightMouseBox = new Q3VBox ( splitMouseBox );
	mMouseTestGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["MouseTest"],rightMouseBox
	);
	mTestField = new SCCMouseTest (
		mTextPtr,mMouseTestGroup
	);
	splitMouseBox -> setStretchFactor ( leftMouseBox, 20 );
	// second group with synaptics options
	Q3HBox* splitSynapBox = new Q3HBox ( mOptionStack );
	// insert standard synaptics option dialog
	Q3VBox* leftSynapticsBox  = new Q3VBox ( splitSynapBox );
	mSynapOptionGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["MouseOptions"],leftSynapticsBox
	);
	// TODO
	// add code for synaptics options dialog here
	// ...
	// third group with synaptics test field
	splitSynapBox -> setSpacing ( 15 );
	Q3VBox* rightSynapBox = new Q3VBox ( splitSynapBox );
	mSynapTestGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["MouseTest"],rightSynapBox
	);
	// TODO
	// add code for synaptics field dialog here
	// ...
	splitSynapBox -> setStretchFactor ( leftSynapticsBox, 20 );
	// add stacked widgets...
	mMouseOptID = mOptionStack -> addWidget ( splitMouseBox );
	mSynapOptID = mOptionStack -> addWidget ( splitSynapBox );

	//=====================================
	// create toplevel dialogs
	//-------------------------------------
	mChangeMouseModelDialog = new SCCMouseModel (
		text,section,mText["SetupMouse"],display,this
	);

	//=====================================
	// initialize toplevel dialogs
	//-------------------------------------
	mChangeMouseModelDialog -> init();
	mChangeMouseModelDialog -> import();

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mChangeMouse , SIGNAL (clicked         ( void )),
		this         , SLOT   (slotChangeMouse ( void ))
	);
	QObject::connect (
		mCheckEnable , SIGNAL (clicked             ( void )),
		this         , SLOT   (slotActivateDisplay ( void ))
	);
	QObject::connect (
		mCheckEmulateWheel , SIGNAL (clicked            ( void )),
		this               , SLOT   (slotWheelEmulation ( void ))
	);
	QObject::connect (
		mCheckLeftHand     , SIGNAL (clicked      ( void )),
		this               , SLOT   (slotLeftHand ( void ))
	);
	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mMainLayout -> setMargin  ( 15 );
	mMainLayout -> addWidget  ( mCheckEnable );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mMouseNameGroup );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mOptionStack );
	mMainLayout -> setStretchFactor ( mOptionStack,20 );

	//=====================================       
	// Show standard mouse options
	//-------------------------------------
	mOptionStack -> raiseWidget ( mMouseOptID );

	//=====================================
	// save display number
	//-------------------------------------
	mPointerID= SAX_CORE_POINTER;
	mDisplay  = display;
	mSaxMouse = 0;
}
//====================================
// init
//------------------------------------
void SCCMouseDisplay::init ( void ) {
	//====================================
	// create manipulators...
	//------------------------------------
	mSaxMouse = new SaXManipulateMice (
		mSection["Pointers"]
	);
	//====================================
	// obtain device count
	//------------------------------------
	int usedDevices  = 0;
	int pointerCount = SAX_CORE_POINTER;
	mSaxMouse -> selectPointer (SAX_CORE_POINTER);
	for (int i=SAX_CORE_POINTER;i<mSection["Pointers"]->getCount();i+=2) {
		if (mSaxMouse -> selectPointer (i)) {
		if (mSaxMouse -> isMouse()) {
			if (pointerCount == mDisplay) {
				mPointerID = i;
			}
			pointerCount+=2;
			usedDevices++;
		}
		}
	}
	//====================================
	// select pointer device
	//------------------------------------
	mSaxMouse -> selectPointer ( mPointerID );

	//====================================
	// check activate box
	//------------------------------------
	mCheckEnable -> setChecked ( true );
	if (usedDevices == 1) {
		mCheckEnable -> hide();
		mEnabled = true;
	} else {
		mCheckEnable -> setChecked ( true );
		mEnabled = true;
	}
	//====================================
	// check availability of options box
	//------------------------------------
	if (mSaxMouse -> getDriver() == "synaptics") {
		mMouseOptionGroup->setDisabled ( true );
	}
	//====================================
	// disable wheel button spin box
	//------------------------------------
	mEmulateWheelButton -> setDisabled ( true );
}
//====================================
// import
//------------------------------------
void SCCMouseDisplay::import ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// check manipulators...
	//------------------------------------
	if (! mSaxMouse ) {
		return;
	}
	//====================================
	// select pointer device
	//------------------------------------
	mSaxMouse -> selectPointer ( mPointerID );

	//====================================
	// handle mouse name
	//------------------------------------
	QString mouseModel  = mSection["Pointers"]->getItem("Name");
	QString mouseVendor = mSection["Pointers"]->getItem("Vendor");
	if (mouseVendor != "Sysp") {
		mouseModel=mouseVendor+";"+mouseModel;
	}
	if (! mouseModel.isEmpty()) {
		mouseModel.replace(QRegExp(";")," ");
		mLabelMouseName -> setText (
			mText["Mouse"] + ": "+ mouseModel
		);
	} else {
		mLabelMouseName -> setText (
			mText["Mouse"] + ": " + mText["UnknownMousePleaseChange"]
		);
	}
	//====================================
	// handle 3 Button emulation
	//------------------------------------
	if (mSaxMouse -> isButtonEmulated()) {
		mCheck3BtnEmulation -> setChecked ( true );
	}
	//====================================
	// handle mouse wheel
	//------------------------------------
	if (mSaxMouse -> isWheelEnabled()) {
		mCheckMouseWheel -> setChecked ( true );
	}
	//====================================
	// handle X axis invertation
	//------------------------------------
	if (mSaxMouse -> isYAxisInverted ()) {
		mCheckInvertYAxis -> setChecked ( true );   
	}
	//====================================
	// handle X axis invertation
	//------------------------------------
	if (mSaxMouse -> isXAxisInverted ()) {
		mCheckInvertXAxis -> setChecked ( true );
	}
	//====================================
	// handle mouse wheel emulation
	//------------------------------------
	if (mSaxMouse -> isWheelEmulated()) {
		mCheckEmulateWheel  -> setChecked  ( true );
		mEmulateWheelButton -> setDisabled ( false );
		mEmulateWheelButton -> setValue ( mSaxMouse->getWheelEmulatedButton() );
	}
	//====================================
	// handle left hand mapping
	//------------------------------------
	if (mSaxMouse -> isLeftHandEnabled()) {
		mCheckLeftHand -> setChecked ( true );
	}
}
//====================================
// slotChangeMouse
//------------------------------------
void SCCMouseDisplay::slotChangeMouse ( void ) {
	mChangeMouseModelDialog -> show();
}
//====================================
// slotActivateDisplay
//------------------------------------
void SCCMouseDisplay::slotActivateDisplay ( void ) {
	if (mCheckEnable -> isChecked()) {
		mMouseNameGroup   -> setDisabled (false);
		mMouseOptionGroup -> setDisabled (false);
		mMouseTestGroup   -> setDisabled (false);
		mEnabled = true;
	} else {
		mMouseNameGroup   -> setDisabled (true);
		mMouseOptionGroup -> setDisabled (true);
		mMouseTestGroup   -> setDisabled (true);
		mEnabled = false;
	}
	if (! mSaxMouse) {
		return;
	}
	//====================================
	// check mouse driver from config
	//------------------------------------
	mSaxMouse -> selectPointer ( mPointerID );
	if (mSaxMouse -> getDriver() == "synaptics") {
		mMouseOptionGroup->setDisabled ( true );
	} else {
		//====================================
		// check mouse driver from V/N dialog
		//------------------------------------
		Q3Dict<QString> mouseData = mSaxMouse -> getMouseData (
			mChangeMouseModelDialog->getVendorName(),
			mChangeMouseModelDialog->getModelName()
		);
		if (mouseData["Driver"]) {
		if (*mouseData["Driver"] == "synaptics") {
			mMouseOptionGroup->setDisabled ( true );
		}
		}
	}
	emit sigActivate();
}
//====================================
// slotWheelEmulation
//------------------------------------
void SCCMouseDisplay::slotWheelEmulation ( void ) {
	if (mCheckEmulateWheel -> isChecked()) {
		mEmulateWheelButton -> setDisabled ( false );
	} else {
		mEmulateWheelButton -> setDisabled ( true );
	}
}
//====================================
// slotLeftHand
//------------------------------------
void SCCMouseDisplay::slotLeftHand ( void ) {
	qApp->setOverrideCursor ( Qt::forbiddenCursor );
	Q3Process* proc = new Q3Process ();
	proc -> addArgument ("xmodmap");
	proc -> addArgument ("-e");
	if (mCheckLeftHand -> isChecked()) {
		proc -> addArgument (
			"pointer = 3 2 1 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20"
		);
	} else {
		proc -> addArgument (
			"pointer = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20"
		);
	}
	if ( ! proc -> start() ) {
		qApp->restoreOverrideCursor();
		return;
	}
	while (proc->isRunning()) {
		usleep (1000);
	}
	qApp->restoreOverrideCursor();
}
//====================================
// isEnabled
//------------------------------------
bool SCCMouseDisplay::isEnabled  ( void ) {
	return mEnabled;
}
//====================================
// setEnabled
//------------------------------------
void SCCMouseDisplay::setEnabled ( bool status ) {
	mCheckEnable -> setChecked ( status );
	slotActivateDisplay();
}
//====================================
// setMouseName
//------------------------------------
void SCCMouseDisplay::setMouseName ( const QString& name ) {
	mLabelMouseName -> clear();
	mLabelMouseName -> setText ( name );
}
//====================================
// setMouseOptionState
//------------------------------------
void SCCMouseDisplay::setMouseOptionState ( bool status ) {
	mMouseOptionGroup->setDisabled ( ! status );
}
//====================================
// isButtonEmulationEnabled
//------------------------------------
bool SCCMouseDisplay::isButtonEmulationEnabled ( void ) {
	return mCheck3BtnEmulation->isChecked();
}
//====================================
// isWheelEmulationEnabled
//------------------------------------
bool SCCMouseDisplay::isWheelEmulationEnabled  ( void ) {
	return mCheckEmulateWheel->isChecked();
}
//====================================
// isWheelEnabled
//------------------------------------
bool SCCMouseDisplay::isWheelEnabled ( void ) {
	return mCheckMouseWheel->isChecked();
}
//====================================
// isXInverted
//------------------------------------
bool SCCMouseDisplay::isXInverted ( void ) {
	return mCheckInvertXAxis->isChecked();
}
//====================================
// isYInverted
//------------------------------------
bool SCCMouseDisplay::isYInverted ( void ) {
	return mCheckInvertYAxis->isChecked();
}
//====================================
// isLeftHanded
//------------------------------------
bool SCCMouseDisplay::isLeftHanded ( void ) {
	return mCheckLeftHand->isChecked();
}
//====================================
// getWheelButton
//------------------------------------
int SCCMouseDisplay::getWheelButton ( void ) {
	return mEmulateWheelButton->value();
}
//====================================
// getModelData
//------------------------------------
SCCMouseModel* SCCMouseDisplay::getModelData ( void ) {
	return mChangeMouseModelDialog;
}
//====================================
// getDisplay
//------------------------------------
int SCCMouseDisplay::getDisplay ( void ) {
	return mDisplay;
}
//====================================
// setButtonEmulationEnabled
//------------------------------------
void SCCMouseDisplay::setButtonEmulationEnabled ( bool state ) {
	mCheck3BtnEmulation -> setChecked ( state );
}
//====================================
// setWheelEmulationEnabled
//------------------------------------
void SCCMouseDisplay::setWheelEmulationEnabled  ( bool state ) {
	mCheckEmulateWheel -> setChecked ( state );
	slotWheelEmulation();
}
//====================================
// setWheelEnabled
//------------------------------------
void SCCMouseDisplay::setWheelEnabled ( bool state ) {
	mCheckMouseWheel -> setChecked ( state );
}
//====================================
// setWheelButton
//------------------------------------
void SCCMouseDisplay::setWheelButton ( int btn ) {
	mEmulateWheelButton -> setValue ( btn );
}
} // end namespace
