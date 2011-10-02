/**************
FILE          : tabletconnection.cpp
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
              : - tabletconnection.h: SaXGUI::SCCTabletConnection implementation
              : - tabletconnection.cpp: configure tablet system
              : ----
              :
STATUS        : Status: Development
**************/
#include "tabletconnection.h"
//Added by qt3to4:
#include <Q3VBoxLayout>

namespace SaXGUI {
//====================================
// Constructor
//------------------------------------
SCCTabletConnection::SCCTabletConnection (
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
	mPortGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["ConnectionPort"],this
	);
	mPortBox = new QComboBox ( mPortGroup );
	mOptionGroup = new Q3ButtonGroup (
		1,Qt::Vertical,mText["TabletOptions"],this
	);
	mOptionList = new Q3ListBox ( mOptionGroup );
	mOptionList -> setSelectionMode ( Q3ListBox::Multi );
	mModeGroup = new Q3ButtonGroup (
		1,Qt::Horizontal,mText["PrimaryTabletMode"],this
	);
	mRelative = new QRadioButton (
		mText["Relative"], mModeGroup
	);
	mAbsolute = new QRadioButton (
		mText["Absolute"], mModeGroup
	);
	//=====================================
	// connect widgets
	//-------------------------------------
	QObject::connect (
		mOptionList , SIGNAL (clicked    ( Q3ListBoxItem*)),
		this        , SLOT   (slotOption ( Q3ListBoxItem*))
	);
	//=====================================
	// add widgets to the layout
	//-------------------------------------
	mMainLayout -> setMargin  ( 15 );
	mMainLayout -> addWidget  ( mPortGroup );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mOptionGroup );
	mMainLayout -> addSpacing ( 15 );
	mMainLayout -> addWidget  ( mModeGroup );
}
//====================================
// init
//------------------------------------
void SCCTabletConnection::init ( void ) {
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//====================================
	// create manipulators... 
	//------------------------------------   
	mSaxTablet = new SaXManipulateTablets (
		mSection["Pointers"],mSection["Layout"]
	);
	//=====================================
	// insert available port device names
	//-------------------------------------
	mPortBox -> insertItem ( mText["USB"] );
	mPortBox -> insertItem ( mText["Serial1"] );
	mPortBox -> insertItem ( mText["Serial2"] );
	mPortBox -> insertItem ( mText["Serial3"] );
	mPortBox -> insertItem ( mText["Serial4"] );
	mPortBox -> insertItem ( mText["Serial5"] );

	//=====================================
	// set default tablet mode to relative
	//-------------------------------------	
	mRelative -> setChecked ( true );
}
//====================================
// import
//------------------------------------
void SCCTabletConnection::import ( void ) {
	// import is done in SCCTabletPens::import()...
}
//====================================
// getPortName
//------------------------------------
QString SCCTabletConnection::getPortName ( void ) {
	return mPortBox->currentText();
}
//====================================
// isAutoPort
//------------------------------------
bool SCCTabletConnection::isAutoPort ( void ) {
	return ! mPortGroup->isEnabled();
}
//====================================
// getTabletMode
//------------------------------------
int SCCTabletConnection::getTabletMode ( void ) {
	if ( mRelative->isChecked() ) {
		return 0;
	} else {
		return 1;
	}
}
//====================================
// getOptions
//------------------------------------
Q3Dict<QString> SCCTabletConnection::getOptions ( void ) {
	mSelectedOptions.clear();
	for (unsigned int i=0;i<mOptionList->count();i++) {
		Q3ListBoxItem* item = mOptionList->item (i);
		if (item->isSelected()) {
			QStringList tokens = QStringList::split ( " >>> ", item->text() );
			QString option = tokens.first();
			QString* value = new QString (tokens.last());
			if (tokens.count() > 1) {
				mSelectedOptions.insert (option,value);
			} else {
				mSelectedOptions.insert (option,new QString);
			}
		}
	}
	return mSelectedOptions;
}
//====================================
// setMode
//------------------------------------
void SCCTabletConnection::setMode ( const QString& mode ) {
	mRelative -> setChecked ( true );
	if (mode == "Absolute") {
		mAbsolute -> setChecked ( true );
	}
}
//====================================
// setPort
//------------------------------------
void SCCTabletConnection::setPort ( const QString& device ) {
	QRegExp identifier ("/dev/input/(by-id|by-path|event|wacom)");
	if (identifier.search (device) >= 0) {
		mPortBox -> setCurrentItem ( 0 );
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
//====================================
// setOptions
//------------------------------------
void SCCTabletConnection::setOptions ( const Q3Dict<QString>& options ) {
	if (! mOptionDict.isEmpty()) {
		mOptionList->clear();
		Q3DictIterator<QString> it (mOptionDict);
		for (; it.current(); ++it) {
			mOptionList->insertItem ( it.currentKey() );
		}
	}
	Q3DictIterator<QString> it (options);
	for (; it.current(); ++it) {
		QString opt;
		if ( it.current()->isEmpty()) {
			QTextOStream (&opt) << it.currentKey();
		} else {
			QTextOStream (&opt) << it.currentKey() << " >>> " << it.current();
		}
		if ( Q3ListBoxItem* item = mOptionList->findItem (it.currentKey()) ) {
			int index = mOptionList->index (item);
			mOptionList -> changeItem  ( opt, index );
			mOptionList -> setSelected ( index, true );
		}
	}
	mOptionList -> ensureCurrentVisible();
}
//====================================
// slotOption
//------------------------------------
void SCCTabletConnection::slotOption ( Q3ListBoxItem* item ) {
	//=====================================
	// check item address
	//-------------------------------------
	if (! item) {
		return;
	}
	//=====================================
	// get translation pointer
	//-------------------------------------
	SCCWrapPointer< Q3Dict<QString> > mText (mTextPtr);

	//=====================================
	// prepare selection information 
	//-------------------------------------
	int index = mOptionList->currentItem();
	QString current = mOptionList->currentText();
	QStringList tokens = QStringList::split ( " >>> ", current );
	QString option = tokens.first();
	QString value  = tokens.last();
	QString flag   = *mOptionDict[option];
	QString message= mText["ValueRequired"];
	message.replace (
		QRegExp("\%1"),option
	);
	QString msgValue;
	QString msgResult;
	//=====================================
	// ask for an option value if needed
	//-------------------------------------
	if (mOptionList->isSelected (index)) {
		if (flag == "integer") {
			//=====================================
			// option requires an integer value
			//-------------------------------------
			SCCMessage* mMessageBox = new SCCMessage (
				qApp->mainWidget(), mTextPtr, SaXMessage::OPT_INT,message,
				"MessageCaption", SaXMessage::Information
			);
			mMessageBox -> setSpinValue ( value.toInt() );
			msgResult = mMessageBox -> showMessage();
			if ( msgResult == mText["Ok"] ) {
				QTextOStream (&msgValue) << mMessageBox -> getSpinValue();
			} else {
				mOptionList->setSelected (index,false );
			}
		} else if (flag == "any") {
			//=====================================
			// option requires a string value
			//-------------------------------------
			SCCMessage* mMessageBox = new SCCMessage (
				qApp->mainWidget(), mTextPtr, SaXMessage::OPT_ANY,message,
				"MessageCaption", SaXMessage::Information
			);
			if (value != option) {
				mMessageBox -> setLineValue ( value );
			}
			msgResult = mMessageBox -> showMessage();
			if ( msgResult == mText["Ok"] ) {
				msgValue = mMessageBox -> getLineValue();
			} else {
				mOptionList->setSelected (index,false );
			}
		} else if (flag.contains("string")) {
			//=====================================
			// option requires a predefined value
			//-------------------------------------
			QStringList tokens = QStringList::split ( "%", flag );
			QStringList values = QStringList::split ( ",", tokens.last() );
			SCCMessage* mMessageBox = new SCCMessage (
				qApp->mainWidget(), mTextPtr, SaXMessage::OPT_STRING,message,
				"MessageCaption", SaXMessage::Information
			);
			mMessageBox -> setComboList ( values );
			if (value != option) {
				mMessageBox -> setComboValue ( value );
			}
			msgResult = mMessageBox -> showMessage();
			if ( msgResult == mText["Ok"] ) {
				msgValue = mMessageBox -> getComboValue();
			} else {
				mOptionList->setSelected (index,false );
			}
		}
	}
	//=====================================
	// update option value if needed
	//-------------------------------------
	if (! msgValue.isEmpty()) {
		QString updateOption;
		QTextOStream (&updateOption) << option << " >>> " << msgValue;
		mOptionList -> changeItem  ( updateOption,index );
		mOptionList -> setSelected ( index,true );
	}
}
//====================================
// slotTablet
//------------------------------------
void SCCTabletConnection::slotTablet (
	const QString& vendor,const QString& name
) {
	//====================================
	// retrieve data record for tablet
	//------------------------------------
	Q3Dict<QString> tabletDict = mSaxTablet->getTabletData ( vendor,name );
	
	//====================================
	// set tablet device
	//------------------------------------
	if (tabletDict["Device"]) {
		QString device = *tabletDict["Device"];
		mPortGroup -> setDisabled ( false );
		if (device == "AUTO") {
			mPortGroup -> setDisabled ( true );
		}
		QRegExp identifier ("/dev/input/(by-id|by-path|event|wacom)");
		if (identifier.search (device) >= 0) {
			mPortBox -> setCurrentItem ( 0 );
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
	//====================================
	// set tablet driver specific options
	//------------------------------------
	if (tabletDict["Driver"]) {
		mOptionList->clear();
		mOptionDict = mSaxTablet->getTabletOptions (
			*tabletDict["Driver"]
		);
		Q3DictIterator<QString> it (mOptionDict);
		for (; it.current(); ++it) {
			mOptionList->insertItem ( it.currentKey() );
		}
	}
	//====================================
	// activate tablet options [Bool]
	//------------------------------------
	if (tabletDict["Option"]) {
		QStringList tokens = QStringList::split (",",*tabletDict["Option"]);
		for ( QStringList::Iterator
			in = tokens.begin(); in != tokens.end(); ++in
		) {
			QString opt (*in);
			if ( Q3ListBoxItem* item = mOptionList->findItem (opt) ) {
				int index = mOptionList->index (item);
				mOptionList -> setSelected ( index, true );
			}
		}
	}
	//====================================
	// activate tablet options [Raw]
	//------------------------------------
	if (tabletDict["RawOption"]) {
		QStringList tokens = QStringList::split (",",*tabletDict["RawOption"]);
		for ( QStringList::Iterator
			in = tokens.begin(); in != tokens.end(); ++in
		) {
			QString opt (*in);
			QString set;
			QStringList items = QStringList::split (" ",opt);
			QString key = items.first();
			QString val = items.last();
			key.replace(QRegExp("\""),"");
			val.replace(QRegExp("\""),"");
			QTextOStream (&set) << key << " >>> " << val;
			if ( Q3ListBoxItem* item = mOptionList->findItem (key) ) {
				int index = mOptionList->index (item);
				mOptionList -> changeItem  ( set, index );
				mOptionList -> setSelected ( index, true );
			}
		}
	}
	//====================================
	// set tablet mode
	//------------------------------------
	if (tabletDict["TabletMode"]) {
		QString mode = *tabletDict["TabletMode"];
		if ( mode == "Absolute" ) {
			mAbsolute -> setChecked ( true );
		} else {
			mRelative -> setChecked ( true );
		}
	}
}
} // end namespace
