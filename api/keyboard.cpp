/**************
FILE          : keyboard.cpp
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
              : - keyboard.h: SaXGUI::SCCKeyboard implementation
              : - keyboard.cpp: configure keyboard system
              : ----
              :
STATUS        : Status: Development
**************/
#include "keyboard.h"
//Added by qt3to4:
#include <QLabel>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCKeyboard::SCCKeyboard (
	Q3WidgetStack* stack,Q3Dict<QString>* text,
	Q3Dict<SaXImport> section, QWidget* parent
) : SCCDialog ( stack,text,section,parent ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// create keyboard dialog
	//-------------------------------------
	mKeyboardTab   = new QTabWidget  ( mDialogFrame );
	Q3HBox* testBox = new Q3HBox       ( mDialogFrame );
	mTestLabel     = new QLabel      ( mText["Test"],testBox );
	testBox -> setSpacing ( 10 );
	mTestField     = new QLineEdit   ( testBox );

	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mDialogLayout -> addWidget  ( mKeyboardTab );
	mDialogLayout -> addSpacing ( 15 );
	mDialogLayout -> addWidget  ( testBox );

	//=====================================
	// create basic keyboard tabs
	//-------------------------------------
	mKeyboardLayout  = new SCCKeyboardLayout  ( text,section,0,this );
	mKeyboardOptions = new SCCKeyboardOptions ( text,section,0,this );
	mKeyboardTab -> addTab (
		mKeyboardLayout , mText["XKBLayout"] 
	);
	#if 1
	mKeyboardTab -> addTab (
		mKeyboardOptions, mText["XKBOptions"]
	);
	#endif
	QObject::connect (
		mKeyboardLayout , SIGNAL (sigApply ( void )),
		this            , SLOT   (apply    ( void ))
	);
	QObject::connect (
		mKeyboardOptions, SIGNAL (sigApply ( void )),
		this            , SLOT   (apply    ( void ))
	);
	//=====================================
	// initialize basic keyboard tabs
	//-------------------------------------
	mKeyboardLayout  -> init();
	mKeyboardLayout  -> import();
	mKeyboardOptions -> init();
	mKeyboardOptions -> import();
}
//====================================
// init
//------------------------------------
void SCCKeyboard::init ( void ) {
	//====================================
	// set init state
	//------------------------------------
	needInitData = false;
}
//====================================
// import
//------------------------------------
void SCCKeyboard::import ( void ) {
	//====================================
	// set import state
	//------------------------------------
	needImportData = false;
}
//====================================
// apply default keyboard
//------------------------------------
void SCCKeyboard::applyDefault ( void ) {
	qApp->setOverrideCursor ( Qt::forbiddenCursor );
	QString layoutApply = mKeyboardLayout  -> getDefaultLayout();
	QString optionApply = mKeyboardOptions -> getDefaultOption();
	QString optionReset = "-option \"\"";
	QString complete = "setxkbmap " +
		layoutApply + " " + optionReset + " " + optionApply;
	log (L_INFO,"Apply default keyboard: %s\n",complete.ascii());
	Q3Process* proc = new Q3Process ();
	proc -> addArgument ("/bin/bash");
	proc -> addArgument ("-c");
	proc -> addArgument (complete);
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
// apply keyboard
//------------------------------------
void SCCKeyboard::apply ( void ) {
	if (mKeyboardTab->isVisible()) {
		qApp->setOverrideCursor ( Qt::forbiddenCursor );
		QString layoutApply = mKeyboardLayout  -> getApplyString();
		QString optionApply = mKeyboardOptions -> getApplyString();
		QString optionReset = "-option \"\"";
		QString complete = "setxkbmap " +
			layoutApply + " " + optionReset + " " + optionApply;
		log (L_INFO,"Apply keyboard: %s\n",complete.ascii());
		Q3Process* proc = new Q3Process ();
		proc -> addArgument ("/bin/bash");
		proc -> addArgument ("-c");
		proc -> addArgument (complete);
		if ( ! proc -> start() ) {
			qApp->restoreOverrideCursor();
			return;
		}
		while (proc->isRunning()) {
			usleep (1000);
		}
		qApp->restoreOverrideCursor();
	}
}
//====================================
// exportData
//------------------------------------
bool SCCKeyboard::exportData ( void ) {
	//====================================
	// create Manipulator
	//------------------------------------
	SaXManipulateKeyboard saxKeyboard (
		mSection["Keyboard"]
	);
	//====================================
	// retrieve dialog data
	//------------------------------------
	QString type    = mKeyboardLayout  -> getType();
	QString layout  = mKeyboardLayout  -> getLayout();
	QString variant = mKeyboardLayout  -> getVariant();
	QString option  = mKeyboardOptions -> getOptions();

	//====================================
	// save keyboard model/type
	//------------------------------------
	saxKeyboard.setXKBModel  ( type );

	int count = 0;
	int kbcnt = 0;
	//====================================
	// save keyboard layout(s)
	//------------------------------------
	QStringList variants = QStringList::split ( ",",variant,true );
	QStringList layouts  = QStringList::split ( ",",layout );
	for (QStringList::Iterator it=layouts.begin();it!=layouts.end();++it) {
		QString layout  (*it);
		QString variant (variants[count]);
		if (count == 0) {
			saxKeyboard.setXKBLayout ( layout );
			saxKeyboard.setXKBVariant (layout,variant);
		} else {
			saxKeyboard.addXKBLayout ( layout );
			if (variant.isEmpty()) {
				variant = ",";
			}
			saxKeyboard.setXKBVariant (layout,variant);
		}
		count++;
	}
	kbcnt = count;
	count = 0;
	//====================================
	// save keyboard options
	//------------------------------------
	QStringList options = QStringList::split ( ",",option );
	for (QStringList::Iterator it=options.begin();it!=options.end();++it) {
		QString option (*it);
		if (count == 0) {
			saxKeyboard.setXKBOption ( option );
		} else {
			saxKeyboard.addXKBOption ( option );
		}
		count++;
	}
	//====================================
	// add keyboard toggle key
	//------------------------------------
	if (kbcnt == 1) {
		saxKeyboard.removeXKBOption ("grp:alt_shift_toggle");
	}
	if ((kbcnt > 1) && (option.isEmpty())) {
		saxKeyboard.setXKBOption ( "grp:alt_shift_toggle" );
	}
	//====================================
	// run yast keyboard module 
	//------------------------------------
	qApp->setOverrideCursor ( Qt::forbiddenCursor );
	QString baseLayout = *layouts.begin();
	QString baseVariant = *variants.begin();
	QString complete = "/usr/share/sax/api/macros/setconsolekeyboard.sh " 
		+ baseLayout + " " + baseVariant;
	log (L_INFO,"Run YaST keyboard module (console keyboard)\n");
	Q3Process* proc = new Q3Process ();
	proc -> addArgument ("/bin/bash");
	proc -> addArgument ("-c");
	proc -> addArgument (complete);
	proc -> start();
	while (proc->isRunning()) {
		usleep (1000);
	}
	qApp->restoreOverrideCursor();
	return true;
}
} // end namespace
