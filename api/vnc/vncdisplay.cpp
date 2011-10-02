/**************
FILE          : vncdisplay.cpp
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
              : - vncdisplay.h: SaXGUI::SCCVNCDisplay implementation
              : - vncdisplay.cpp: configure VNC system
              : ---- 
              :
STATUS        : Status: Development
**************/
#include "vncdisplay.h"
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <QLabel>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCVNCDisplay::SCCVNCDisplay (
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
	mCheckVNC = new QCheckBox ( mText["EnableVNC"], this );
	mOptions = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["VNCOptions"],this
	);
	mOptions -> setDisabled (true);
	mCheckPWD = new QCheckBox ( mText["PWDProtect"], mOptions );

	Q3HBox* space = new Q3HBox ( mOptions );
	QLabel* hidden1 = new QLabel ( space );
	hidden1 -> setFixedWidth ( 22 );
	mEntries = new Q3VBox ( space );
	mEntries -> setSpacing (5);
	mEntries -> setDisabled ( true );

	Q3VBox* pwd1 = new Q3VBox ( mEntries );
	QLabel* pwd1text = new QLabel ( pwd1 );
	pwd1text -> setText ( mText["Password"] );
	mPWD1 = new QLineEdit ( pwd1 );
	mPWD1 -> setText ( mText["NewPassword"] );
	mPWD1 -> setMaximumWidth ( 400 );

	Q3VBox* pwd2 = new Q3VBox ( mEntries );
	QLabel* pwd2text = new QLabel ( pwd2 );
	pwd2text -> setText ( mText["PasswordVerify"] );
	mPWD2 = new QLineEdit ( pwd2 );
	mPWD2 -> setEchoMode ( QLineEdit::Password );
	mPWD2 -> setMaximumWidth ( 400 );
	mPWD2 -> setDisabled ( true );

	QLabel* hidden2 = new QLabel ( mEntries );
	hidden2 -> setFixedHeight ( 5 );
	
	mCheckShared   = new QCheckBox ( mText["VNCShared"], mOptions );
	mCheckHTTP = new QCheckBox ( mText["VNCHTTP"], mOptions );
	Q3HBox* space2 = new Q3HBox ( mOptions );
	QLabel* hidden3 = new QLabel ( space2 );
	hidden3 -> setFixedWidth ( 22 );
	mHTTPEntries = new Q3VBox ( space2 );
	mHTTPEntries -> setSpacing (5);
	mHTTPEntries -> setDisabled ( true );

	Q3VBox* http = new Q3VBox ( mHTTPEntries );
	QLabel* httptext = new QLabel ( http );
	httptext -> setText ( mText["HTTPPort"] );
	mHTTP = new QSpinBox (5000,10000,1,http);
	mHTTP -> setValue ( 5800 );
	mHTTP -> setMaximumWidth ( 400 );

	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mCheckVNC , SIGNAL ( toggled         ( bool ) ),
		this      , SLOT   ( slotActivateVNC ( bool ) )
	);
	QObject::connect (
		mCheckPWD , SIGNAL ( toggled         ( bool ) ),
		this      , SLOT   ( slotPassword    ( bool ) )
	);
	QObject::connect (
		mCheckHTTP , SIGNAL ( toggled        ( bool ) ),
		this       , SLOT   ( slotHTTP       ( bool ) )
	);
	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mMainLayout -> setMargin  ( 15 );
	mMainLayout -> addWidget  ( mCheckVNC );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mOptions );
}
//====================================
// init
//------------------------------------
void SCCVNCDisplay::init ( void ) {
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
	mPWD = mText["NewPassword"];
}
//====================================
// import
//------------------------------------
void SCCVNCDisplay::import ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// create VNC manipulator
	//------------------------------------
	SaXManipulateVNC mVNC (
		mSection["Card"],mSection["Pointers"],mSection["Keyboard"],
		mSection["Layout"],mSection["Path"]
	);
	//====================================
	// handle basic VNC switch
	//------------------------------------
	if (mVNC.isVNCEnabled()) {
		mCheckVNC -> setChecked ( true );
	}
	//====================================
	// handle VNC password feature
	//------------------------------------
	if (mVNC.isPwdProtectionEnabled()) {
		mCheckPWD -> setChecked ( true );
		mPWD = mText["ChangePassword"];
		mPWD1 -> setText ( mPWD );
		mPWD1 -> setEchoMode ( QLineEdit::Normal );
		mPWD2 -> setDisabled ( true );
	}
	//====================================
	// handle VNC multi connection feature
	//------------------------------------
	if (mVNC.isMultiConnectEnabled()) {
		mCheckShared -> setChecked ( true );
	}
	//====================================
	// handle VNC HTTP connection feature
	//------------------------------------
	if (mVNC.isHTTPAccessEnabled()) {
		mCheckHTTP -> setChecked ( true );
		mHTTP -> setValue ( mVNC.getHTTPPort() );
	}
	//====================================
	// connect input field
	//------------------------------------
	QObject::connect (
		mPWD1      , SIGNAL ( textChanged       ( const QString &) ),
		this       , SLOT   ( slotInput1Changed ( const QString &) )
	);
}
//====================================
// isEnabled
//------------------------------------
bool SCCVNCDisplay::isEnabled ( void ) {
	return mCheckVNC->isChecked();
}
//====================================
// isHTTPEnabled
//------------------------------------
bool SCCVNCDisplay::isHTTPEnabled ( void ) {
	return mCheckHTTP->isChecked();
}
//====================================
// isPWDProtected
//------------------------------------
bool SCCVNCDisplay::isPWDProtected ( void ) {
	return mCheckPWD->isChecked();
}
//====================================
// isShared
//------------------------------------
bool SCCVNCDisplay::isShared ( void ) {
	return mCheckShared->isChecked();
}
//====================================
// getHTTPPort
//------------------------------------
int SCCVNCDisplay::getHTTPPort ( void ) {
	return mHTTP->value();
}
//====================================
// checkPassword
//------------------------------------
bool SCCVNCDisplay::checkPassword ( void ) {
	if (mCheckPWD -> isOn()) {
		SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);
		if (mPWD2 -> isEnabled()) {
			if (mPWD1->text() != mPWD2->text()) {
				SCCMessage* mMessageBox = new SCCMessage (
					qApp->mainWidget(), mTextPtr, SaXMessage::OK, "PWDMismatch",
					"MessageCaption", SaXMessage::Critical
				);
				mMessageBox -> showMessage();
				mPWD = mText["NewPassword"];
				mPWD1->clear();
				mPWD2->clear();
				mPWD1 -> setText ( mPWD );
				mPWD1 -> setEchoMode ( QLineEdit::Normal );
				mPWD2->setDisabled ( true );
				mPWD1->setFocus();
				return false;
			} else if (mPWD1->text().length() < 6) {
				SCCMessage* mMessageBox = new SCCMessage (
					qApp->mainWidget(), mTextPtr, SaXMessage::OK, "PWDTooShort",
					"MessageCaption", SaXMessage::Critical
				);
				mMessageBox -> showMessage();
				mPWD = mText["NewPassword"];
				mPWD1->clear();
				mPWD2->clear();
				mPWD1 -> setText ( mPWD );
				mPWD1 -> setEchoMode ( QLineEdit::Normal );
				mPWD2->setDisabled ( true );
				mPWD1->setFocus();
				return false;
			} else if (mPWD1->text().length() > 8) {
				SCCMessage* mMessageBox = new SCCMessage (
					qApp->mainWidget(), mTextPtr, SaXMessage::OK, "PWDTooLong",
					"MessageCaption", SaXMessage::Critical
				);
				mMessageBox -> showMessage();
				mPWD = mText["NewPassword"];
				mPWD1->clear(); 
				mPWD2->clear();
				mPWD1 -> setText ( mPWD );
				mPWD1 -> setEchoMode ( QLineEdit::Normal );
				mPWD2->setDisabled ( true );
				mPWD1->setFocus();
				return false;
			} else {
				return true;
			}
		} else {
			if (mPWD == mText["ChangePassword"]) {
				return true;
			} else {
				SCCMessage* mMessageBox = new SCCMessage (
					qApp->mainWidget(), mTextPtr, SaXMessage::OK, "PWDMismatch",
					"MessageCaption", SaXMessage::Critical
				);
				mMessageBox -> showMessage();
				return false;
			}
		}
	}
	return false;
}
//====================================
// getPassword
//------------------------------------
QString SCCVNCDisplay::getPassword ( void ) {
	if (mCheckPWD -> isOn()) {
		return mPWD1->text();
	}
	QString* nope = new QString;
	return *nope;
}
//====================================
// slotActivateVNC
//------------------------------------
void SCCVNCDisplay::slotActivateVNC ( bool on ) {
	if (on) {
		mOptions -> setDisabled (false);
	} else {
		mOptions -> setDisabled (true);
	}
}
//====================================
// slotPassword
//------------------------------------
void SCCVNCDisplay::slotPassword ( bool on ) {
	if (on) {
		mEntries -> setDisabled (false);
	} else {
		mEntries -> setDisabled (true);
	}
}
//====================================
// slotHTTP
//------------------------------------
void SCCVNCDisplay::slotHTTP ( bool on ) {
	if (on) {
		mHTTPEntries -> setDisabled (false);
	} else {
		mHTTPEntries -> setDisabled (true);
	}
}
//====================================
// slotInput1Changed
//------------------------------------
void SCCVNCDisplay::slotInput1Changed (const QString& data) {
	if (mPWD1 -> echoMode() != QLineEdit::Password) {
		mPWD1 -> setEchoMode ( QLineEdit::Password );
		mPWD1 -> clear();
		if (! mPWD2 -> isEnabled()) {
			mPWD2 -> setDisabled (false);
		}
		QChar start;
		for (unsigned int i=0;i< (unsigned int)data.length();i++) {
		if (data.at(i) != mPWD.at(i)) {
			start = data.at(i);
			break;
		}
		}
		mPWD1 -> setText (start);
	}
}
} // end namespace
