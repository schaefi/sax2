/**************
FILE          : tablet.cpp
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
              : - tablet.h: SaXGUI::SCCTablet header definitions
              : - tablet.cpp: configure tablet system
              : ----
              :
STATUS        : Status: Development
**************/
#include "tablet.h"

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCTablet::SCCTablet (
	Q3WidgetStack* stack,Q3Dict<QString>* text,
	Q3Dict<SaXImport> section, QWidget* parent
) : SCCDialog ( stack,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create tablet dialog
	//-------------------------------------
	mTabletTab = new QTabWidget  ( mDialogFrame );

	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mDialogLayout -> addWidget  ( mTabletTab );

	//=====================================
	// create basic tablet tabs
	//-------------------------------------
	mTabletSelection  = new SCCTabletSelection  ( text,section,0,this );
	mTabletConnection = new SCCTabletConnection ( text,section,0,this );
	mTabletPens       = new SCCTabletPens       ( text,section,0,this );
	mTabletTab -> addTab (
		mTabletSelection, mText["Tablet"]
	);
	mTabletTab -> addTab (
		mTabletConnection, mText["TabletPortAndMode"]
	);
	mTabletTab -> addTab (
		mTabletPens, mText["TabletElectronicPens"]
	);
	QObject::connect (
		mTabletSelection, SIGNAL (sigActivate  ( bool )),
		this            , SLOT   (slotActivate ( bool ))
	);
	QObject::connect (
		mTabletSelection , SIGNAL (sigTablet  (const QString&,const QString&)),
		mTabletConnection, SLOT   (slotTablet (const QString&,const QString&))
	);
	QObject::connect (
		mTabletSelection , SIGNAL (sigTablet  (const QString&,const QString&)),
		mTabletPens      , SLOT   (slotTablet (const QString&,const QString&))
	);
	//=====================================
	// initialize basic tablet tabs
	//-------------------------------------
	mTabletSelection   -> init();
	mTabletSelection   -> import();
	mTabletConnection  -> init();
	mTabletConnection  -> import();
	mTabletPens        -> init();
	mTabletPens        -> import();
}
//====================================
// init
//------------------------------------
void SCCTablet::init ( void ) {
	//====================================
	// set init state
	//------------------------------------
	needInitData = false;
}
//====================================
// import
//------------------------------------
void SCCTablet::import ( void ) {
	//====================================
	// create manipulators... 
	//------------------------------------
	SaXManipulateTablets saxTablet (
		mSection["Pointers"],mSection["Layout"]
	);
	SaXManipulateLayout saxLayout (
		mSection["Layout"],mSection["Card"]
	);
	//=====================================
	// search tablet ID
	//-------------------------------------
	int tabletID = 0;
	for (int i=SAX_CORE_POINTER;i<mSection["Pointers"]->getCount();i+=2) {
		if (saxTablet.selectPointer (i)) {
		if (saxTablet.isTablet()) { 
			tabletID = i;
		}
		}
	}
	if ( tabletID == 0 ) {
		mTabletSelection -> setEnabled ( false );
		needImportData = false;
		return;
	}
	//====================================
	// handle tablet's state
	//------------------------------------  
	QList<QString> inputLayout = saxLayout.getInputLayout();
	QString it;
	bool foundID = false;
	foreach (it,inputLayout) {
		if (tabletID == it.toInt()) {
			foundID = true;
			break;
		}
	}
	if ( foundID ) {
		mTabletSelection -> setEnabled ( true );
	} else {
		mTabletSelection -> setEnabled ( false );
	}
	//====================================
	// set import state
	//------------------------------------
	needImportData = false;
}
//====================================
// getSelectionData
//------------------------------------
SCCTabletSelection* SCCTablet::getSelectionData ( void ) {
	return mTabletSelection;
}
//====================================
// getConnectionData
//------------------------------------
SCCTabletConnection* SCCTablet::getConnectionData ( void ) {
	return mTabletConnection;
}
//====================================
// disablePens
//------------------------------------
void SCCTablet::enablePens ( bool state ) {
	mTabletTab -> setTabEnabled ( mTabletPens, state );
}
//====================================
// slotActivate
//------------------------------------
void SCCTablet::slotActivate ( bool state ) {
	mTabletTab -> setTabEnabled ( mTabletConnection, state );
	mTabletTab -> setTabEnabled ( mTabletPens, state );
}
//====================================
// exportData
//------------------------------------
bool SCCTablet::exportData ( void ) {
	//====================================
	// create manipulators... 
	//------------------------------------
	SaXManipulateTablets saxTablet (
		mSection["Pointers"],mSection["Layout"]
	);
	SaXManipulateDevices saxDevice (
		mSection["Pointers"],mSection["Layout"]
	);
	//====================================
	// search and remove tablet
	//------------------------------------
	int inputCount = mSection["Pointers"]->getCount();
	for (int i=inputCount;i>=0;i--) {
		if (saxTablet.selectPointer (i)) {
		if (saxTablet.isTablet()) {
			saxDevice.removeInputDevice (i);
		}
		if (saxTablet.isPen()) {
			saxDevice.removeInputDevice (i);
		}
		if (saxTablet.isEraser()) {
			saxDevice.removeInputDevice (i);
		}
		if (saxTablet.isTouch()) {
			saxDevice.removeInputDevice (i);
		}
		if (saxTablet.isPad()) {
			saxDevice.removeInputDevice (i);
		}
		}
	}
	//====================================
	// add tablet if enabled
	//------------------------------------
	if (mTabletSelection->isEnabled()) {
		QString vendor = mTabletSelection->getVendor();
		QString model  = mTabletSelection->getModel();
		if (! model.isEmpty()) {
			int tabletID = saxDevice.addInputDevice (SAX_INPUT_TABLET);
			saxTablet.selectPointer ( tabletID );
			saxTablet.setTablet ( vendor,model );
			Q3Dict<QString> tabletDict = saxTablet.getTabletData (
				vendor,model
			);
			//====================================
			// save tablet connection port
			//------------------------------------
			if (! mTabletConnection->isAutoPort()) {
				QString port = mTabletConnection->getPortName();
				if (port.contains ("ttyS0")) {
					saxTablet.setDevice ( "/dev/ttyS0" );
				}
				if (port.contains ("ttyS1")) {
					saxTablet.setDevice ( "/dev/ttyS1" );
				}
				if (port.contains ("ttyS2")) {
					saxTablet.setDevice ( "/dev/ttyS2" );
				}
				if (port.contains ("ttyS3")) {
					saxTablet.setDevice ( "/dev/ttyS3" );
				}
				if (port.contains ("ttyS4")) {
					saxTablet.setDevice ( "/dev/ttyS4" );
				}
				if (port.contains ("USB")) {
					QString _device = *tabletDict["Device"];
					if (! _device.isEmpty()) {
						saxTablet.setDevice ( _device );
					} else {
				
						Q3Process* proc = new Q3Process ();
						proc -> addArgument ( USB_PORT );
						if (proc -> start()) {
							while (proc->isRunning()) {
								usleep (1000);
							}
							QByteArray data = proc->readStdout();
							QStringList lines = QStringList::split ("\n",data);
							saxTablet.setDevice ( lines.first() );
						}
					}
				}
			}
			//====================================
			// save tablet options
			//------------------------------------
			QString driver = saxTablet.getDriver();
			Q3Dict<QString> allOptions = saxTablet.getTabletOptions ( driver );
			Q3DictIterator<QString> it (allOptions);
			for (; it.current(); ++it) {
				saxTablet.removeOption (it.currentKey());
			}
			Q3Dict<QString> tabletOptions = mTabletConnection->getOptions();
			Q3DictIterator<QString> io (tabletOptions);
			for (; io.current(); ++io) {
				saxTablet.addOption (io.currentKey(),*io.current());
			}
			//====================================
			// save tablet mode
			//------------------------------------
			int mode = mTabletConnection->getTabletMode();
			if (mode == 0) {
				saxTablet.setMode ("Relative");
			} else {
				saxTablet.setMode ("Absolute");
			}
			int sticks[3] = {-1,-1,-1};
			//====================================
			// handle Tablet Pen
			//------------------------------------
			if (mTabletPens->hasPen()) {
				QString penLink = *tabletDict["StylusLink"];
				sticks[0] = saxTablet.addPen ( vendor,penLink );
			}
			//====================================
			// handle Tablet Eraser
			//------------------------------------
			if (mTabletPens->hasEraser()) {
				QString eraserLink = *tabletDict["EraserLink"];
				sticks[1] = saxTablet.addPen ( vendor,eraserLink );
			}
			//====================================
			// handle Tablet Touch
			//------------------------------------
			if (mTabletPens->hasTouch()) {
				QString TouchLink = *tabletDict["TouchLink"];
				sticks[2] = saxTablet.addPen ( vendor,TouchLink );
			}
			//====================================
			// handle Tablet Pad
			//------------------------------------
			if (mTabletPens->hasPad()) {
				printf ("ADDING PAD\n");
				QString padLink = *tabletDict["PadLink"];
				saxTablet.addPad ( vendor,padLink );
			}
			//====================================
			// save pen/eraser data
			//------------------------------------
			for (int n=0;n<3;n++) {
			if (sticks[n] > 0) {
				SCCTabletPenProperty* pen = 0;
				switch (n) {
					case 0: pen = mTabletPens->getPenPropertyData();    break;
					case 1: pen = mTabletPens->getEraserPropertyData(); break;
					case 2: pen = mTabletPens->getTouchPropertyData();  break;
				}
				//====================================
				// create manipulators... 
				//------------------------------------
				SaXManipulateTablets saxPen (
					mSection["Pointers"],mSection["Layout"]
				);
				saxPen.selectPointer ( sticks[n] );
				//====================================
				// save pen's mode
				//------------------------------------
				int mode = pen->getPenMode();
				if (mode == 0) {
					saxPen.setMode ("Relative");
				} else {
					saxPen.setMode ("Absolute");
				}
				//====================================
				// save pen's options
				//------------------------------------
				QString driver = saxTablet.getDriver();
				Q3Dict<QString> allOptions = saxTablet.getTabletOptions (driver);
				Q3DictIterator<QString> it (allOptions);
				for (; it.current(); ++it) {
					saxPen.removeOption (it.currentKey());
				}
				Q3Dict<QString> penOptions = pen->getPenOptions();
				Q3DictIterator<QString> io (penOptions);
				for (; io.current(); ++io) {
					saxPen.addOption (io.currentKey(),*io.current());
				}
				//====================================
				// save pen's device
				//------------------------------------
				saxTablet.selectPointer ( tabletID );
				QString device = saxTablet.getDevice();
				saxPen.selectPointer ( sticks[n] );
				saxPen.setDevice (device);
			}
			}
		}
	}
	return true;
}
} // end namespace
